PYSRC=../py
BUILD=build

CC = gcc
CFLAGS = -I. -I$(PYSRC) -Wall -ansi -std=gnu99 -Os #-DNDEBUG
CFLAGS_PY = -DEMIT_ENABLE_CPY -DEMIT_ENABLE_THUMB
LDFLAGS =

SRC_C = \
	main.c \

PY_O = \
	malloc.o \
	qstr.o \
	misc.o \
	lexer.o \
	lexerfile.o \
	parse.o \
	scope.o \
	compile.o \
	emitcommon.o \
	emitpass1.o \
	emitcpy.o \
	emitbc.o \
	asmx64.o \
	emitnx64.o \
	asmthumb.o \
	emitnthumb.o \
	emitinlinethumb.o \
	runtime.o \
	vm.o \

OBJ = $(addprefix $(BUILD)/, $(SRC_C:.c=.o) $(PY_O))
LIB =
PROG = py

$(PROG): $(BUILD) $(OBJ)
	$(CC) -o $@ $(OBJ) $(LIB) $(LDFLAGS)

$(BUILD):
	mkdir $@

$(BUILD)/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/%.o: $(PYSRC)/%.c mpyconfig.h
	$(CC) $(CFLAGS) $(CFLAGS_PY) -c -o $@ $<

$(BUILD)/emitnx64.o: $(PYSRC)/emitnative.c $(PYSRC)/emit.h
	$(CC) $(CFLAGS) $(CFLAGS_PY) -DN_X64 -c -o $@ $<

$(BUILD)/emitnthumb.o: $(PYSRC)/emitnative.c $(PYSRC)/emit.h
	$(CC) $(CFLAGS) $(CFLAGS_PY) -DN_THUMB -c -o $@ $<

# optimising vm for speed, adds only a small amount to code size but makes a huge difference to speed (20% faster)
$(BUILD)/vm.o: $(PYSRC)/vm.c
	$(CC) $(CFLAGS) $(CFLAGS_PY) -O3 -c -o $@ $<

$(BUILD)/parse.o: $(PYSRC)/grammar.h
$(BUILD)/compile.o: $(PYSRC)/grammar.h
$(BUILD)/emitcpy.o: $(PYSRC)/emit.h
$(BUILD)/emitbc.o: $(PYSRC)/emit.h

clean:
	/bin/rm -r $(BUILD)
