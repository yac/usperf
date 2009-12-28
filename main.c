#include <stdio.h>
#include "uperf.h"

int
/*main(int argc, char *argv[])*/
main()
{

	uperf_init();
	
	PERFPOINT(0);

	for( int i=0; i < 5000; i++ ) {
		PERFPOINT(1);
	}

	PERFPOINT(2);

	uperf_print();

	return 0;
}
