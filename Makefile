CC = gcc
CFLAGS = -DDEBUG -Wall -g -Wextra -pedantic --std=gnu99
LIB = libuperf.so

VER = 0.1

SRCS = decode.c pktdec.c packets.c pktmatch.c

HDRS = common.h debug.h packets.h decode.h match_rates.h


all: $(LIB)

perf.o: perf.c
	$(CC) -fpic -finline-functions -finline-functions-called-once $(CFLAGS) -c perf.c

uperf.o: uperf.c
	$(CC) -fpic -O2 $(CFLAGS) -c uperf.c

$(LIB): uperf.o perf.o
	$(CC) -shared -Wl,-soname,$(LIB) $(CFLAGS) uperf.o perf.o -o $(LIB)

test: test.c
	$(CC) -luperf $(CFLAGS) test.c -o test

test-rdpmc: test-rdpmc.c
	$(CC) $(CFLAGS) test-rdpmc.c -o test-rdpmc

tests: test test-rdpmc
	
run: install test
	./test

install: $(LIB)
	install $(LIB) /usr/lib64/

clean:
	rm -f $(LIB) *.o test test-rdpmc

