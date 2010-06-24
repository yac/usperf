#ifndef _UPERF_H
#define _UPERF_H

#include <stdio.h>
#include <stdint.h>

#include "perf.h"


#define PERFPOINTS_MAX 128
#define PERFPOINT_EDGES_MAX (PERFPOINTS_MAX * PERFPOINTS_MAX)
#define SYSTEM_RATIO 10

#define PERFPOINT(x) perfpoint(uperf.points + x)

enum uperf_print_formats {
	UPERF_PRINT_DEFAULT = 0,
	UPERF_PRINT_DOT,
};

struct perfpoint_edge_s {
	uint32_t user_count;
	uint64_t user_sum;
	uint32_t system_count;
	uint64_t system_sum;
};

struct uperf_s {
	long int last_count;
	int last_point;
	struct perfpoint_edge_s points[PERFPOINT_EDGES_MAX];

	struct pcounter cnt;
	int counter_type;
};

///// FUNCTION DECLARATIONS /////

void perfpoint(struct uperf_s * uperf, int index);
int uperf_init(struct uperf_s * uperf, int counter_type);
void uperf_print(struct uperf_s * uperf, FILE *stream, int format);
void uperf_close(struct uperf_s * uperf);

#endif
