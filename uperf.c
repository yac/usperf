#include "uperf.h"
#include <stdlib.h>

struct uperf_s uperf;

void
uperf_init()
{
	uperf.last = NULL;
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
			edge->pred = uperf.last;		
			new = 1;
		}

		if (edge->pred == uperf.last || new) {
			// found the edge
			
			// read the counter here

			break;
		}

		edge++;
	}

	uperf.last = point;
}
