/** @file
 * Basic test and example usage of uperf.
 */
#include <stdio.h>

/*
 * You probably want to use uperf.h from include dir:
 * #include <uperf.h>
 */
#include "../uperf.h"


/**
 * Uninteresting print-to-file helper, move along
 */
int save_uperf_output(const char *path, struct uperf_s * uperf, int format, const char * (*point_name_fnc)(int)) {
	FILE *log = fopen(path, "w");

	if (log != NULL) {
		UPERF_PRINT(uperf, log, format, point_name_fnc);

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
	int i;

	/**
	 * struct uperf_s is used to store uperf info and is passed to UPERF_*
	 * macros. You can also use UPERF_*_S() macros if you are not to use more 
	 * than one structure (==no threads).
	 */
	struct uperf_s uperf;
	
	/*
	 * Initialization.
	 *
	 * `grep PERF_COUNT_HW /usr/include/linux/perf_event.h` for possible
	 * values of counter_type. The two most popular ones:
	 * @li PERF_COUNT_HW_CPU_CYCLES
	 * @li PERF_COUNT_HW_INSTRUCTION
	 */
	int c = UPERF_INIT(&uperf, 128, PERF_COUNT_HW_INSTRUCTIONS);

	if( c != 0 ) {
		printf("uperf init failed: %d\n", c);
		return c;
	}

	/*
	 * Perfpoint with index 1. 0 is reserved for initial entry (UPERF_INIT).
	 */
	PERFPOINT(&uperf, 1);

	for( i=0; i < 500000; i++ ) {
		PERFPOINT(&uperf, 2);
	}

	PERFPOINT(&uperf, 3);
	PERFPOINT(&uperf, 10);


	/*
	 * UPERF_PRINT will print uperf statistics to given stream. See its
	 * description for more info about formats.
	 */
	UPERF_PRINT(&uperf, stdout, UPERF_PRINT_DEFAULT, my_point_name);

	/*
	 * if UPERF symbol is not defined, UPERF_* macros are disabled. This may
	 * lead to "unused ..." warning - nothing is ever perfect (in C).
	 */
#ifdef UPERF
	save_uperf_output("test.uperf.dot", &uperf, UPERF_PRINT_DOT, my_point_name);
#else
	printf("uPerf is turned off (no -DUPERF passed to compiler?)\n");
#endif

	UPERF_CLOSE(&uperf);

	return 0;
}
