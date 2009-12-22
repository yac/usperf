#ifndef _UPERF_H
#define _UPERF_H

#define PERFPOINTS_MAX 32
#define PERFPOINT_EDGES_MAX 8

#define PERFPOINT(x) perfpoint(uperf.points + x)


struct perfpoint_edge_s {
	struct perfpoint_s *pred;
	int count;
	long int   user_sum;
	long int system_sum;
}
perfpoint_edge_t;

typedef struct perfpoint_edge_s perfpoint_t[PERFPOINT_EDGES_MAX];

struct uperf_s {
	perfpoint_t *last;
	perfpoint_t points[PERFPOINTS_MAX];
};

struct uperf_s uperf;


///// FUNCTION DECLARATIONS /////
void perfpoint(perfpoint_t *point);

#endif
