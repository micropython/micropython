/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2014-2017 Paul Sokolovsky
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "py/builtin.h"
#include "py/objlist.h"
#include "py/objmodule.h"
#include "py/objtuple.h"
#include "py/objstr.h"
#include "py/objint.h"
#include "py/objtype.h"
#include "py/stream.h"
#include "py/smallint.h"
#include "py/runtime.h"
#include "py/persistentcode.h"
#include "extmod/modplatform.h"
#include "genhdr/mpversion.h"

#if MICROPY_PY_SYS_SETTRACE
#include "py/objmodule.h"
#include "py/profile.h"
#endif

#if MICROPY_PY_SYS

// defined per port; type of these is irrelevant, just need pointer
extern struct _mp_dummy_t mp_sys_stdin_obj;
extern struct _mp_dummy_t mp_sys_stdout_obj;
extern struct _mp_dummy_t mp_sys_stderr_obj;

#if MICROPY_PY_IO && MICROPY_PY_SYS_STDFILES
const mp_print_t mp_sys_stdout_print = {&mp_sys_stdout_obj, mp_stream_write_adaptor};
#endif

// version - Python language version that this implementation conforms to, as a string
static const MP_DEFINE_STR_OBJ(mp_sys_version_obj, "3.4.0; " MICROPY_BANNER_NAME_AND_VERSION);

// version_info - Python language version that this implementation conforms to, as a tuple of ints
// TODO: CPython is now at 5-element array (major, minor, micro, releaselevel, serial), but save 2 els so far...
static const mp_rom_obj_tuple_t mp_sys_version_info_obj = {{&mp_type_tuple}, 3, {MP_ROM_INT(3), MP_ROM_INT(4), MP_ROM_INT(0)}};

// sys.implementation object
// this holds the MicroPython version
static const mp_rom_obj_tuple_t mp_sys_implementation_version_info_obj = {
    {&mp_type_tuple},
    4,
    {
        MP_ROM_INT(MICROPY_VERSION_MAJOR),
        MP_ROM_INT(MICROPY_VERSION_MINOR),
        MP_ROM_INT(MICROPY_VERSION_MICRO),
        #if MICROPY_VERSION_PRERELEASE
        MP_ROM_QSTR(MP_QSTR_preview),
        #else
        MP_ROM_QSTR(MP_QSTR_),
        #endif
    }
};
static const MP_DEFINE_STR_OBJ(mp_sys_implementation_machine_obj, MICROPY_BANNER_MACHINE);
#define SYS_IMPLEMENTATION_ELEMS_BASE \
    MP_ROM_QSTR(MP_QSTR_micropython), \
    MP_ROM_PTR(&mp_sys_implementation_version_info_obj), \
    MP_ROM_PTR(&mp_sys_implementation_machine_obj)

#if MICROPY_PERSISTENT_CODE_LOAD
#define SYS_IMPLEMENTATION_ELEMS__MPY \
    , MP_ROM_INT(MPY_FILE_HEADER_INT)
#else
#define SYS_IMPLEMENTATION_ELEMS__MPY
#endif

#if MICROPY_PY_ATTRTUPLE
#if MICROPY_PREVIEW_VERSION_2
#define SYS_IMPLEMENTATION_ELEMS__V2 \
    , MP_ROM_TRUE
#else
#define SYS_IMPLEMENTATION_ELEMS__V2
#endif

static const qstr impl_fields[] = {
    MP_QSTR_name,
    MP_QSTR_version,
    MP_QSTR__machine,
    #if MICROPY_PERSISTENT_CODE_LOAD
    MP_QSTR__mpy,
    #endif
    #if MICROPY_PREVIEW_VERSION_2
    MP_QSTR__v2,
    #endif
};
static MP_DEFINE_ATTRTUPLE(
    mp_sys_implementation_obj,
    impl_fields,
    3 + MICROPY_PERSISTENT_CODE_LOAD + MICROPY_PREVIEW_VERSION_2,
    SYS_IMPLEMENTATION_ELEMS_BASE
    SYS_IMPLEMENTATION_ELEMS__MPY
    SYS_IMPLEMENTATION_ELEMS__V2
    );
