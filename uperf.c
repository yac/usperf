#include "uperf.h"
#include <stdlib.h>
#include <stdio.h>


#if !(defined(__i386) || defined(__ia64) || defined(__amd64) )
#error "Not a right architecture."
#endif


inline uint64_t
get_timer(struct uperf_s * uperf)
{
	return pcounter_get(&(uperf->cnt));
}

inline uint64_t
get_count(struct uperf_s * uperf)
{
	uint64_t timer = get_timer(uperf);
	uint64_t delta = timer - uperf->last_count; 
	uperf->last_count = timer;

	return delta;
}

int
uperf_init(struct uperf_s * uperf, int counter_type)
{
	struct perfpoint_edge_s *edge;

	if( pcounter_init(&(uperf->cnt), counter_type) != 0 )
		return uperf->cnt.state;

	edge = uperf->points;
	for( int i = 0; i < PERFPOINT_EDGES_MAX; i++) {
		edge->user_count = 0;
		edge->user_sum = 0;
		edge->system_count = 0;
		edge->system_sum = 0;

		edge++;
	}

	pcounter_enable(&(uperf->cnt));

	// NOTE: 0 is used for entry point
	uperf->last_point = 0;
	uperf->last_count = get_timer(uperf);
	uperf->counter_type = counter_type;

	return uperf->cnt.state;
}

const char UPERF_DOT_HEAD[] = "digraph \"uperf\" {\n"
"	rankdir=UD\n"
"	size = \"6,6\"\n"
"	node [ fontname = \"Helvetica\" ];\n";

const char UPERF_DOT_TAIL[] = "}\n";


void
uperf_print(struct uperf_s * uperf, FILE *stream, int format)
{
	uint64_t uavg, savg;
	struct perfpoint_edge_s *edge;

	edge = uperf->points;

	if( format == UPERF_PRINT_DOT ) {
		fprintf(stream, UPERF_DOT_HEAD);
	}

	for( int i = 0; i < PERFPOINTS_MAX; i++) {
		for( int j = 0; j < PERFPOINTS_MAX; j++) {

			if( edge->user_count > 0 ) {

				uavg = edge->user_sum / edge->user_count;

				if( edge->system_count > 0 )
					savg = edge->system_sum / edge->system_count;
				else
					savg = 0;

				if( format == UPERF_PRINT_DOT ) {
					fprintf(stream,  "	\"%d\" -> \"%d\" [ label = \"%ld (x%d = %ld)\" ];\n",
							j, i, uavg, edge->user_count, edge->user_sum);
				}
				else {
					fprintf(stream, "%3i => %2d: %8dx usr: %8ld avg, %12ld sum | %6dx sys: %10ld avg\n",
							j, i, edge->user_count, uavg, edge->user_sum, edge->system_count, savg);
				}
			}

			edge++;
		}
	}

	if( format == UPERF_PRINT_DOT ) {
		fprintf(stream, UPERF_DOT_TAIL);
	}
}

void
perfpoint(struct uperf_s * uperf, int index)
{
	struct perfpoint_edge_s *edge;
   
	edge = uperf->points + (index * PERFPOINTS_MAX) + uperf->last_point;

	uint64_t cnt = get_count(uperf);

	// TODO: insert awesome system time detection here
	edge->user_count += 1;
	edge->user_sum += cnt;

	uperf->last_point = index;
}

void
uperf_close(struct uperf_s * uperf)
{
	pcounter_close(&(uperf->cnt));
}
