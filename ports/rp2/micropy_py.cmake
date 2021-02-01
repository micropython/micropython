# CMake fragment for MicroPython core py component

set(MPY_PY_DIR "${MPY_DIR}/py")
set(MPY_PY_QSTRDEFS "${MPY_PY_DIR}/qstrdefs.h")
set(MPY_GENHDR_DIR "${CMAKE_BINARY_DIR}/genhdr")
set(MPY_MPVERSION "${MPY_GENHDR_DIR}/mpversion.h")
set(MPY_MODULEDEFS "${MPY_GENHDR_DIR}/moduledefs.h")
set(MPY_QSTR_DEFS_LAST "${MPY_GENHDR_DIR}/qstr.i.last")
set(MPY_QSTR_DEFS_SPLIT "${MPY_GENHDR_DIR}/qstr.split")
set(MPY_QSTR_DEFS_COLLECTED "${MPY_GENHDR_DIR}/qstrdefs.collected.h")
set(MPY_QSTR_DEFS_PREPROCESSED "${MPY_GENHDR_DIR}/qstrdefs.preprocessed.h")
set(MPY_QSTR_DEFS_GENERATED "${MPY_GENHDR_DIR}/qstrdefs.generated.h")
set(MPY_FROZEN_CONTENT "${CMAKE_BINARY_DIR}/frozen_content.c")

