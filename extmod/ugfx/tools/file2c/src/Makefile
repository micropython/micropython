TARGET = file2c
SRCS = $(shell find -name '*.c')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

CFLAGS = -Wall -p

CC = /usr/bin/gcc
RM = /bin/rm -f
 
all: clean
		$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

clean:
		$(RM) $(TARGET) $(OBJS)

