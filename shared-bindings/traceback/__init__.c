/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 microDev
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

#include "py/stream.h"
#include "py/runtime.h"

#include "shared-module/traceback/__init__.h"

//| """Traceback Module
//|
//| This module provides a standard interface to print stack traces of programs.
//| This is useful when you want to print stack traces under program control.
//|
//| |see_cpython_module| :mod:`cpython:traceback`.
//| """
//| ...
//|

STATIC void traceback_exception_common(bool is_print_exception, mp_print_t *print, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_exc, ARG_value, ARG_tb, ARG_limit, ARG_file, ARG_chain };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_,      MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_value, MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_tb,    MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_limit, MP_ARG_OBJ,  {.u_obj = mp_const_none} },
        { MP_QSTR_file,  MP_ARG_OBJ,  {.u_obj = mp_const_none} },
        { MP_QSTR_chain, MP_ARG_BOOL, {.u_bool = true} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t value = args[ARG_value].u_obj;
    if (value == MP_OBJ_NULL) {
        value = args[ARG_exc].u_obj;
    }
    mp_obj_t tb_obj = args[ARG_tb].u_obj;
    mp_obj_t limit_obj = args[ARG_limit].u_obj;
    #if MICROPY_CPYTHON_EXCEPTION_CHAIN
    bool chain = args[ARG_chain].u_bool;
    #endif

    if (args[ARG_file].u_obj != mp_const_none) {
        if (!is_print_exception) {
            #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
            mp_arg_error_terse_mismatch();
            #else
            mp_raise_msg_varg(&mp_type_TypeError, MP_ERROR_TEXT("unexpected keyword argument '%q'"), MP_QSTR_file);
            #endif

        }
        #if MICROPY_PY_IO && MICROPY_PY_SYS_STDFILES
        mp_get_stream_raise(args[ARG_file].u_obj, MP_STREAM_OP_WRITE);
        print->data = MP_OBJ_TO_PTR(args[ARG_file].u_obj);
        print->print_strn = mp_stream_write_adaptor;
        #else
        mp_raise_NotImplementedError(translate("file write is not available"));
        #endif
    }

    if (!mp_obj_is_exception_instance(value)) {
        mp_raise_TypeError(translate("invalid exception"));
    }

    mp_int_t limit = 0;
    bool print_tb = true;
    if (limit_obj != mp_const_none) {
        limit = mp_obj_get_int(limit_obj);
        print_tb = (limit != 0);
    }

    mp_obj_exception_t *exc = mp_obj_exception_get_native(value);
    mp_obj_traceback_t *trace_backup = exc->traceback;
    #if MICROPY_CPYTHON_EXCEPTION_CHAIN
    mp_obj_exception_t *context_backup = exc->context;
    mp_obj_exception_t *cause_backup = exc->cause;

    if (!chain) {
        exc->context = NULL;
        exc->cause = NULL;
    }
    #endif

    if (tb_obj == MP_OBJ_NULL) {
        /* Print the traceback's exception as is */
    } else if (tb_obj != mp_const_none && print_tb) {
        exc->traceback = mp_arg_validate_type(tb_obj, &mp_type_traceback, MP_QSTR_tb);
    } else {
        exc->traceback = (mp_obj_traceback_t *)&mp_const_empty_traceback_obj;
    }

    shared_module_traceback_print_exception(MP_OBJ_TO_PTR(value), print, limit);
    exc->traceback = trace_backup;
    #if MICROPY_CPYTHON_EXCEPTION_CHAIN
    exc->context = context_backup;
    exc->cause = cause_backup;
    #endif
}

