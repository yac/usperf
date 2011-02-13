CC = gcc
CFLAGS = -DDEBUG -DUPERF -Wall -g -Wextra -pedantic --std=gnu99

NAME = uperf
VER = 0.3
LIB = lib$(NAME).so
PACKDIR = lib$(NAME)-$(VER)
ARCHIVE = $(PACKDIR).tgz

TEST_DIR = tests

PACK_FILES = uperf.c uperf.h perf.c Makefile README


all: $(LIB)

perf.o: perf.c
	$(CC) -fpic -finline-functions -finline-functions-called-once $(CFLAGS) -c perf.c

uperf.o: uperf.c
	$(CC) -fpic -O2 $(CFLAGS) -c uperf.c

$(LIB): uperf.o perf.o
	$(CC) -shared -Wl,-soname,$(LIB) $(CFLAGS) uperf.o perf.o -o $@

test-lib: $(LIB) $(TEST_DIR)/test.c
	$(CC) -luperf $(CFLAGS) $(TEST_DIR)/test.c -o $@

test-nolib: $(TEST_DIR)/test.c perf.o uperf.o
	$(CC) $(CFLAGS) perf.o uperf.o $(TEST_DIR)/test.c -o $@

test-single: $(TEST_DIR)/test-single.c
	$(CC) -luperf $(CFLAGS) $(TEST_DIR)/test-single.c -o $@

test-mini: $(TEST_DIR)/test-mini.c perf.o
	$(CC) $(CFLAGS) perf.o $(TEST_DIR)/test-mini.c -o $@

test-rdpmc: $(TEST_DIR)/test-rdpmc.c
	$(CC) $(CFLAGS) $(TEST_DIR)/test-rdpmc.c -o $@

tests: test-lib test-rdpmc test-single test-mini test-nolib
	
doc:
	doxygen

run: install test-lib
	./test-lib

install: $(LIB)
	install $(LIB) /usr/lib64/

clean:
	rm -f $(LIB) *.o test-lib test-nolib test-single test-rdpmc test-mini

dist:
	rm -rf $(PACKDIR)
	mkdir $(PACKDIR)
	ln $(PACK_FILES) $(PACKDIR)
	tar -czf $(ARCHIVE) $(PACKDIR)
	rm -rf $(PACKDIR)


.PHONY: doc
