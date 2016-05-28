ECHO = @echo
RM = rm
MKDIR = mkdir
PYTHON = python
TRANSFORM = transform.py
AMALGAMATE = amalgamate.py

BUILD = build
HEADER_BUILD = $(BUILD)/genhdr

PY_SRC = ../py

TR_SRC_PY_H = $(addprefix $(BUILD)/py/,\
	mpconfig.h \
	misc.h \
	mpprint.h \
	nlr.h \
	qstr.h \
	obj.h \
	objlist.h \
	objtuple.h \
	objexcept.h \
	mpstate.h \
	mpz.h \
	runtime0.h \
	lexer.h \
	parse.h \
	compile.h \
	emitglue.h \
	runtime.h \
	builtin.h \
	repl.h \
	gc.h \
	stackctrl.h \
	stream.h \
	binary.h \
	objint.h \
	formatfloat.h \
	unicode.h \
	parsenum.h \
        smallint.h \
	scope.h \
	emit.h \
	bc0.h \
	bc.h \
	objstr.h \
	objmodule.h \
	objgenerator.h \
	frozenmod.h \
	objfun.h \
	objtype.h \
	parsenumbase.h \
	mperrno.h \
	mphal.h \
	ringbuf.h \
	)

TR_SRC_PY_C = $(addprefix $(BUILD)/py/,\
	malloc.c \
	qstr.c \
	mpprint.c \
	vstr.c \
	unicode.c \
	mpz.c \
	gc.c \
	nlrsetjmp.c \
	lexer.c \
	lexerstr.c \
	lexerunix.c \
	parse.c \
	scope.c \
	emitcommon.c \
	emitbc.c \
	compile.c \
	formatfloat.c \
	parsenumbase.c \
	parsenum.c \
	emitglue.c \
	runtime.c \
	nativeglue.c \
	stackctrl.c \
	argcheck.c \
	warning.c \
	map.c \
	opmethods.c \
	sequence.c \
	stream.c \
	binary.c \
	builtinimport.c \
	builtinevex.c \
	obj.c \
	objattrtuple.c \
	objarray.c \
	objbool.c \
	objboundmeth.c \
	objcell.c \
	objclosure.c \
	objcomplex.c \
	objdict.c \
	objenumerate.c \
	objexcept.c \
	objfilter.c \
	objfloat.c \
	objfun.c \
	objgenerator.c \
	objgetitemiter.c \
	objint.c \
	objint_longlong.c \
	objint_mpz.c \
	objlist.c \
	objmap.c \
	objmodule.c \
	objobject.c \
	objpolyiter.c \
	objproperty.c \
	objnone.c \
	objnamedtuple.c \
	objrange.c \
	objreversed.c \
	objset.c \
	objsingleton.c \
	objslice.c \
	objstr.c \
	objstrunicode.c \
	objstringio.c \
	objtuple.c \
	objtype.c \
	objzip.c \
	modarray.c \
	modbuiltins.c \
	modcollections.c \
	modgc.c \
	modio.c \
	modmath.c \
	modcmath.c \
	modmicropython.c \
	modstruct.c \
	modsys.c \
	bc.c \
	vm.c \
	showbc.c \
	repl.c \
	smallint.c \
	frozenmod.c \
	)

TR_SRC_EXTMOD_H = $(addprefix $(BUILD)/extmod/,\
	modubinascii.h \
	)

TR_SRC_EXTMOD_C = $(addprefix $(BUILD)/extmod/,\
	moductypes.c \
	modujson.c \
	modure.c \
	moduheapq.c \
	moduhashlib.c \
	modubinascii.c \
	)

#	py/mpstate.c \
	py/nlrx86.c \
	py/nlrx64.c \
	py/nlrthumb.c \
	py/nlrxtensa.c \
	py/asmx64.c \
	py/emitnx64.c \
	py/asmx86.c \
	py/emitnx86.c \
	py/asmthumb.c \
	py/emitnthumb.c \
	py/asmarm.c \
	py/emitnarm.c \
	py/emitinlinethumb.c \

TR_SRC_GENHDR_H = $(addprefix $(HEADER_BUILD)/,\
	mpversion.h \
	qstrdefs.generated.h \
	)

QSTR_DEFS = ../py/qstrdefs.h
SRC_QSTR = $(TR_SRC_PY_H) $(TR_SRC_PY_C) $(TR_SRC_EXTMOD_H) $(TR_SRC_EXTMOD_C)

