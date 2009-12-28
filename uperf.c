#include "uperf.h"
#include <stdlib.h>
#include <stdio.h>


#if !(defined(__i386) || defined(__ia64) || defined(__amd64) )
#error "Not a right architecture."
#endif


struct uperf_s uperf;



static inline uint64_t
get_timer()
{
    uint32_t a, d;
    asm volatile (
		"RDTSC" 
		: "=a" (a), "=d" (d));
    return ((uint64_t)a) | (((uint64_t)d) << 32);
}

static inline uint64_t
get_count()
{
	uint64_t timer = get_timer();
	uint64_t delta = timer - uperf.last_count; 
	uperf.last_count = timer;

	return delta;
}

int
addr2pnum(void * addr)
{
	return (int)(((uint64_t)addr - (uint64_t)(void *)(&(uperf.points)))) / sizeof(perfpoint_t);
}

void
uperf_init()
{
	// HACK: entry point - any address not in uperf.points[] will do here
	uperf.last_point = (perfpoint_edge_t *)&(uperf.last_point);
	uperf.last_count = get_timer();

	perfpoint_edge_t *edge;

	for( int i = 0; i < PERFPOINT_EDGES_MAX; i++) {

		edge = (perfpoint_edge_t *)(uperf.points[i]);
		for( int j = 0; j < PERFPOINT_EDGES_MAX; j++) {
			edge->pred = NULL;
			edge->count = 0;
			edge->user_sum = 0;
			edge->system_sum = 0;

			edge++;
		}
	}
}

void
uperf_print()
{
	perfpoint_edge_t *edge;

	for( int i = 0; i < PERFPOINT_EDGES_MAX; i++) {
		edge = (perfpoint_edge_t *)(uperf.points[i]);
		for( int j = 0; j < PERFPOINT_EDGES_MAX; j++) {
			if( edge->pred == NULL )
				break;

			printf("%3d => %2d: %dx, %ld avg cycles\n", addr2pnum(edge->pred), i, edge->count, edge->user_sum / edge->count);

			edge++;
		}
	}
}

void
perfpoint(perfpoint_t *point)
{
	perfpoint_edge_t *edge = (perfpoint_edge_t *)point;

	int i;
	int new = 0;

	for( i = 0; i < PERFPOINT_EDGES_MAX; i++) {

		if (edge->pred == NULL) {
			// new edge
			edge->pred = uperf.last_point;		
			new = 1;
		}

		if (edge->pred == uperf.last_point || new) {
			// found the edge
			edge->count += 1;
			edge->user_sum += get_count(); 

			// read the counter here

			break;
		}

		edge++;
	}

	uperf.last_point = (perfpoint_edge_t *)point;
}
