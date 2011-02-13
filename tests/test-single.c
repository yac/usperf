/** @file
 * Basic test and example usage of usperf.
 *
 * The *_S macro version.
 */
#include <stdio.h>

/*
 * You probably want to use usperf.h from include dir:
 * #include <usperf.h>
 */
#include "usperf.h"


/**
 * Uninteresting print-to-file helper, move along
 */
int save_usperf_output(const char *path, int format, const char * (*point_name_fnc)(int)) {
	FILE *log = fopen(path, "w");

	if (log != NULL) {
		USPERF_PRINT_S(log, format, point_name_fnc);

		fclose(log);
	}
	else {
		printf("Opening .dot file failed!");
		return 0;
	}

	return 1;
}

/**
 * Example function for named perfpoints.
 *
 * Pass function with this signature (int -> const char*) to USPERF_PRINT() to
 * print perfpoint names instead of just numbers.
 */
const char * my_point_name(int point) {
	static char num[8];
	switch (point) {
		case 0:
			return "START";
		case 1:
			return "prvni";
		case 2:
			return "druhy";
		case 3:
			return "treti";
		default:
			sprintf(num, "[%d]", point);
			return num;
	}
}

/**
 * Basic usperf usage.
 */
int main()
{
	int i;
	/*
	 * Initialization.
	 */
	int c = USPERF_INIT_S(128, PERF_COUNT_HW_INSTRUCTIONS);

	if( c != 0 ) {
		printf("usperf init failed: %d\n", c);
		return c;
	}

	/*
	 * Perfpoint with index 1. 0 is reserved for initial entry (USPERF_INIT).
	 */
	PERFPOINT_S(1);

	for( i=0; i < 5; i++ ) {
		PERFPOINT_S(2);
	}

	PERFPOINT_S(3);
	PERFPOINT_S(10);


	/*
	 * Print statistics.
	 */
	USPERF_PRINT_S(stdout, USPERF_PRINT_DEFAULT, my_point_name);

	/*
	 * if USPERF symbol is not defined, USPERF_* macros are disabled. This may
	 * lead to "unused ..." warning - nothing is ever perfect (in C).
	 */
#ifdef USPERF
	save_usperf_output("test.usperf.dot", USPERF_PRINT_DOT, my_point_name);
#else
	printf("usperf is turned off (no -DUSPERF passed to compiler?)\n");
#endif

	USPERF_CLOSE_S;

	return 0;
}
