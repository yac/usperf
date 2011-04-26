/** @file
 * Performance counter abstraction using perf events.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>                                                                                                             
#include <unistd.h>
#include <asm/unistd.h>
#include <asm/unistd_64.h>
#include <sys/types.h>
#include <linux/types.h>
#include <linux/ioctl.h>
#include <asm/byteorder.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <error.h>
#include <fcntl.h>

#include "usperf.h"


#define __NR_perf_event_open          298

// 32: #define __NR_perf_event_open          336
// 64: #define __NR_perf_event_open          298

#if defined(__i386__)
#define rmb()		asm volatile("lock; addl $0,0(%%esp)" ::: "memory")
#define cpu_relax()	asm volatile("rep; nop" ::: "memory");
#endif

#if defined(__x86_64__)
#define rmb()		asm volatile("lfence" ::: "memory")
#define cpu_relax()	asm volatile("rep; nop" ::: "memory");
#endif

/* Optimization barrier */
/* The "volatile" is due to gcc bugs */
#define barrier() __asm__ __volatile__("": : :"memory")


inline int
sys_perf_event_open(struct perf_event_attr *attr,
              pid_t pid, int cpu, int group_fd,
              unsigned long flags)
{
    attr->size = sizeof(*attr);
    return syscall(__NR_perf_event_open, attr, pid, cpu,
               group_fd, flags);
}


inline uint64_t rdpmc(uint32_t reg)
{
	uint64_t val;

	asm("rdpmc" : "=A" (val) : "c" (reg));

	return val;
}


static unsigned int 	mmap_pages	= 128;
static unsigned int		page_size = 0;


struct perf_event_attr default_event_attr = {

	.type = PERF_TYPE_HARDWARE,
	.config = PERF_COUNT_HW_INSTRUCTIONS,

	.size = sizeof(struct perf_event_attr),

	.read_format    = 0,
	.disabled       = 1,
	.inherit        = 0, /* children inherit it   */
	.pinned	        = 1, /* must always be on PMU */
	.exclusive      = 1, /* only group on PMU     */
	.exclude_user   = 0, /* don't count user      */
	.exclude_kernel = 0, /* ditto kernel          */
	.exclude_hv     = 0, /* ditto hypervisor      */
	.exclude_idle   = 0, /* don't count when idle */
	.mmap           = 1, /* include mmap data     */
	.comm	       	= 0, /* include comm data     */
	.freq           = 1, /* use freq, not period  */
	.inherit_stat   = 1, /* per task counts       */
	.enable_on_exec = 0, /* next exec enables     */
	.task           = 0, /* trace fork/exit       */
};

/** Init performance counter.
 *
 * @param[in,out] cnt pcounter to init
 * @param[in] counter_type `<tt>grep PERF_COUNT_HW /usr/include/linux/perf_event.h</tt>` for possible values. The two most popular ones:
 * @li PERF_COUNT_HW_CPU_CYCLES - count CPU cycles
 * @li PERF_COUNT_HW_INSTRUCTION - count instructions
 * @return 0 on success, -1 on perf syscall error, -2 on mmap error
 */
int pcounter_init(struct pcounter *cnt, unsigned int counter_type)
{
	struct perf_event_attr event_attr;

	cnt->base = NULL;
	cnt->prev = 0;

	memcpy(&event_attr, &default_event_attr, sizeof(struct perf_event_attr));

	event_attr.config = counter_type;

	cnt->fd = sys_perf_event_open(&event_attr, getpid(), -1, -1, 0);
	if( cnt->fd < 0 )
		return (cnt->state = -1);

	/*fcntl(cnt->fd, F_SETFL, O_NONBLOCK);*/

	if( ! page_size )
		page_size = sysconf(_SC_PAGE_SIZE);

	cnt->base = mmap(NULL, (mmap_pages + 1) * page_size, PROT_READ | PROT_WRITE, MAP_SHARED, cnt->fd, 0);
	if (cnt->base == MAP_FAILED)
		return (cnt->state = -2);

	return cnt->state = 0;
}

/// Enable the performance counter.
void pcounter_enable(struct pcounter *cnt)
{
	ioctl(cnt->fd, PERF_EVENT_IOC_ENABLE);
}

static uint64_t mmap_read_head(struct pcounter *cnt)
{
	struct perf_event_mmap_page *pc = cnt->base;
	long head;

	head = pc->data_head;
	rmb();

	return head;
}

static void mmap_write_tail(struct pcounter *cnt, uint64_t tail)
{
	struct perf_event_mmap_page *pc = cnt->base;

	pc->data_tail = tail;
}

/** 
 * Read the perf ring-buffer.
 *
 * Doesn't do all that much at the moment.
 */
static void pcounter_mmap_read(struct pcounter *cnt)
{
	unsigned int head = mmap_read_head(cnt);
	unsigned int old = cnt->prev;
	unsigned char *data = (unsigned char *)cnt->base + page_size;
	int diff;

	/*
	 * If we're further behind than half the buffer, there's a chance
	 * the writer will bite our tail and mess up the samples under us.
	 *
	 * If we somehow ended up ahead of the head, we got messed up.
	 *
	 * In either case, truncate and restart at head.
	 */
	diff = head - old;
	if (diff > ((int)mmap_pages - 1) / 2 || diff < 0) {
		fprintf(stderr, "WARNING: failed to keep up with mmap data.\n");

		// head points to a known good entry, start there.
		old = head;
	}

	for (; old != head;) {
		struct perf_event_header *event = (struct perf_event_header *)&data[old];

		size_t size = event->size;

		printf("[EVENT] type: %d\n", event->type); 
		/*if (event->header.type == PERF_RECORD_SAMPLE)*/
			/*event__process_sample(event, self, md->counter);*/
		/*else*/
			/*event__process(event, self);*/
		old += size;
	}

	mmap_write_tail(cnt, old);

	cnt->prev = old;
}

/// Get the performance counter value.
uint64_t pcounter_get(struct pcounter *cnt)
{
	uint64_t count;
	uint32_t seq;
	struct perf_event_mmap_page *pc = cnt->base;


	do {
		seq = pc->lock;

		barrier();

		if (pc->index) {
			count = rdpmc(pc->index - 1);
			count += pc->offset;
		}
		else {
			// TODO: consider putting normal read here
			printf("No HW counter for this perf event!\n");
			return -1;
		}

		barrier();

	} while (pc->lock != seq);

	/*pcounter_mmap_read(cnt);*/
	
	return count;
}

/// Close the performance counter.
void pcounter_close(struct pcounter *cnt)
{
	munmap(cnt->base, (mmap_pages + 1) * page_size);
	close(cnt->fd);
}