//| def format_exception(
//|     exc: BaseException | Type[BaseException],
//|     /,
//|     value: Optional[BaseException] = None,
//|     tb: Optional[TracebackType] = None,
//|     limit: Optional[int] = None,
//|     chain: Optional[bool] = True,
//| ) -> List[str]:
//|     """Format a stack trace and the exception information.
//|
//|     If the exception value is passed in ``exc``, then this exception value and its
//|     associated traceback are used. This is compatible with CPython 3.10 and newer.
//|
//|     If the exception value is passed in ``value``, then any value passed in for
//|     ``exc`` is ignored.  ``value`` is used as the exception value and the
//|     traceback in the ``tb`` argument is used.  In this case, if ``tb`` is None,
//|     no traceback will be shown. This is compatible with CPython 3.5 and
//|     newer.
//|
//|     The arguments have the same meaning as the corresponding arguments
//|     to print_exception().  The return value is a list of strings, each
//|     ending in a newline and some containing internal newlines.  When
//|     these lines are concatenated and printed, exactly the same text is
//|     printed as does print_exception().
//|
//|     :param exc: The exception. Must be an instance of `BaseException`. Unused if value is specified.
//|     :param value: If specified, is used in place of ``exc``.
//|     :param TracebackType tb: When value is alsp specified, ``tb`` is used in place of the exception's own traceback. If `None`, the traceback will not be printed.
//|     :param int limit: Print up to limit stack trace entries (starting from the caller’s frame) if limit is positive.
//|                       Otherwise, print the last ``abs(limit)`` entries. If limit is omitted or None, all entries are printed.
//|     :param bool chain: If `True` then chained exceptions will be printed.
//|     """
//|
STATIC mp_obj_t traceback_format_exception(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mp_print_t print;
    vstr_t vstr;
    vstr_init_print(&vstr, 0, &print);
    traceback_exception_common(false, &print, n_args, pos_args, kw_args);
    mp_obj_t output = mp_obj_new_str_from_vstr(&vstr);
    return mp_obj_new_list(1, &output);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_KW(traceback_format_exception_obj, 0, traceback_format_exception);

//| def print_exception(
//|     exc: BaseException | Type[BaseException],
//|     /,
//|     value: Optional[BaseException] = None,
//|     tb: Optional[TracebackType] = None,
//|     limit: Optional[int] = None,
//|     file: Optional[io.FileIO] = None,
//|     chain: Optional[bool] = True,
//| ) -> None:
//|     """Prints exception information and stack trace entries.
//|
//|     If the exception value is passed in ``exc``, then this exception value and its
//|     associated traceback are used. This is compatible with CPython 3.10 and newer.
//|
//|     If the exception value is passed in ``value``, then any value passed in for
//|     ``exc`` is ignored.  ``value`` is used as the exception value and the
//|     traceback in the ``tb`` argument is used.  In this case, if ``tb`` is None,
//|     no traceback will be shown. This is compatible with CPython 3.5 and
//|     newer.
//|
//|     :param exc: The exception. Must be an instance of `BaseException`. Unused if value is specified.
//|     :param value: If specified, is used in place of ``exc``.
//|     :param tb: When value is alsp specified, ``tb`` is used in place of the exception's own traceback. If `None`, the traceback will not be printed.
//|     :param int limit: Print up to limit stack trace entries (starting from the caller’s frame) if limit is positive.
//|                       Otherwise, print the last ``abs(limit)`` entries. If limit is omitted or None, all entries are printed.
//|     :param io.FileIO file: If file is omitted or `None`, the output goes to `sys.stderr`; otherwise it should be an open
//|                            file or file-like object to receive the output.
//|     :param bool chain: If `True` then chained exceptions will be printed.
//|
//|     """
//|     ...
//|

STATIC mp_obj_t traceback_print_exception(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mp_print_t print = mp_plat_print;
    traceback_exception_common(true, &print, n_args, pos_args, kw_args);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(traceback_print_exception_obj, 0, traceback_print_exception);

STATIC const mp_rom_map_elem_t traceback_module_globals_table[] = {
    // module name
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_traceback) },
    // module functions
    { MP_ROM_QSTR(MP_QSTR_format_exception), MP_ROM_PTR(&traceback_format_exception_obj) },
    { MP_ROM_QSTR(MP_QSTR_print_exception), MP_ROM_PTR(&traceback_print_exception_obj) },
};
STATIC MP_DEFINE_CONST_DICT(traceback_module_globals, traceback_module_globals_table);

const mp_obj_module_t traceback_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&traceback_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_traceback, traceback_module);
