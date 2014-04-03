# where py object files go (they have a name prefix to prevent filename clashes)
PY_BUILD = $(BUILD)/py

# file containing qstr defs for the core Python bit

PY_QSTR_DEFS = $(PY_SRC)/qstrdefs.h

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
	mpz.o \
	lexer.o \
	lexerstr.o \
	lexerunix.o \
	parse.o \
	parsehelper.o \
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
	formatfloat.o \
	parsenumbase.o \
	parsenum.o \
	emitglue.o \
	runtime.o \
	map.o \
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
	objgetitemiter.o \
	objint.o \
	objint_longlong.o \
	objint_mpz.o \
	objlist.o \
	objmap.o \
	objmodule.o \
	objobject.o \
	objnone.o \
	objnamedtuple.o \
	objrange.o \
	objset.o \
	objslice.o \
	objstr.o \
	objtuple.o \
	objtype.o \
	objzip.o \
	sequence.o \
	stream.o \
	binary.o \
	builtin.o \
	builtinimport.o \
	builtinevex.o \
	builtintables.o \
	modarray.o \
	modcollections.o \
	modio.o \
	modmath.o \
	modmicropython.o \
	vm.o \
	showbc.o \
	repl.o \
	intdivmod.o \
	pfenv.o \

# prepend the build destination prefix to the py object files
PY_O = $(addprefix $(PY_BUILD)/, $(PY_O_BASENAME))

# qstr data

# Adding an order only dependency on $(PY_BUILD) causes $(PY_BUILD) to get
# created before we run the script to generate the .h
$(PY_BUILD)/qstrdefs.generated.h: | $(PY_BUILD)/
$(PY_BUILD)/qstrdefs.generated.h: $(PY_QSTR_DEFS) $(QSTR_DEFS) $(PY_SRC)/makeqstrdata.py
	$(ECHO) "makeqstrdata $(PY_QSTR_DEFS) $(QSTR_DEFS)"
	$(Q)python $(PY_SRC)/makeqstrdata.py $(PY_QSTR_DEFS) $(QSTR_DEFS) > $@

# We don't know which source files actually need the generated.h (since
# it is #included from str.h). The compiler generated dependencies will cause
# the right .o's to get recompiled if the generated.h file changes. Adding
# an order-only dependendency to all of the .o's will cause the generated .h
# to get built before we try to compile any of them.
$(PY_O): | $(PY_BUILD)/qstrdefs.generated.h

# emitters

$(PY_BUILD)/emitnx64.o: CFLAGS += -DN_X64
$(PY_BUILD)/emitnx64.o: py/emitnative.c
	$(call compile_c)

$(PY_BUILD)/emitnthumb.o: CFLAGS += -DN_THUMB
$(PY_BUILD)/emitnthumb.o: py/emitnative.c
	$(call compile_c)

# optimising gc for speed; 5ms down to 4ms on pybv2
$(PY_BUILD)/gc.o: CFLAGS += -O3

# optimising vm for speed, adds only a small amount to code size but makes a huge difference to speed (20% faster)
$(PY_BUILD)/vm.o: CFLAGS += -O3

