/** @file
 * Basic test and example usage of uperf.
 *
 * The *_S macro versions.
 */
#include <stdio.h>
#include "uperf.h"

/**
 * Uninteresting print-to-file helper, move along
 */
int save_uperf_output(const char *path, int format, const char * (*point_name_fnc)(int)) {
	FILE *log = fopen(path, "w");

	if (log != NULL) {
		UPERF_PRINT_S(log, format, point_name_fnc);

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
 * Pass function with this signature (int -> const char*) to UPERF_PRINT() to
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
 * Basic uperf usage.
 */
int main()
{
	/*
	 * Initialization.
	 */
	int c = UPERF_INIT_S(128, PERF_COUNT_HW_INSTRUCTIONS);

	if( c != 0 ) {
		printf("uperf init failed: %d\n", c);
		return c;
	}

	/*
	 * Perfpoint with index 1. 0 is reserved for initial entry (UPERF_INIT).
	 */
	PERFPOINT_S(1);

	for( int i=0; i < 5; i++ ) {
		PERFPOINT_S(2);
	}

	PERFPOINT_S(3);
	PERFPOINT_S(10);


	/*
	 * Print statistics.
	 */
	UPERF_PRINT_S(stdout, UPERF_PRINT_DEFAULT, my_point_name);

	/*
	 * if UPERF symbol is not defined, UPERF_* macros are disabled. This may
	 * lead to "unused ..." warning - nothing is ever perfect (in C).
	 */
#ifdef UPERF
	save_uperf_output("test.uperf.dot", UPERF_PRINT_DOT, my_point_name);
#endif

	UPERF_CLOSE_S;

	return 0;
}
