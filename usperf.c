/** @file
 * usperf userspace interface.
 */
#include "usperf.h"
#include <stdlib.h>


#if !(defined(__i386) || defined(__ia64) || defined(__amd64) )
#error "Not a right architecture."
#endif

/// Global evil for USPERF_*_S single-thread macros.
struct usperf_s usperf_global;

/// Get raw timer data.
inline uint64_t
get_timer(struct usperf_s * usperf)
{
	return pcounter_get(&(usperf->cnt));
}

/// Get count since last perfpoint.
inline uint64_t
get_count(struct usperf_s * usperf)
{
	uint64_t timer = get_timer(usperf);
	uint64_t delta = timer - usperf->last_count; 
	usperf->last_count = timer;

	return delta;
}

/**
 * Init usperf structure.
 *
 * Prepare supplied struct usperf_s - init HW perf counter and alloc perfpoints
 * array. Note that (perfpoints_max + 1)^2 array of <tt>struct perfpoint_edge_s</tt>
 * (24 bytes at time of writing) will be allocated.
 *
 * @param[in,out] usperf usperf structure to init
 * @param[in] perfpoints_max max number of perfpoints
 * @param[in] counter_type what to count - <tt>`grep PERF_COUNT_HW /usr/include/linux/perf_event.h`</tt> for possible values. The two most popular ones:
 * @li PERF_COUNT_HW_CPU_CYCLES - count CPU cycles
 * @li PERF_COUNT_HW_INSTRUCTION - count instructions
 * @return 0 on success, see perf.c:pcounter_init() for error values
 */
int
usperf_init(struct usperf_s * usperf, int perfpoints_max, int counter_type)
{
	struct perfpoint_edge_s *edge;

	if( pcounter_init(&(usperf->cnt), counter_type) != 0 )
		return usperf->cnt.state;

	usperf->perfpoints_max = perfpoints_max + 1;
	usperf->edges_max = perfpoints_max * perfpoints_max;
	usperf->points = malloc(sizeof(struct perfpoint_edge_s) * usperf->edges_max);

	edge = usperf->points;
	for( int i = 0; i < PERFPOINT_EDGES_MAX; i++) {
		edge->user_count = 0;
		edge->user_sum = 0;
		edge->system_count = 0;
		edge->system_sum = 0;

		edge++;
	}

	pcounter_enable(&(usperf->cnt));

	// NOTE: 0 is used for entry point
	usperf->counter_type = counter_type;
	usperf->last_point = 0;
	usperf->last_count = get_timer(usperf);

	return usperf->cnt.state;
}

const char USPERF_DOT_HEAD[] = "digraph \"usperf\" {\n"
"	rankdir=UD\n"
"	size = \"6,6\"\n"
"	node [ fontname = \"Helvetica\" ];\n";
const char USPERF_DOT_TAIL[] = "}\n";
const char USPERF_PRINT_MALLOC_FAILED[] = "Memory allocation required for printing failed. What the...";

#define USPERF_DOT_MAX_PENWIDTH 20

/**
 * Print usperf statistics to specified stream.
 *
 * Text and dot (graphviz) formats are supported. If point_name_fnc function
 * is supplied, it will be used to obtain the names of perfpoints.
 *
 * @param[in] usperf usperf structure to print
 * @param[out] stream stream to print output to
 * @param[in] format print format, one of:
 * @li USPERF_PRINT_DEFAULT - text format
 * @li USPERF_PRINT_DOT - dot (graphviz) format (run through dot to obtain a graph)
 * @param point_name_fnc custom function that will return the name of perfpoint. Leave NULL to use perfpoint numbers.
 */
