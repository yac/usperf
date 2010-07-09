#ifndef _UPERF_H
#define _UPERF_H

#include <stdio.h>
#include <stdint.h>

#include "perf.h"


#define PERFPOINTS_MAX 128
#define PERFPOINT_EDGES_MAX (PERFPOINTS_MAX * PERFPOINTS_MAX)
#define SYSTEM_RATIO 10

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
	int64_t last_count;
	int last_point;
	struct perfpoint_edge_s *points;

	int perfpoints_max;
	int edges_max;

	struct pcounter cnt;
	int counter_type;
};

///// FUNCTION DECLARATIONS /////

void perfpoint(struct uperf_s * uperf, int index);
int uperf_init(struct uperf_s * uperf, int perfpoints_max, int counter_type);
void uperf_print(struct uperf_s * uperf, FILE *stream, int format);
void uperf_close(struct uperf_s * uperf);

///// MACRO WRAPPERS (depending on UPERF) /////

// for single thread macros
extern struct uperf_s uperf_global;

#ifdef UPERF

#define PERFPOINT(uperf, index)                          perfpoint(uperf, index)
#define UPERF_INIT(uperf, perfpoints_max, counter_type)  uperf_init(uperf, perfpoints_max, counter_type)
#define UPERF_PRINT(uperf, stream, format)               uperf_print(uperf, stream, format)
#define UPERF_CLOSE(uperf)                               uperf_close(uperf)

#define PERFPOINT_S(index)                               perfpoint(&uperf_global, index)
#define UPERF_INIT_S(perfpoints_max, counter_type)       uperf_init(&uperf_global, perfpoints_max, counter_type)
#define UPERF_PRINT_S(stream, format)                    uperf_print(&uperf_global, stream, format)
#define UPERF_CLOSE_S                                    uperf_close(&uperf_global)

#else

#define PERFPOINT(uperf, index)
#define UPERF_INIT(uperf, perfpoints_max, counter_type)  0
#define UPERF_PRINT(uperf, stream, format)
#define UPERF_CLOSE(uperf)

#define PERFPOINT_S(index)
#define UPERF_INIT_S(perfpoints_max, counter_type)       0
#define UPERF_PRINT_S(stream, format)
#define UPERF_CLOSE_S

#endif



/*
#define PERFPOINT(uperf, index) #ifdef UPERF_ENABLE \
	perfpoint(uperf, index) \
#endif
*/

#endif
