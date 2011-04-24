#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include "../usperf.h"

#define error(format, ...) { printf(format "\n", ## __VA_ARGS__); perror("errno msg"); exit(-1); }

int
main()
{
	struct pcounter cntr;
	uint64_t count;
	printf("\n");

	int init = pcounter_init(&cntr, PERF_COUNT_SW_CPU_CLOCK);
	if (init == -2) {
		printf("mmap() FAILED. Let's try reading the file descriptor.\n");
		pcounter_enable(&cntr);

		int r = read(cntr.fd, &count, sizeof(count));
		if (r > 0) {
			printf("read(%d) succeeded.\n", cntr.fd);
		}
		else {
			printf("read(%d) FAILED.\n", cntr.fd);
		}
	}

	if (init != 0)
		error("Init failed: %d\n", init);

	pcounter_enable(&cntr);

	count = pcounter_get(&cntr);

	printf("Got counter: %ld\n", count);

	pcounter_close(&cntr);

	return 0;
}
