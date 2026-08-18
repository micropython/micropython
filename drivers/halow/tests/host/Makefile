# Host tests for the HaLow allocator. Compiles the REAL drivers/halow/halow_osal.c
# against stubs, so the tests cannot drift from the code they cover.
#
#   make          build + run
#   make asan     build + run under ASan/UBSan (catches what the checks miss)
CC      ?= gcc
HALOW   := ../..
SDK     := ../../../../lib/mm-iot-sdk/framework/morselib/include
CFLAGS  := -m32 -std=c11 -Wall -Wextra -Werror -g -O1 \
           -Istub -I$(HALOW) -I$(SDK) -Wno-unused-parameter

all: run

test_alloc: test_alloc.c $(HALOW)/halow_osal.c
	$(CC) $(CFLAGS) -o $@ test_alloc.c

run: test_alloc
	./test_alloc

asan: test_alloc.c $(HALOW)/halow_osal.c
	$(CC) $(CFLAGS) -fsanitize=address,undefined -fno-omit-frame-pointer \
	    -o test_alloc_asan test_alloc.c
	./test_alloc_asan

clean:
	rm -f test_alloc test_alloc_asan

.PHONY: all run asan clean