void
usperf_print(struct usperf_s * usperf, FILE *stream, int format, const char * (*point_name_fnc)(int))
{
	struct perfpoint_edge_s *edge;
	uint64_t uavg, savg;
	uint64_t avg_max = 0, avg_min = INT64_MAX;
	uint32_t cnt_max = 0;
	uint32_t peniswidth, color;

	if( format == USPERF_PRINT_DOT ) {
		fprintf(stream, USPERF_DOT_HEAD);

		// We need maxima to print something meaningful.
		edge = usperf->points;
		for( int i = 0; i < PERFPOINTS_MAX; i++) {
			for( int j = 0; j < PERFPOINTS_MAX; j++) {
				if( edge->user_count > 0 ) {
					uavg = edge->user_sum / edge->user_count;

					if( uavg > avg_max) 
						avg_max = uavg;

					if( uavg < avg_min) 
						avg_min = uavg;

					if( edge->user_count > cnt_max) 
						cnt_max = edge->user_count;
				}

				edge++;
			}
		}

		// with small cnt_max, line'd be ridiculously large for small counts
		if( cnt_max < USPERF_DOT_MAX_PENWIDTH )
			cnt_max = USPERF_DOT_MAX_PENWIDTH;

		avg_max += 1;
	}

	edge = usperf->points;

	for( int i = 0; i < PERFPOINTS_MAX; i++) {
		for( int j = 0; j < PERFPOINTS_MAX; j++) {

			if( edge->user_count > 0 ) {

				uavg = edge->user_sum / edge->user_count;

				if( edge->system_count > 0 )
					savg = edge->system_sum / edge->system_count;
				else
					savg = 0;

				if( format == USPERF_PRINT_DOT ) {
					// pen width depends on number of transitions
					peniswidth = (int)((float)(edge->user_count) / (float)(cnt_max) * USPERF_DOT_MAX_PENWIDTH) + 1;
					color = (int)(((float)(uavg - avg_min) / (float)(avg_max - avg_min)) * 254) + 1;
					if (point_name_fnc == NULL) {
						fprintf(stream, "	\"%d\" -> \"%d\"", j, i);
					}
					else {
						fprintf(stream, "	\"%s\" -> \"%s\"", point_name_fnc(j), point_name_fnc(i));
					}

					fprintf(stream, "[ label = \"%ld (x%d = %ld)\", penwidth = %d, color=\"#%02X0000\" ];\n",
							uavg, edge->user_count, edge->user_sum, peniswidth, color
							);
				}
				else {
					if (point_name_fnc == NULL) {
						fprintf(stream, "%3d => %3d: ", j, i); 
					}
					else {
						fprintf(stream, "%10s => %-10s: ", point_name_fnc(j), point_name_fnc(i)); 
					}
					fprintf(stream, "%8dx usr: %8ld avg, %12ld sum | %6dx sys: %10ld avg\n",
							edge->user_count, uavg, edge->user_sum, edge->system_count, savg);
				}
			}

			edge++;
		}
	}

	if( format == USPERF_PRINT_DOT ) {
		fprintf(stream, USPERF_DOT_TAIL);
	}
}

#define USPERF_MAX_DELTA 1000
/**
 * Insert perfpoint.
 *
 * Each time this function is called, timer is read and elapsed time since last
 * perfpoint is written into the supplied usperf structure. Each perfpoint is
 * identified by an integer ranging from 1 to perfpoints_max (specified in
 * usperf_init). Index 0 is reserved for program entry (counter
 * initialization). Also this is C, so if you supply too big index, expect
 * usual SIGSEGV-and-friends fun.
 *
 * @param[in,out] usperf usperf structure to write info to.
 * @param[in] index perfpoint index <1; perfpoints_max>
 */
void
perfpoint(struct usperf_s * usperf, int index)
{
	struct perfpoint_edge_s *edge;
   
	edge = usperf->points + (index * PERFPOINTS_MAX) + usperf->last_point;

	uint64_t cnt = get_count(usperf);
	uint32_t ucount = edge->user_count;
	uint64_t usum = edge->user_sum;

	if ( ucount > 5 && cnt > (usum / ucount + USPERF_MAX_DELTA) ) {
		edge->system_count += 1;
		edge->system_sum += cnt;
	}
	else {
		edge->user_count += 1;
		edge->user_sum += cnt;
	}

	usperf->last_point = index;
}

/**
 * Clean up usperf structure.
 * @param[in,out] usperf usperf structure to close
 */
void
usperf_close(struct usperf_s * usperf)
{
	free(usperf->points);
	pcounter_close(&(usperf->cnt));
}
