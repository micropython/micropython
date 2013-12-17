PYSRC=../py
BUILD=build

CC = gcc
CFLAGS = -I. -I$(PYSRC) -Wall -Werror -ansi -std=gnu99 -Os #-DNDEBUG
LDFLAGS = -lm

SRC_C = \
	main.c \
	lexerunix.c \

PY_O = \
	nlrx64.o \
	malloc.o \
	qstr.o \
	vstr.o \
	misc.o \
	lexer.o \
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
	map.o \
	obj.o \
	builtin.o \
	vm.o \
	showbc.o \
	repl.o \

OBJ = $(addprefix $(BUILD)/, $(SRC_C:.c=.o) $(PY_O))
LIB = -lreadline
PROG = py

$(PROG): $(BUILD) $(OBJ)
	$(CC) -o $@ $(OBJ) $(LIB) $(LDFLAGS)

$(BUILD):
	mkdir $@

$(BUILD)/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/%.o: $(PYSRC)/%.s
	$(AS) -c -o $@ $<

$(BUILD)/%.o: $(PYSRC)/%.c mpyconfig.h
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/emitnx64.o: $(PYSRC)/emitnative.c $(PYSRC)/emit.h
	$(CC) $(CFLAGS) -DN_X64 -c -o $@ $<

$(BUILD)/emitnthumb.o: $(PYSRC)/emitnative.c $(PYSRC)/emit.h
	$(CC) $(CFLAGS) -DN_THUMB -c -o $@ $<

# optimising vm for speed, adds only a small amount to code size but makes a huge difference to speed (20% faster)
$(BUILD)/vm.o: $(PYSRC)/vm.c
	$(CC) $(CFLAGS) -O3 -c -o $@ $<

$(BUILD)/main.o: mpyconfig.h
$(BUILD)/parse.o: $(PYSRC)/grammar.h
$(BUILD)/compile.o: $(PYSRC)/grammar.h
$(BUILD)/emitcpy.o: $(PYSRC)/emit.h
$(BUILD)/emitbc.o: $(PYSRC)/emit.h

clean:
	/bin/rm -r $(BUILD)
