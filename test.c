#include <stdio.h>
#include <linux/perf_event.h>
#include "uperf.h"

int save_uperf_log(struct uperf_s *uperf, const char *path) {
	FILE * log = fopen(path, "w");
	if (log == NULL)
		return 0;

	UPERF_PRINT(uperf, log, UPERF_PRINT_DEFAULT, NULL);

	fclose(log);

	return 1;
}

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
/*const char points[128][255];*/

int
/*main(int argc, char *argv[])*/
main()
{
	struct uperf_s uperf;

	// grep /usr/include/linux/perf_event.h for PERF_COUNT_HW for alternatives
	/*int c = uperf_init(&uperf, PERF_COUNT_HW_CPU_CYCLES);*/
	int c = UPERF_INIT(&uperf, 128, PERF_COUNT_HW_INSTRUCTIONS);

	if( c != 0 ) {
		printf("uperf init failed: %d\n", c);
		return c;
	}

	PERFPOINT(&uperf, 1);

	for( int i=0; i < 5; i++ ) {
		PERFPOINT(&uperf, 2);
	}

	PERFPOINT(&uperf, 3);
	PERFPOINT(&uperf, 10);

	/*save_uperf_log(&uperf, "test.log");*/
	UPERF_PRINT(&uperf, stdout, UPERF_PRINT_DEFAULT, my_point_name);

#ifdef UPERF
	FILE *log = fopen("uperf.dot", "w");

	if (log != NULL) {
		UPERF_PRINT(&uperf, log, UPERF_PRINT_DOT, my_point_name);

		fclose(log);
	}
	else {
		printf("Opening .dot file failed!");
	}
#endif

	UPERF_CLOSE(&uperf);

	return 0;
}
