#ifndef _UPERF_H
#define _UPERF_H

#include <stdint.h>

#define PERFPOINTS_MAX 32
#define PERFPOINT_EDGES_MAX 8

#define PERFPOINT(x) perfpoint(uperf.points + x)



typedef struct perfpoint_edge_s {
	struct perfpoint_edge_s *pred;
	uint32_t count;
	uint64_t user_sum;
	uint64_t system_sum;
}
perfpoint_edge_t;

typedef perfpoint_edge_t perfpoint_t[PERFPOINT_EDGES_MAX];

struct uperf_s {
	long int last_count;
	perfpoint_edge_t *last_point;
	perfpoint_t points[PERFPOINTS_MAX];
};

struct uperf_s uperf;


///// FUNCTION DECLARATIONS /////
void uperf_init();
void uperf_print();
void perfpoint(perfpoint_t *point);

#endif
