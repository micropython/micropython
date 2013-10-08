CC = gcc
CFLAGS = -Wall -ansi -std=gnu99 -Os -DEMIT_ENABLE_CPY -DEMIT_ENABLE_THUMB #-DNDEBUG
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
	asmthumb.c \
	emitinlinethumb.c \
	runtime.c \
	vm.c \
	main.c \

SRC_ASM = \

OBJ = $(SRC:.c=.o) $(SRC_ASM:.s=.o) emitnx64.o emitnthumb.o
LIB =
PROG = py

$(PROG): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LIB) $(LDFLAGS)

runtime.o: runtime.c
	$(CC) $(CFLAGS) -O3 -c -o $@ $<

vm.o: vm.c
	$(CC) $(CFLAGS) -O3 -c -o $@ $<

parse.o: grammar.h
compile.o: grammar.h
emitcpy.o: emit.h
emitbc.o: emit.h

emitnx64.o: emitnative.c emit.h
	$(CC) $(CFLAGS) -DN_X64 -c -o $@ $<

emitnthumb.o: emitnative.c emit.h
	$(CC) $(CFLAGS) -DN_THUMB -c -o $@ $<

clean:
	/bin/rm $(OBJ)
