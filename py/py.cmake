# CMake fragment for MicroPython core py component

set(MICROPY_PY_DIR "${MICROPY_DIR}/py")

list(APPEND MICROPY_INC_CORE "${MICROPY_DIR}")

# All py/ source files
set(MICROPY_SOURCE_PY
    ${MICROPY_PY_DIR}/argcheck.c
    ${MICROPY_PY_DIR}/asmarm.c
    ${MICROPY_PY_DIR}/asmbase.c
    ${MICROPY_PY_DIR}/asmrv32.c
    ${MICROPY_PY_DIR}/asmthumb.c
    ${MICROPY_PY_DIR}/asmx64.c
    ${MICROPY_PY_DIR}/asmx86.c
    ${MICROPY_PY_DIR}/asmxtensa.c
    ${MICROPY_PY_DIR}/bc.c
    ${MICROPY_PY_DIR}/binary.c
    ${MICROPY_PY_DIR}/builtinevex.c
    ${MICROPY_PY_DIR}/builtinhelp.c
    ${MICROPY_PY_DIR}/builtinimport.c
    ${MICROPY_PY_DIR}/compile.c
    ${MICROPY_PY_DIR}/cstack.c
    ${MICROPY_PY_DIR}/emitbc.c
    ${MICROPY_PY_DIR}/emitcommon.c
    ${MICROPY_PY_DIR}/emitglue.c
    ${MICROPY_PY_DIR}/emitinlinerv32.c
    ${MICROPY_PY_DIR}/emitinlinethumb.c
    ${MICROPY_PY_DIR}/emitinlinextensa.c
    ${MICROPY_PY_DIR}/emitnarm.c
    ${MICROPY_PY_DIR}/emitndebug.c
    ${MICROPY_PY_DIR}/emitnrv32.c
    ${MICROPY_PY_DIR}/emitnthumb.c
    ${MICROPY_PY_DIR}/emitnx64.c
    ${MICROPY_PY_DIR}/emitnx86.c
    ${MICROPY_PY_DIR}/emitnxtensa.c
    ${MICROPY_PY_DIR}/emitnxtensawin.c
    ${MICROPY_PY_DIR}/formatfloat.c
    ${MICROPY_PY_DIR}/frozenmod.c
    ${MICROPY_PY_DIR}/gc.c
    ${MICROPY_PY_DIR}/lexer.c
    ${MICROPY_PY_DIR}/malloc.c
    ${MICROPY_PY_DIR}/map.c
    ${MICROPY_PY_DIR}/modarray.c
    ${MICROPY_PY_DIR}/modbuiltins.c
    ${MICROPY_PY_DIR}/modcmath.c
    ${MICROPY_PY_DIR}/modcollections.c
    ${MICROPY_PY_DIR}/modgc.c
    ${MICROPY_PY_DIR}/modio.c
    ${MICROPY_PY_DIR}/modmath.c
    ${MICROPY_PY_DIR}/modmicropython.c
    ${MICROPY_PY_DIR}/modstruct.c
    ${MICROPY_PY_DIR}/modsys.c
    ${MICROPY_PY_DIR}/modthread.c
    ${MICROPY_PY_DIR}/moderrno.c
    ${MICROPY_PY_DIR}/mpprint.c
    ${MICROPY_PY_DIR}/mpstate.c
    ${MICROPY_PY_DIR}/mpz.c
    ${MICROPY_PY_DIR}/nativeglue.c
    ${MICROPY_PY_DIR}/nlr.c
    ${MICROPY_PY_DIR}/nlrmips.c
    ${MICROPY_PY_DIR}/nlrpowerpc.c
    ${MICROPY_PY_DIR}/nlrrv32.c
    ${MICROPY_PY_DIR}/nlrrv64.c
    ${MICROPY_PY_DIR}/nlrsetjmp.c
    ${MICROPY_PY_DIR}/nlrthumb.c
    ${MICROPY_PY_DIR}/nlrx64.c
    ${MICROPY_PY_DIR}/nlrx86.c
    ${MICROPY_PY_DIR}/nlrxtensa.c
    ${MICROPY_PY_DIR}/obj.c
    ${MICROPY_PY_DIR}/objarray.c
    ${MICROPY_PY_DIR}/objattrtuple.c
    ${MICROPY_PY_DIR}/objbool.c
    ${MICROPY_PY_DIR}/objboundmeth.c
    ${MICROPY_PY_DIR}/objcode.c
    ${MICROPY_PY_DIR}/objcell.c
    ${MICROPY_PY_DIR}/objclosure.c
    ${MICROPY_PY_DIR}/objcomplex.c
    ${MICROPY_PY_DIR}/objdeque.c
    ${MICROPY_PY_DIR}/objdict.c
    ${MICROPY_PY_DIR}/objenumerate.c
    ${MICROPY_PY_DIR}/objexcept.c
    ${MICROPY_PY_DIR}/objfilter.c
    ${MICROPY_PY_DIR}/objfloat.c
    ${MICROPY_PY_DIR}/objfun.c
    ${MICROPY_PY_DIR}/objgenerator.c
    ${MICROPY_PY_DIR}/objgetitemiter.c
    ${MICROPY_PY_DIR}/objint.c
    ${MICROPY_PY_DIR}/objint_longlong.c
    ${MICROPY_PY_DIR}/objint_mpz.c
    ${MICROPY_PY_DIR}/objlist.c
    ${MICROPY_PY_DIR}/objmap.c
    ${MICROPY_PY_DIR}/objmodule.c
    ${MICROPY_PY_DIR}/objnamedtuple.c
    ${MICROPY_PY_DIR}/objnone.c
    ${MICROPY_PY_DIR}/objobject.c
    ${MICROPY_PY_DIR}/objpolyiter.c
    ${MICROPY_PY_DIR}/objproperty.c
    ${MICROPY_PY_DIR}/objrange.c
    ${MICROPY_PY_DIR}/objreversed.c
    ${MICROPY_PY_DIR}/objringio.c
    ${MICROPY_PY_DIR}/objset.c
    ${MICROPY_PY_DIR}/objsingleton.c
    ${MICROPY_PY_DIR}/objslice.c
    ${MICROPY_PY_DIR}/objstr.c
    ${MICROPY_PY_DIR}/objstringio.c
    ${MICROPY_PY_DIR}/objstrunicode.c
    ${MICROPY_PY_DIR}/objtuple.c
    ${MICROPY_PY_DIR}/objtype.c
    ${MICROPY_PY_DIR}/objzip.c
    ${MICROPY_PY_DIR}/opmethods.c
    ${MICROPY_PY_DIR}/pairheap.c
    ${MICROPY_PY_DIR}/parse.c
    ${MICROPY_PY_DIR}/parsenum.c
    ${MICROPY_PY_DIR}/parsenumbase.c
    ${MICROPY_PY_DIR}/persistentcode.c
    ${MICROPY_PY_DIR}/profile.c
    ${MICROPY_PY_DIR}/pystack.c
    ${MICROPY_PY_DIR}/qstr.c
    ${MICROPY_PY_DIR}/reader.c
    ${MICROPY_PY_DIR}/repl.c
    ${MICROPY_PY_DIR}/ringbuf.c
    ${MICROPY_PY_DIR}/runtime.c
    ${MICROPY_PY_DIR}/runtime_utils.c
    ${MICROPY_PY_DIR}/scheduler.c
    ${MICROPY_PY_DIR}/scope.c
    ${MICROPY_PY_DIR}/sequence.c
    ${MICROPY_PY_DIR}/showbc.c
    ${MICROPY_PY_DIR}/smallint.c
    ${MICROPY_PY_DIR}/stackctrl.c
    ${MICROPY_PY_DIR}/stream.c
    ${MICROPY_PY_DIR}/unicode.c
    ${MICROPY_PY_DIR}/vm.c
    ${MICROPY_PY_DIR}/vstr.c
    ${MICROPY_PY_DIR}/warning.c
)

# Helper macro to collect include directories and compile definitions for qstr processing.
macro(micropy_gather_target_properties targ)
    if(TARGET ${targ})
        get_target_property(type ${targ} TYPE)
        set(_inc OFF)
        set(_def OFF)
        if(${type} STREQUAL STATIC_LIBRARY)
            get_target_property(_inc ${targ} INCLUDE_DIRECTORIES)
            get_target_property(_def ${targ} COMPILE_DEFINITIONS)
        elseif(${type} STREQUAL INTERFACE_LIBRARY)
            get_target_property(_inc ${targ} INTERFACE_INCLUDE_DIRECTORIES)
            get_target_property(_def ${targ} INTERFACE_COMPILE_DEFINITIONS)
        endif()
        if(_inc)
            list(APPEND MICROPY_CPP_INC_EXTRA ${_inc})
        endif()
        if(_def)
            list(APPEND MICROPY_CPP_DEF_EXTRA ${_def})
        endif()
    endif()
endmacro()