#else
static const mp_rom_obj_tuple_t mp_sys_implementation_obj = {
    {&mp_type_tuple},
    3 + MICROPY_PERSISTENT_CODE_LOAD,
    // Do not include SYS_IMPLEMENTATION_ELEMS__V2 because
    // SYS_IMPLEMENTATION_ELEMS__MPY may be empty if
    // MICROPY_PERSISTENT_CODE_LOAD is disabled, which means they'll share
    // the same index. Cannot query _v2 if MICROPY_PY_ATTRTUPLE is
    // disabled.
    {
        SYS_IMPLEMENTATION_ELEMS_BASE
                                SYS_IMPLEMENTATION_ELEMS__MPY
    }
};
#endif

#undef I

#ifdef MICROPY_PY_SYS_PLATFORM
// platform - the platform that MicroPython is running on
static const MP_DEFINE_STR_OBJ(mp_sys_platform_obj, MICROPY_PY_SYS_PLATFORM);
#endif

#ifdef MICROPY_PY_SYS_EXECUTABLE
// executable - the path to the micropython binary
// This object is non-const and is populated at startup in main()
MP_DEFINE_STR_OBJ(mp_sys_executable_obj, "");
#endif

#if MICROPY_PY_SYS_INTERN
MP_DEFINE_CONST_FUN_OBJ_1(mp_sys_intern_obj, mp_obj_str_intern_checked);
#endif

// exit([retval]): raise SystemExit, with optional argument given to the exception
static mp_obj_t mp_sys_exit(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        mp_raise_type(&mp_type_SystemExit);
    } else {
        mp_raise_type_arg(&mp_type_SystemExit, args[0]);
    }
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_sys_exit_obj, 0, 1, mp_sys_exit);

static mp_obj_t mp_sys_print_exception(size_t n_args, const mp_obj_t *args) {
    #if MICROPY_PY_IO && MICROPY_PY_SYS_STDFILES
    void *stream_obj = &mp_sys_stdout_obj;
    if (n_args > 1) {
        mp_get_stream_raise(args[1], MP_STREAM_OP_WRITE);
        stream_obj = MP_OBJ_TO_PTR(args[1]);
    }

    mp_print_t print = {stream_obj, mp_stream_write_adaptor};
    mp_obj_print_exception(&print, args[0]);
    #else
    (void)n_args;
    mp_obj_print_exception(&mp_plat_print, args[0]);
    #endif

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_sys_print_exception_obj, 1, 2, mp_sys_print_exception);

#if MICROPY_PY_SYS_EXC_INFO
static mp_obj_t mp_sys_exc_info(void) {
    mp_obj_t cur_exc = MP_OBJ_FROM_PTR(MP_STATE_VM(cur_exception));
    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(3, NULL));

    if (cur_exc == MP_OBJ_NULL) {
        t->items[0] = mp_const_none;
        t->items[1] = mp_const_none;
        t->items[2] = mp_const_none;
        return MP_OBJ_FROM_PTR(t);
    }

    t->items[0] = MP_OBJ_FROM_PTR(mp_obj_get_type(cur_exc));
    t->items[1] = cur_exc;
    t->items[2] = mp_const_none;
    return MP_OBJ_FROM_PTR(t);
}
MP_DEFINE_CONST_FUN_OBJ_0(mp_sys_exc_info_obj, mp_sys_exc_info);
#endif

#if MICROPY_PY_SYS_GETSIZEOF
static mp_obj_t mp_sys_getsizeof(mp_obj_t obj) {
    return mp_unary_op(MP_UNARY_OP_SIZEOF, obj);
}
static MP_DEFINE_CONST_FUN_OBJ_1(mp_sys_getsizeof_obj, mp_sys_getsizeof);
#endif

#if MICROPY_PY_SYS_ATEXIT
// atexit(callback): Callback is called when sys.exit is called.
static mp_obj_t mp_sys_atexit(mp_obj_t obj) {
    mp_obj_t old = MP_STATE_VM(sys_exitfunc);
    MP_STATE_VM(sys_exitfunc) = obj;
    return old;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mp_sys_atexit_obj, mp_sys_atexit);
#endif

