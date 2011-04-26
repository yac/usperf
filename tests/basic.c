/** @file
 * Basic test and example usage of usperf.
 */
#include <stdio.h>

/*
 * You probably want to use usperf.h from include dir:
 * #include <usperf.h>
 */
#include "../usperf.h"


/**
 * Uninteresting print-to-file helper, move along
 */
int save_usperf_output(const char *path, struct usperf_s * usperf, int format, const char * (*point_name_fnc)(int)) {
	FILE *log = fopen(path, "w");

	if (log != NULL) {
		USPERF_PRINT(usperf, log, format, point_name_fnc);

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
			return "first";
		case 2:
			return "second";
		case 3:
			return "third";
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

	/**
	 * struct usperf_s is used to store usperf info and is passed to USPERF_*
	 * macros. You can also use USPERF_*_S() macros if you are not to use more 
	 * than one structure (==no threads).
	 */
	struct usperf_s usperf;
	
	/*
	 * Initialization.
	 *
	 * `grep PERF_COUNT_HW /usr/include/linux/perf_event.h` for possible
	 * values of counter_type. The two most popular ones:
	 * @li PERF_COUNT_HW_CPU_CYCLES
	 * @li PERF_COUNT_HW_INSTRUCTION
	 */
	int c = USPERF_INIT(&usperf, 10, PERF_COUNT_HW_INSTRUCTIONS);

	if( c != 0 ) {
		printf("usperf init failed: %d\n", c);
		return c;
	}

	/*
	 * Perfpoint with index 1. 0 is reserved for initial entry (USPERF_INIT).
	 */
	PERFPOINT(&usperf, 1);

	for( i=0; i < 500000; i++ ) {
		PERFPOINT(&usperf, 2);
	}

	PERFPOINT(&usperf, 3);
	PERFPOINT(&usperf, 10);


	/*
	 * USPERF_PRINT will print usperf statistics to given stream. See its
	 * description for more info about formats.
	 */
	USPERF_PRINT(&usperf, stdout, USPERF_PRINT_DEFAULT, my_point_name);

	/*
	 * if USPERF symbol is not defined, USPERF_* macros are disabled. This may
	 * lead to "unused ..." warning - nothing is ever perfect (in C).
	 */
#ifdef USPERF
	save_usperf_output("test.usperf.dot", &usperf, USPERF_PRINT_DOT, my_point_name);
#else
	printf("usperf is turned off (no -DUSPERF passed to compiler?)\n");
#endif

	USPERF_CLOSE(&usperf);

	return 0;
}
