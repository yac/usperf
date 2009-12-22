CC = gcc
CFLAGS = -DDEBUG -Wall -Wextra -pedantic --std=gnu99
NAME = uperf

VER = 0.1

SRCS = decode.c pktdec.c packets.c pktmatch.c

HDRS = common.h debug.h packets.h decode.h match_rates.h


all: $(NAME)

$(NAME): main.c uperf.c uperf.h
	$(CC) $(CFLAGS) main.c uperf.c -o $(NAME)

clean:
	rm -f $(LIB)