TR_SRC_H = $(TR_SRC_PY_H) $(BUILD)/py/grammar.h $(BUILD)/py/vmentrytable.h $(TR_SRC_EXTMOD_H) $(TR_SRC_GENHDR_H)
TR_SRC_C = $(TR_SRC_PY_C) $(TR_SRC_EXTMOD_C)

ifeq ("$(origin V)", "command line")
    BUILD_VERBOSE=$(V)
endif
ifndef BUILD_VERBOSE
    BUILD_VERBOSE = 0
endif
ifeq ($(BUILD_VERBOSE),0)
    $(info Use make V=1 or set BUILD_VERBOSE to increase build verbosity.)
    Q = @
else
    Q =
endif

.PHONY: all clean source amalgamation FORCE

all: embed

# Anything that depends on FORCE will be considered out-of-date
FORCE:

py extmod genhdr:
	$(Q)$(MKDIR) -p $(BUILD)/$@

$(BUILD)/py/%.c: ../py/%.c $(TRANSFORM) | py
	$(ECHO) "TR $<"
	$(Q)python $(TRANSFORM) $< > $@

$(BUILD)/py/%.h: ../py/%.h $(TRANSFORM) | py
	$(ECHO) "TR $<"
	$(Q) python $(TRANSFORM) $< > $@

$(BUILD)/extmod/%.c: ../extmod/%.c $(TRANSFORM) | extmod
	$(ECHO) "TR $<"
	$(Q) python $(TRANSFORM) $< > $@

$(BUILD)/extmod/%.h: ../extmod/%.h $(TRANSFORM) | extmod
	$(ECHO) "TR $<"
	$(Q) python $(TRANSFORM) $< > $@

$(HEADER_BUILD)/mpversion.h: FORCE | genhdr
	$(Q)$(PYTHON) ../py/makeversionhdr.py $@

$(HEADER_BUILD)/qstrdefs.generated.h: $(QSTR_DEFS) $(SRC_QSTR) | genhdr
	$(ECHO) "GEN $@"
	$(Q)cat $(SRC_QSTR) > $(HEADER_BUILD)/abc
	$(Q)$(PYTHON) $(PY_SRC)/makeqstrdefs.py filter $(HEADER_BUILD)/abc $(HEADER_BUILD) $(HEADER_BUILD)/def
	$(Q)echo "QCFG(BYTES_IN_LEN, (1))" > $(HEADER_BUILD)/geh
	$(Q)echo "QCFG(BYTES_IN_HASH, (2))" >> $(HEADER_BUILD)/geh
	$(Q)grep "^Q(" $(QSTR_DEFS) >> $(HEADER_BUILD)/geh
	$(Q)cat $(HEADER_BUILD)/def >> $(HEADER_BUILD)/geh
	$(Q)$(PYTHON) $(PY_SRC)/makeqstrdata.py $(HEADER_BUILD)/geh > $@

source: $(TR_SRC_H) $(TR_SRC_C)

amalgamation: upy.h upy.c

upy.h: $(TR_SRC_H) util.h
	$(ECHO) "GEN $@"
	$(Q)echo '#define MP_CONFIGFILE "mpconfigport.h"' > upy.h
	$(Q)echo '#include <stdarg.h>' >> upy.h
	$(Q)echo '#define MICROPY_QSTR_BYTES_IN_LEN (1)' >> upy.h
	$(Q)echo '#define MICROPY_QSTR_BYTES_IN_HASH (2)' >> upy.h
	$(Q)echo '#define MP_SSIZE_MAX (0x7fffffff)' >> upy.h
	$(Q)cat $(TR_SRC_PY_H) $(TR_SRC_EXTMOD_H) | python $(AMALGAMATE) >> upy.h
	$(Q)cat util.h >> upy.h

upy.c: $(TR_SRC_H) $(TR_SRC_C) util.c
	$(ECHO) "GEN $@"
	$(Q)echo '#include "upy.h"' > upy.c
	$(Q)cat $(TR_SRC_PY_C) $(TR_SRC_EXTMOD_C) | python $(AMALGAMATE) >> upy.c
	$(Q)cat util.c >> upy.c

upy.o: upy.c upy.h
	$(ECHO) "CC $<"
	$(Q)$(CC) -Os -Wall -c $<

libmicropython.a: upy.o
	$(ECHO) "AR $@"
	$(Q)$(AR) rc $@ $^

embed: embed.c libmicropython.a
	$(ECHO) "BUILD $@"
	$(Q)$(CC) embed.c -lm -lmicropython -L. -o $@

clean:
	$(RM) -rf $(BUILD)
	$(RM) -rf upy.c upy.h upy.o
	$(RM) -rf libmicropython.a
	$(RM) -rf embed
