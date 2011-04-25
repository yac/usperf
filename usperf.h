/** @file
 * usperf userspace interface.
 *
 * USPERF_* macros are just wrappers around the same named usperf_* functions
 * that are defined only when USPERF is defined (your app compiled with
 * -DUSPERF).
 *
 * USPERF_*_S macros use global struct usperf_s as the first argument for usperf_
 * functions making them easier to type and unusable in multi-threaded
 * applications.
 */
#ifndef _USPERF_H
#define _USPERF_H

#include <stdio.h>
#include <stdint.h>

#include <linux/perf_event.h>

#define SYSTEM_RATIO 10

enum usperf_print_formats {
	USPERF_PRINT_DEFAULT = 0,
	USPERF_PRINT_DOT,
};

struct pcounter {
	int fd;
	void *base;
	char state;
	int prev;
};

struct perfpoint_edge_s {
	uint32_t user_count;
	uint64_t user_sum;
	uint32_t system_count;
	uint64_t system_sum;
};

struct usperf_s {
	int64_t last_count;
	int last_point;
	struct perfpoint_edge_s *points;

	int perfpoints_max;
	int edges_max;

	struct pcounter cnt;
	int counter_type;
};

///// FUNCTION DECLARATIONS /////

void perfpoint(struct usperf_s * usperf, int index);
int usperf_init(struct usperf_s * usperf, int perfpoints_max, int counter_type);
void usperf_print(struct usperf_s * usperf, FILE *stream, int format, const char * (*point_name_fnc)(int));
void usperf_close(struct usperf_s * usperf);

///// MACRO WRAPPERS (depending on USPERF) /////

// for single thread macros
extern struct usperf_s usperf_global;

#ifdef USPERF

#define PERFPOINT(usperf, index)                          perfpoint(usperf, index)
#define USPERF_INIT(usperf, perfpoints_max, counter_type)  usperf_init(usperf, perfpoints_max, counter_type)
#define USPERF_PRINT(usperf, stream, format, point_names)  usperf_print(usperf, stream, format, point_names)
#define USPERF_CLOSE(usperf)                               usperf_close(usperf)

#define PERFPOINT_S(index)                               perfpoint(&usperf_global, index)
#define USPERF_INIT_S(perfpoints_max, counter_type)       usperf_init(&usperf_global, perfpoints_max, counter_type)
#define USPERF_PRINT_S(stream, format, point_names)       usperf_print(&usperf_global, stream, format, point_names)
#define USPERF_CLOSE_S                                    usperf_close(&usperf_global)

#else

#define PERFPOINT(usperf, index)
#define USPERF_INIT(usperf, perfpoints_max, counter_type)  0
#define USPERF_PRINT(usperf, stream, format, point_names)
#define USPERF_CLOSE(usperf)

#define PERFPOINT_S(index)
#define USPERF_INIT_S(perfpoints_max, counter_type)       0
#define USPERF_PRINT_S(stream, format, point_names)
#define USPERF_CLOSE_S

#endif


// These are likely none of your bussiness, move along.
int pcounter_init(struct pcounter *cnt, unsigned int counter_type);
void pcounter_enable(struct pcounter *cnt);
uint64_t pcounter_get(struct pcounter *cnt);
void pcounter_close(struct pcounter *cnt);

#endif
