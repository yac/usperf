#ifndef _PERF_H
#define _PERF_H

//#include "include/linux/perf_event.h"
#include <linux/perf_event.h>

struct pcounter {
	int fd;
	void *base;
	char state;
	int prev;
};

int pcounter_test();
int pcounter_init(struct pcounter *cnt, unsigned int counter_type);
void pcounter_enable(struct pcounter *cnt);
uint64_t pcounter_get(struct pcounter *cnt);
void pcounter_close(struct pcounter *cnt);

#endif
