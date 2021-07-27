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

#include "supervisor/shared/stack.h"

//| """Traceback Module
//|
//| This module provides a standard interface to print stack traces of programs.
//| This is useful when you want to print stack traces under program control.
//|
//| """
//| ...
//|

//| def print_exception(etype: Type[BaseException], value: BaseException, tb: TracebackType,
//|                     limit: Optional[int] = None, file: Optional[io.FileIO] = None, chain: Optional[bool] = True) -> None:
//|
//|     """Prints exception information and stack trace entries.
//|
//|     .. note: Setting `chain` will have no effect as chained exceptions are not yet implemented.
//|
//|     :param Type[BaseException] etype: This is ignored and inferred from the type of ``value``.
//|     :param BaseException value: The exception. Must be an instance of `BaseException`.
//|     :param TracebackType tb: The traceback object. If `None`, the traceback will not be printed.
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
    enum { ARG_etype, ARG_value, ARG_tb, ARG_limit, ARG_file, ARG_chain };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_etype, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_value, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_tb,    MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_limit, MP_ARG_OBJ,  {.u_obj = mp_const_none} },
        { MP_QSTR_file,  MP_ARG_OBJ,  {.u_obj = mp_const_none} },
        { MP_QSTR_chain, MP_ARG_BOOL, {.u_bool = true} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t exc = args[ARG_value].u_obj;
    if (!mp_obj_is_exception_instance(exc)) {
        mp_raise_TypeError(translate("invalid exception"));
    }

    mp_print_t print = mp_plat_print;
    if (args[ARG_file].u_obj != mp_const_none) {
        #if MICROPY_PY_IO && MICROPY_PY_SYS_STDFILES
        mp_get_stream_raise(args[ARG_file].u_obj, MP_STREAM_OP_WRITE);
        print.data = MP_OBJ_TO_PTR(args[ARG_file].u_obj);
        print.print_strn = mp_stream_write_adaptor;
        #else
        mp_raise_NotImplementedError(translate("file write is not available"));
        #endif
    }

    mp_int_t limit = 0;
    bool print_tb = true;
    if (args[ARG_limit].u_obj != mp_const_none) {
        if (!mp_obj_get_int_maybe(args[ARG_limit].u_obj, &limit)) {
            mp_raise_TypeError(translate("limit should be an int"));
        }
        print_tb = !(limit == 0);
    }

    if (args[ARG_tb].u_obj != mp_const_none && print_tb) {
        if (!stack_ok()) {
            mp_raise_RuntimeError(translate("stack is not ok"));
        }
        size_t n, *values;
        mp_obj_exception_get_traceback(exc, &n, &values);
        if (n > 0) {
            assert(n % 3 == 0);
            // Decompress the format strings
            const compressed_string_t *traceback = MP_ERROR_TEXT("Traceback (most recent call last):\n");
            char decompressed[decompress_length(traceback)];
            decompress(traceback, decompressed);
            #if MICROPY_ENABLE_SOURCE_LINE
            const compressed_string_t *frame = MP_ERROR_TEXT("  File \"%q\", line %d");
            #else
            const compressed_string_t *frame = MP_ERROR_TEXT("  File \"%q\"");
            #endif
            char decompressed_frame[decompress_length(frame)];
            decompress(frame, decompressed_frame);
            const compressed_string_t *block_fmt = MP_ERROR_TEXT(", in %q\n");
            char decompressed_block[decompress_length(block_fmt)];
            decompress(block_fmt, decompressed_block);

            // Set traceback formatting
            // Default: Print full traceback
            int i = n - 3, j;
            if (limit > 0) {
                // Print upto limit traceback
                // from caller's frame
                limit = n - (limit * 3);
            } else if (limit < 0) {
                // Print upto limit traceback
                // from last
                i = 0, limit = 3 + (limit * 3);
            }

            // Print the traceback
            mp_print_str(&print, decompressed);
            for (; i >= limit; i -= 3) {
                j = (i < 0) ? -i : i;
                #if MICROPY_ENABLE_SOURCE_LINE
                mp_printf(&print, decompressed_frame, values[j], (int)values[j + 1]);
                #else
                mp_printf(&print, decompressed_frame, values[j]);
                #endif
                // The block name can be NULL if it's unknown
                qstr block = values[j + 2];
                if (block == MP_QSTRnull) {
                    mp_print_str(&print, "\n");
                } else {
                    mp_printf(&print, decompressed_block, block);
                }
            }
        }
    }
    mp_obj_print_helper(&print, exc, PRINT_EXC);
    mp_print_str(&print, "\n");
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(traceback_print_exception_obj, 3, traceback_print_exception);

STATIC const mp_rom_map_elem_t traceback_module_globals_table[] = {
    // module name
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_traceback) },
    // module functions
    { MP_ROM_QSTR(MP_QSTR_print_exception), MP_ROM_PTR(&traceback_print_exception_obj) },
};
STATIC MP_DEFINE_CONST_DICT(traceback_module_globals, traceback_module_globals_table);

const mp_obj_module_t traceback_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&traceback_module_globals,
};