#if MICROPY_PY_SYS_SETTRACE
// settrace(tracefunc): Set the system's trace function.
static mp_obj_t mp_sys_settrace(mp_obj_t obj) {
    return mp_prof_settrace(obj);
}
MP_DEFINE_CONST_FUN_OBJ_1(mp_sys_settrace_obj, mp_sys_settrace);
#endif // MICROPY_PY_SYS_SETTRACE

#if MICROPY_PY_SYS_PATH && !MICROPY_PY_SYS_ATTR_DELEGATION
#error "MICROPY_PY_SYS_PATH requires MICROPY_PY_SYS_ATTR_DELEGATION"
#endif

#if MICROPY_PY_SYS_PS1_PS2 && !MICROPY_PY_SYS_ATTR_DELEGATION
#error "MICROPY_PY_SYS_PS1_PS2 requires MICROPY_PY_SYS_ATTR_DELEGATION"
#endif

#if MICROPY_PY_SYS_TRACEBACKLIMIT && !MICROPY_PY_SYS_ATTR_DELEGATION
#error "MICROPY_PY_SYS_TRACEBACKLIMIT requires MICROPY_PY_SYS_ATTR_DELEGATION"
#endif

#if MICROPY_PY_SYS_ATTR_DELEGATION && !MICROPY_MODULE_ATTR_DELEGATION
#error "MICROPY_PY_SYS_ATTR_DELEGATION requires MICROPY_MODULE_ATTR_DELEGATION"
#endif

#if MICROPY_PY_SYS_ATTR_DELEGATION
// Must be kept in sync with the enum at the top of mpstate.h.
static const uint16_t sys_mutable_keys[] = {
    #if MICROPY_PY_SYS_PATH
    // Code should access this (as an mp_obj_t) for use with e.g.
    // mp_obj_list_append by using the `mp_sys_path` macro defined in runtime.h.
    MP_QSTR_path,
    #endif
    #if MICROPY_PY_SYS_PS1_PS2
    MP_QSTR_ps1,
    MP_QSTR_ps2,
    #endif
    #if MICROPY_PY_SYS_TRACEBACKLIMIT
    MP_QSTR_tracebacklimit,
    #endif
    MP_QSTRnull,
};

void mp_module_sys_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    MP_STATIC_ASSERT(MP_ARRAY_SIZE(sys_mutable_keys) == MP_SYS_MUTABLE_NUM + 1);
    MP_STATIC_ASSERT(MP_ARRAY_SIZE(MP_STATE_VM(sys_mutable)) == MP_SYS_MUTABLE_NUM);
    mp_module_generic_attr(attr, dest, sys_mutable_keys, MP_STATE_VM(sys_mutable));
}
#endif

