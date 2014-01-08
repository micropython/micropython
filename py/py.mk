# default settings; can be overriden in main Makefile

ifndef PY_SRC
PY_SRC = ../py
endif

ifndef BUILD
BUILD = build
endif

# to create the build directory

$(BUILD):
	mkdir -p $@

# where py object files go (they have a name prefix to prevent filename clashes)

PY_BUILD = $(BUILD)/py.

# py object files

PY_O_BASENAME = \
	nlrx86.o \
	nlrx64.o \
	nlrthumb.o \
	malloc.o \
	gc.o \
	qstr.o \
	vstr.o \
	unicode.o \
	lexer.o \
	lexerstr.o \
	lexerunix.o \
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
	objbool.o \
	objboundmeth.o \
	objcell.o \
	objclass.o \
	objclosure.o \
	objcomplex.o \
	objdict.o \
	objexcept.o \
	objfloat.o \
	objfun.o \
	objgenerator.o \
	objinstance.o \
	objint.o \
	objlist.o \
	objmodule.o \
	objnone.o \
	objrange.o \
	objset.o \
	objslice.o \
	objstr.o \
	objtuple.o \
	objtype.o \
	stream.o \
	builtin.o \
	builtinimport.o \
	vm.o \
	showbc.o \
	repl.o \

# prepend the build destination prefix to the py object files

PY_O = $(addprefix $(PY_BUILD), $(PY_O_BASENAME))

$(PY_BUILD)emitnx64.o: $(PY_SRC)/emitnative.c $(PY_SRC)/emit.h mpconfigport.h
	$(CC) $(CFLAGS) -DN_X64 -c -o $@ $<

$(PY_BUILD)emitnthumb.o: $(PY_SRC)/emitnative.c $(PY_SRC)/emit.h mpconfigport.h
	$(CC) $(CFLAGS) -DN_THUMB -c -o $@ $<

$(PY_BUILD)%.o: $(PY_SRC)/%.S
	$(CC) $(CFLAGS) -c -o $@ $<

$(PY_BUILD)%.o: $(PY_SRC)/%.c mpconfigport.h
	$(CC) $(CFLAGS) -c -o $@ $<

# optimising gc for speed; 5ms down to 4ms on pybv2
$(PY_BUILD)gc.o: $(PY_SRC)/gc.c
	$(CC) $(CFLAGS) -O3 -c -o $@ $<

# optimising vm for speed, adds only a small amount to code size but makes a huge difference to speed (20% faster)
$(PY_BUILD)vm.o: $(PY_SRC)/vm.c
	$(CC) $(CFLAGS) -O3 -c -o $@ $<

# header dependencies

$(PY_BUILD)parse.o: $(PY_SRC)/grammar.h
$(PY_BUILD)compile.o: $(PY_SRC)/grammar.h
$(PY_BUILD)/emitcpy.o: $(PY_SRC)/emit.h
$(PY_BUILD)emitbc.o: $(PY_SRC)/emit.h
