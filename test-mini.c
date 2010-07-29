#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include "uperf.h"

#define error(format, ...) { printf(format "\n", ## __VA_ARGS__); perror("errno msg"); exit(-1); }

int
/*main(int argc, char *argv[])*/
main()
{
	struct pcounter cntr;
	uint64_t count;
	printf("\n");

	int init = pcounter_init(&cntr, PERF_COUNT_HW_CPU_CYCLES);
	if (init == -2) {
		printf("mmap() FAILED. Let's try reading the file descriptor.\n");
		pcounter_enable(&cntr);

		int r = read(cntr.fd, &count, sizeof(count));
		if (r > 0) {
			printf("OK, at least SOMETHING was read from %d. fd:\n", cntr.fd);
			printf("First value: %ld\n", count);
			read(cntr.fd, &count, sizeof(count));
			printf("Second value: %ld\n", count);
			read(cntr.fd, &count, sizeof(count));
			printf("And one more to be sure: %ld\n", count);
		}
		else {
			printf("No, not event the fucking read().\n");
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