# All py/ source files
set(SOURCE_PY
    ${MPY_PY_DIR}/argcheck.c
    ${MPY_PY_DIR}/asmarm.c
    ${MPY_PY_DIR}/asmbase.c
    ${MPY_PY_DIR}/asmthumb.c
    ${MPY_PY_DIR}/asmx64.c
    ${MPY_PY_DIR}/asmx86.c
    ${MPY_PY_DIR}/asmxtensa.c
    ${MPY_PY_DIR}/bc.c
    ${MPY_PY_DIR}/binary.c
    ${MPY_PY_DIR}/builtinevex.c
    ${MPY_PY_DIR}/builtinhelp.c
    ${MPY_PY_DIR}/builtinimport.c
    ${MPY_PY_DIR}/compile.c
    ${MPY_PY_DIR}/emitbc.c
    ${MPY_PY_DIR}/emitcommon.c
    ${MPY_PY_DIR}/emitglue.c
    ${MPY_PY_DIR}/emitinlinethumb.c
    ${MPY_PY_DIR}/emitinlinextensa.c
    ${MPY_PY_DIR}/emitnarm.c
    ${MPY_PY_DIR}/emitnthumb.c
    ${MPY_PY_DIR}/emitnx64.c
    ${MPY_PY_DIR}/emitnx86.c
    ${MPY_PY_DIR}/emitnxtensa.c
    ${MPY_PY_DIR}/emitnxtensawin.c
    ${MPY_PY_DIR}/formatfloat.c
    ${MPY_PY_DIR}/frozenmod.c
    ${MPY_PY_DIR}/gc.c
    ${MPY_PY_DIR}/lexer.c
    ${MPY_PY_DIR}/malloc.c
    ${MPY_PY_DIR}/map.c
    ${MPY_PY_DIR}/modarray.c
    ${MPY_PY_DIR}/modbuiltins.c
    ${MPY_PY_DIR}/modcmath.c
    ${MPY_PY_DIR}/modcollections.c
    ${MPY_PY_DIR}/modgc.c
    ${MPY_PY_DIR}/modio.c
    ${MPY_PY_DIR}/modmath.c
    ${MPY_PY_DIR}/modmicropython.c
    ${MPY_PY_DIR}/modstruct.c
    ${MPY_PY_DIR}/modsys.c
    ${MPY_PY_DIR}/modthread.c
    ${MPY_PY_DIR}/moduerrno.c
    ${MPY_PY_DIR}/mpprint.c
    ${MPY_PY_DIR}/mpstate.c
    ${MPY_PY_DIR}/mpz.c
    ${MPY_PY_DIR}/nativeglue.c
    ${MPY_PY_DIR}/nlr.c
    ${MPY_PY_DIR}/nlrpowerpc.c
    ${MPY_PY_DIR}/nlrsetjmp.c
    ${MPY_PY_DIR}/nlrthumb.c
    ${MPY_PY_DIR}/nlrx64.c
    ${MPY_PY_DIR}/nlrx86.c
    ${MPY_PY_DIR}/nlrxtensa.c
    ${MPY_PY_DIR}/obj.c
    ${MPY_PY_DIR}/objarray.c
    ${MPY_PY_DIR}/objattrtuple.c
    ${MPY_PY_DIR}/objbool.c
    ${MPY_PY_DIR}/objboundmeth.c
    ${MPY_PY_DIR}/objcell.c
    ${MPY_PY_DIR}/objclosure.c
    ${MPY_PY_DIR}/objcomplex.c
    ${MPY_PY_DIR}/objdeque.c
    ${MPY_PY_DIR}/objdict.c
    ${MPY_PY_DIR}/objenumerate.c
    ${MPY_PY_DIR}/objexcept.c
    ${MPY_PY_DIR}/objfilter.c
    ${MPY_PY_DIR}/objfloat.c
    ${MPY_PY_DIR}/objfun.c
    ${MPY_PY_DIR}/objgenerator.c
    ${MPY_PY_DIR}/objgetitemiter.c
    ${MPY_PY_DIR}/objint.c
    ${MPY_PY_DIR}/objint_longlong.c
    ${MPY_PY_DIR}/objint_mpz.c
    ${MPY_PY_DIR}/objlist.c
    ${MPY_PY_DIR}/objmap.c
    ${MPY_PY_DIR}/objmodule.c
    ${MPY_PY_DIR}/objnamedtuple.c
    ${MPY_PY_DIR}/objnone.c
    ${MPY_PY_DIR}/objobject.c
    ${MPY_PY_DIR}/objpolyiter.c
    ${MPY_PY_DIR}/objproperty.c
    ${MPY_PY_DIR}/objrange.c
    ${MPY_PY_DIR}/objreversed.c
    ${MPY_PY_DIR}/objset.c
    ${MPY_PY_DIR}/objsingleton.c
    ${MPY_PY_DIR}/objslice.c
    ${MPY_PY_DIR}/objstr.c
    ${MPY_PY_DIR}/objstringio.c
    ${MPY_PY_DIR}/objstrunicode.c
    ${MPY_PY_DIR}/objtuple.c
    ${MPY_PY_DIR}/objtype.c
    ${MPY_PY_DIR}/objzip.c
    ${MPY_PY_DIR}/opmethods.c
    ${MPY_PY_DIR}/pairheap.c
    ${MPY_PY_DIR}/parse.c
    ${MPY_PY_DIR}/parsenum.c
    ${MPY_PY_DIR}/parsenumbase.c
    ${MPY_PY_DIR}/persistentcode.c
    ${MPY_PY_DIR}/profile.c
    ${MPY_PY_DIR}/pystack.c
    ${MPY_PY_DIR}/qstr.c
    ${MPY_PY_DIR}/reader.c
    ${MPY_PY_DIR}/repl.c
    ${MPY_PY_DIR}/ringbuf.c
    ${MPY_PY_DIR}/runtime.c
    ${MPY_PY_DIR}/runtime_utils.c
    ${MPY_PY_DIR}/scheduler.c
    ${MPY_PY_DIR}/scope.c
    ${MPY_PY_DIR}/sequence.c
    ${MPY_PY_DIR}/showbc.c
    ${MPY_PY_DIR}/smallint.c
    ${MPY_PY_DIR}/stackctrl.c
    ${MPY_PY_DIR}/stream.c
    ${MPY_PY_DIR}/unicode.c
    ${MPY_PY_DIR}/vm.c
    ${MPY_PY_DIR}/vstr.c
    ${MPY_PY_DIR}/warning.c
)
