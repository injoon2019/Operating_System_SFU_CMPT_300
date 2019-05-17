CC_C = gcc
CCFLAGS= -Wall -g -std=c99 -Werror -D_POSIX_C_SOURCE=199309L -pthread -lrt

all: clean candykids

%.o : %.c
	$(CC) -c $(CCFLAGS) $<

candykids: candykids.o bbuff.o stats.o
	$(CC_C) $(CCFLAGS) candykids.o bbuff.o stats.o -o $@

clean:
	rm -f candykids
	rm -f candykids.o bbuff.o stats.o

