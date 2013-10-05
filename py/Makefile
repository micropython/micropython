CC = gcc
CFLAGS = -Wall -ansi -std=gnu99 -Os -DEMIT_ENABLE_CPY -DEMIT_ENABLE_X64 #-DNDEBUG
LDFLAGS =

SRC = \
	malloc.c \
	misc.c \
	qstr.c \
	lexer.c \
	lexerfile.c \
	parse.c \
	scope.c \
	compile.c \
	emitcommon.c \
	emitpass1.c \
	emitcpy.c \
	emitbc.c \
	asmx64.c \
	emitx64.c \
	emitthumb.c \
	asmthumb.c \
	runtime.c \
	vm.c \
	main.c \

SRC_ASM = \

OBJ = $(SRC:.c=.o) $(SRC_ASM:.s=.o)
LIB =
PROG = py

$(PROG): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LIB) $(LDFLAGS)

runtime.o: runtime.c
	$(CC) $(CFLAGS) -O3 -c -o $@ $<

bc.o: bc.c
	$(CC) $(CFLAGS) -O3 -c -o $@ $<

parse.o: grammar.h
compile.o: grammar.h
emitcpy.o: emit.h
emitbc.o: emit.h
emitx64.o: emit.h
emitthumb.o: emit.h

clean:
	/bin/rm $(OBJ)
