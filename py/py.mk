##########
# The following should eventually go into a more central location
# when a reorg is done.
#
# Turn on increased build verbosity by defining BUILD_VERBOSE in your main
# Makefile or in your environment. You can also use V=1 on the make command
# line.
ifeq ("$(origin V)", "command line")
BUILD_VERBOSE=$(V)
endif
ifndef BUILD_VERBOSE
BUILD_VERBOSE = 0
endif
ifeq ($(BUILD_VERBOSE),0)
Q = @
else
Q =
endif
# Since this is a new feature, advertise it
ifeq ($(BUILD_VERBOSE),0)
$(info Use make V=1 or set BUILD_VERBOSE in your environment to increase build verbosity.)
endif
#
#########

# default settings; can be overriden in main Makefile

ifndef PY_SRC
PY_SRC = ../py
endif

ifndef BUILD
BUILD = build
endif

# to create the build directory

$(BUILD):
	$(Q)mkdir -p $@

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
	strtonum.o \
	obj.o \
	objarray.o \
	objbool.o \
	objboundmeth.o \
	objcell.o \
	objclosure.o \
	objcomplex.o \
	objdict.o \
	objenumerate.o \
	objexcept.o \
	objfilter.o \
	objfloat.o \
	objfun.o \
	objgenerator.o \
	objint.o \
	objint_longlong.o \
	objlist.o \
	objmap.o \
	objmodule.o \
	objnone.o \
	objrange.o \
	objset.o \
	objslice.o \
	objstr.o \
	objtuple.o \
	objtype.o \
	sequence.o \
	stream.o \
	builtin.o \
	builtinimport.o \
	builtineval.o \
	builtinmp.o \
	vm.o \
	showbc.o \
	repl.o \
	objzip.o \

# prepend the build destination prefix to the py object files

PY_O = $(addprefix $(PY_BUILD), $(PY_O_BASENAME))

$(PY_BUILD)emitnx64.o: $(PY_SRC)/emitnative.c $(PY_SRC)/emit.h mpconfigport.h
	$(ECHO) "CC $<"
	$(Q)$(CC) $(CFLAGS) -DN_X64 -c -o $@ $<

$(PY_BUILD)emitnthumb.o: $(PY_SRC)/emitnative.c $(PY_SRC)/emit.h mpconfigport.h
	$(ECHO) "CC $<"
	$(Q)$(CC) $(CFLAGS) -DN_THUMB -c -o $@ $<

$(PY_BUILD)%.o: $(PY_SRC)/%.S
	$(ECHO) "CC $<"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

$(PY_BUILD)%.o: $(PY_SRC)/%.c mpconfigport.h
	$(ECHO) "CC $<"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

# optimising gc for speed; 5ms down to 4ms on pybv2
$(PY_BUILD)gc.o: $(PY_SRC)/gc.c
	$(ECHO) "CC $<"
	$(Q)$(CC) $(CFLAGS) -O3 -c -o $@ $<

# optimising vm for speed, adds only a small amount to code size but makes a huge difference to speed (20% faster)
$(PY_BUILD)vm.o: $(PY_SRC)/vm.c
	$(ECHO) "CC $<"
	$(Q)$(CC) $(CFLAGS) -O3 -c -o $@ $<

# header dependencies

$(PY_BUILD)parse.o: $(PY_SRC)/grammar.h
$(PY_BUILD)compile.o: $(PY_SRC)/grammar.h
$(PY_BUILD)/emitcpy.o: $(PY_SRC)/emit.h
$(PY_BUILD)emitbc.o: $(PY_SRC)/emit.h
