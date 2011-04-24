CC = gcc
CFLAGS = -DDEBUG -DUSPERF -g -Wall -Wextra -pedantic --std=gnu99

NAME = usperf
TESTS = $(shell cat tests | tr '\n' ' ')

# link to local library if needed
#export LIBRARY_PATH = ../

all: tests

basic-lib: $(LIB) basic.c
	$(CC) $(CFLAGS) -l$(NAME) basic.c -o $@

basic-nolib: basic.c ../perf.o ../$(NAME).o
	$(CC) $(CFLAGS) ../perf.o ../$(NAME).o basic.c -o $@

mini: mini.c ../perf.o
	$(CC) $(CFLAGS) ../perf.o mini.c -o $@

rdpmc: rdpmc.c
	$(CC) $(CFLAGS) rdpmc.c -o $@

swevent: swevent.c ../perf.o ../$(NAME).o
	$(CC) $(CFLAGS) ../perf.o ../$(NAME).o swevent.c -o $@

tests: $(TESTS)
	
clean:
	rm -f *.o *.dot $(TESTS)
