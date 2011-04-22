CC = gcc
CFLAGS = -DDEBUG -DUSPERF -Wall -g -Wextra -pedantic --std=gnu99

NAME = usperf
VER = 0.4
LIB = lib$(NAME).so
PACKDIR = lib$(NAME)-$(VER)
ARCHIVE = $(PACKDIR).tgz

TEST_DIR = tests

PACK_FILES = usperf.c usperf.h perf.c Makefile README


all: $(LIB)

perf.o: perf.c
	$(CC) -fpic -finline-functions -finline-functions-called-once $(CFLAGS) -c perf.c

usperf.o: usperf.c
	$(CC) -fpic -O2 $(CFLAGS) -c usperf.c

$(LIB): usperf.o perf.o
	$(CC) -shared -Wl,-soname,$(LIB) $(CFLAGS) usperf.o perf.o -o $@

tests: 
	$(MAKE) -C $(TEST_DIR)
	
doc:
	doxygen

run: install test-lib
	./test-lib

# attempts to install into library path from ldconfig, usually /usr/lib{,64}
install: $(LIB)
	install $(LIB) `/sbin/ldconfig -p | sed -nr '5p' | sed -re 's#^.* => (\/.+\/)[^/]+#\1#'`

clean:
	rm -f $(LIB) *.o test-lib test-nolib test-single test-rdpmc test-mini
	$(MAKE) -C $(TEST_DIR) clean

dist:
	rm -rf $(PACKDIR)
	mkdir $(PACKDIR)
	ln $(PACK_FILES) $(PACKDIR)
	tar -czf $(ARCHIVE) $(PACKDIR)
	rm -rf $(PACKDIR)


.PHONY: doc tests