static const mp_rom_map_elem_t mp_module_sys_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_sys) },

    #if MICROPY_PY_SYS_ARGV
    { MP_ROM_QSTR(MP_QSTR_argv), MP_ROM_PTR(&MP_STATE_VM(mp_sys_argv_obj)) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_version), MP_ROM_PTR(&mp_sys_version_obj) },
    { MP_ROM_QSTR(MP_QSTR_version_info), MP_ROM_PTR(&mp_sys_version_info_obj) },
    { MP_ROM_QSTR(MP_QSTR_implementation), MP_ROM_PTR(&mp_sys_implementation_obj) },
    #ifdef MICROPY_PY_SYS_PLATFORM
    { MP_ROM_QSTR(MP_QSTR_platform), MP_ROM_PTR(&mp_sys_platform_obj) },
    #endif
    #if MP_ENDIANNESS_LITTLE
    { MP_ROM_QSTR(MP_QSTR_byteorder), MP_ROM_QSTR(MP_QSTR_little) },
    #else
    { MP_ROM_QSTR(MP_QSTR_byteorder), MP_ROM_QSTR(MP_QSTR_big) },
    #endif

    #if MICROPY_PY_SYS_MAXSIZE
    #if MICROPY_LONGINT_IMPL == MICROPY_LONGINT_IMPL_NONE
    // Maximum mp_int_t value is not representable as small int, so we have
    // little choice but to use MP_SMALL_INT_MAX. Apps also should be careful
    // to not try to compare sys.maxsize to some literal number (as this
    // number might not fit in available int size), but instead count number
    // of "one" bits in sys.maxsize.
    { MP_ROM_QSTR(MP_QSTR_maxsize), MP_ROM_INT(MP_SMALL_INT_MAX) },
    #else
    { MP_ROM_QSTR(MP_QSTR_maxsize), MP_ROM_PTR(&mp_sys_maxsize_obj) },
    #endif
    #endif

    #if MICROPY_PY_SYS_INTERN
    { MP_ROM_QSTR(MP_QSTR_intern), MP_ROM_PTR(&mp_sys_intern_obj) },
    #endif

    #if MICROPY_PY_SYS_EXIT
    { MP_ROM_QSTR(MP_QSTR_exit), MP_ROM_PTR(&mp_sys_exit_obj) },
    #endif

    #if MICROPY_PY_SYS_SETTRACE
    { MP_ROM_QSTR(MP_QSTR_settrace), MP_ROM_PTR(&mp_sys_settrace_obj) },
    #endif

    #if MICROPY_PY_SYS_STDFILES
    { MP_ROM_QSTR(MP_QSTR_stdin), MP_ROM_PTR(&mp_sys_stdin_obj) },
    { MP_ROM_QSTR(MP_QSTR_stdout), MP_ROM_PTR(&mp_sys_stdout_obj) },
    { MP_ROM_QSTR(MP_QSTR_stderr), MP_ROM_PTR(&mp_sys_stderr_obj) },
    #endif

    #if MICROPY_PY_SYS_MODULES
    { MP_ROM_QSTR(MP_QSTR_modules), MP_ROM_PTR(&MP_STATE_VM(mp_loaded_modules_dict)) },
    #endif
    #if MICROPY_PY_SYS_EXC_INFO
    { MP_ROM_QSTR(MP_QSTR_exc_info), MP_ROM_PTR(&mp_sys_exc_info_obj) },
    #endif
    #if MICROPY_PY_SYS_GETSIZEOF
    { MP_ROM_QSTR(MP_QSTR_getsizeof), MP_ROM_PTR(&mp_sys_getsizeof_obj) },
    #endif

    #if MICROPY_PY_SYS_EXECUTABLE
    { MP_ROM_QSTR(MP_QSTR_executable), MP_ROM_PTR(&mp_sys_executable_obj) },
    #endif

    /*
     * Extensions to CPython
     */

    { MP_ROM_QSTR(MP_QSTR_print_exception), MP_ROM_PTR(&mp_sys_print_exception_obj) },
    #if MICROPY_PY_SYS_ATEXIT
    { MP_ROM_QSTR(MP_QSTR_atexit), MP_ROM_PTR(&mp_sys_atexit_obj) },
    #endif
};

static MP_DEFINE_CONST_DICT(mp_module_sys_globals, mp_module_sys_globals_table);

const mp_obj_module_t mp_module_sys = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_sys_globals,
};

// Unlike the other CPython-compatible modules, sys is not extensible from the
// filesystem. We rely on it to work so that things like sys.path are always
// available.
MP_REGISTER_MODULE(MP_QSTR_sys, mp_module_sys);

#if MICROPY_PY_SYS_ARGV
// Code should access this (as an mp_obj_t) for use with e.g.
// mp_obj_list_append by using the `mp_sys_argv` macro defined in runtime.h.
MP_REGISTER_ROOT_POINTER(mp_obj_list_t mp_sys_argv_obj);
#endif

#if MICROPY_PY_SYS_EXC_INFO
// current exception being handled, for sys.exc_info()
MP_REGISTER_ROOT_POINTER(mp_obj_base_t * cur_exception);
#endif

#if MICROPY_PY_SYS_ATEXIT
// exposed through sys.atexit function
MP_REGISTER_ROOT_POINTER(mp_obj_t sys_exitfunc);
#endif

#if MICROPY_PY_SYS_ATTR_DELEGATION
// Contains mutable sys attributes.
MP_REGISTER_ROOT_POINTER(mp_obj_t sys_mutable[MP_SYS_MUTABLE_NUM]);
MP_REGISTER_MODULE_DELEGATION(mp_module_sys, mp_module_sys_attr);
#endif

#endif // MICROPY_PY_SYS
