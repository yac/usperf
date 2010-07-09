CC = gcc
CFLAGS = -DDEBUG -DUPERF -Wall -g -Wextra -pedantic --std=gnu99
LIB = libuperf.so

VER = 0.1

SRCS = decode.c pktdec.c packets.c pktmatch.c

HDRS = common.h debug.h packets.h decode.h match_rates.h


all: $(LIB) test-mini

perf.o: perf.c
	$(CC) -fpic -finline-functions -finline-functions-called-once $(CFLAGS) -c perf.c
	#$(CC) -fpic $(CFLAGS) -c perf.c

uperf.o: uperf.c
	$(CC) -fpic -O2 $(CFLAGS) -c uperf.c
	#$(CC) -fpic $(CFLAGS) -c uperf.c

$(LIB): uperf.o perf.o
	$(CC) -shared -Wl,-soname,$(LIB) $(CFLAGS) uperf.o perf.o -o $@

test-lib: $(LIB) test.c
	$(CC) -luperf $(CFLAGS) test.c -o $@

test-nolib: test.c perf.o uperf.o
	$(CC) $(CFLAGS) perf.o uperf.o test.c -o $@

test-single: test-single.c
	$(CC) -luperf $(CFLAGS) test-single.c -o $@

test-mini: test-mini.c perf.o
	$(CC) $(CFLAGS) perf.o test-mini.c -o $@

test-rdpmc: test-rdpmc.c
	$(CC) $(CFLAGS) test-rdpmc.c -o $@

tests: test-lib test-rdpmc test-single test-mini test-nolib
	
run: install test-lib
	./test-lib

install: $(LIB)
	install $(LIB) /usr/lib64/

clean:
	rm -f $(LIB) *.o test-lib test-nolib test-single test-rdpmc test-mini

