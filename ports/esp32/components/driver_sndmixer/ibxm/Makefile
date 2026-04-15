
#You can use this to compile xm2wav to test the ibxm library on
#the host, to inspect correct working and view memory use.

CC=gcc
CFLAGS=-pedantic -Wall -std=gnu99 -g

all: xm2wav

clean:
	rm -f xm2wav

xm2wav: xm2wav.c ibxm.c ibxm.h
	$(CC) $(CFLAGS) xm2wav.c ibxm.c -o xm2wav
