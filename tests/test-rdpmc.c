#include <stdio.h>
#include <stdint.h>


inline uint64_t rdpmc(uint32_t reg)
{
	uint64_t val;

	asm("rdpmc" : "=A" (val) : "c" (reg));

	return val;
}

int
/*main(int argc, char *argv[])*/
main()
{
	uint32_t counter = 0;
	uint64_t c;

	for (int i = 0; i < 5; i++) {
		c = rdpmc(counter);
		printf("%ld\n", c);
	}

	return 0;
}
