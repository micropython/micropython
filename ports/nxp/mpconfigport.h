/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2019 Damien P. George
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

#include <stdint.h>
#include "mpconfigboard.h"
// options to control how MicroPython is built

// You can disable the built-in MicroPython compiler by setting the following
// config option to 0.  If you do this then you won't get a REPL prompt, but you
// will still be able to execute pre-compiled scripts, compiled with mpy-cross.
#define MICROPY_ENABLE_COMPILER     (1)

// #define MICROPY_QSTR_BYTES_IN_HASH  (1)
//#define MICROPY_QSTR_EXTRA_POOL     mp_qstr_frozen_const_pool
#define MICROPY_ALLOC_PATH_MAX      (128)
#define MICROPY_ALLOC_PARSE_CHUNK_INIT (16)
#define MICROPY_EMIT_X64            (0)
#define MICROPY_EMIT_THUMB          (0)
#define MICROPY_EMIT_INLINE_THUMB   (0)
#define MICROPY_COMP_MODULE_CONST   (0)
#define MICROPY_COMP_CONST          (0)
#define MICROPY_COMP_DOUBLE_TUPLE_ASSIGN (0)
#define MICROPY_COMP_TRIPLE_TUPLE_ASSIGN (0)
#define MICROPY_MEM_STATS           (0)
#define MICROPY_DEBUG_PRINTERS      (0)
#define MICROPY_ENABLE_GC           (1)
#define MICROPY_GC_ALLOC_THRESHOLD  (0)
#define MICROPY_ENABLE_FINALISER    (1)
#define MICROPY_HELPER_REPL         (1)
#define MICROPY_HELPER_LEXER_UNIX   (0)
#define MICROPY_ENABLE_SOURCE_LINE  (0)
#define MICROPY_ENABLE_DOC_STRING   (0)
#define MICROPY_ERROR_REPORTING     (MICROPY_ERROR_REPORTING_TERSE)
#define MICROPY_BUILTIN_METHOD_CHECK_SELF_ARG (0)
#define MICROPY_PY_ASYNC_AWAIT      (0)
#define MICROPY_PY_BUILTINS_BYTEARRAY (1)
#define MICROPY_PY_BUILTINS_DICT_FROMKEYS (0)
#define MICROPY_PY_BUILTINS_MEMORYVIEW (0)
#define MICROPY_PY_BUILTINS_ENUMERATE (0)
#define MICROPY_PY_BUILTINS_FILTER  (0)
#define MICROPY_PY_BUILTINS_FROZENSET (0)
#define MICROPY_PY_BUILTINS_REVERSED (0)
#define MICROPY_PY_BUILTINS_SET     (0)
#define MICROPY_PY_BUILTINS_SLICE   (0)
#define MICROPY_PY_BUILTINS_PROPERTY (0)
#define MICROPY_PY_BUILTINS_MIN_MAX (0)
#define MICROPY_PY_BUILTINS_STR_COUNT (0)
#define MICROPY_PY_BUILTINS_STR_OP_MODULO (0)
#define MICROPY_PY___FILE__         (0)
#define MICROPY_PY_GC               (1)
#define MICROPY_PY_ARRAY            (1)
#define MICROPY_PY_ATTRTUPLE        (0)
#define MICROPY_PY_COLLECTIONS      (0)
#define MICROPY_PY_MATH             (0)

/* module cmath */
#define MICROPY_PY_CMATH            (1)
#define MICROPY_FLOAT_IMPL          (MICROPY_FLOAT_IMPL_FLOAT)

#define MICROPY_MODULE_FROZEN_MPY   (0)
#define MICROPY_CPYTHON_COMPAT      (0)
#define MICROPY_LONGINT_IMPL        (MICROPY_LONGINT_IMPL_NONE)

#define MICROPY_PY_BUILTINS_HELP    (1)

#define MICROPY_ENABLE_SCHEDULER    (1)

/* VFS */
#define MICROPY_READER_VFS          (1)
#define MICROPY_VFS                 (1)

#ifndef MICROPY_VFS_FAT
#define MICROPY_VFS_FAT             (1)
#endif 

/* ubinascii module */
#define MICROPY_PY_UBINASCII        (1)

/* json module */
#define MICROPY_PY_UJSON            (1)

/* re module */
#define MICROPY_PY_URE              (1)


#define MICROPY_FATFS_ENABLE_LFN    (1)
#define MICROPY_FATFS_USE_LABEL     (1)
#define MICROPY_FATFS_RPATH         (2)
#define MICROPY_FATFS_MULTI_PARTITION   (1)

/* IO */
#define MICROPY_PY_IO               (1)
#define MICROPY_PY_IO_IOBASE        (1)
#define MICROPY_PY_IO_FILEIO        (MICROPY_VFS_FAT)
#define MICROPY_PY_SYS_STDFILES     (0)

#define mp_type_fileio        mp_type_vfs_fat_fileio
#define mp_type_textio        mp_type_vfs_fat_textio

#define MICROPY_STACK_CHECK         (1)

/* little VGL */
#if defined(MICROPY_PY_LVGL) && MICROPY_PY_LVGL
#include "lib/nxplib/middleware/littlevgl/lv_misc/lv_gc.h"
#define MICROPY_PY_LVGL_DEF \
    { MP_OBJ_NEW_QSTR(MP_QSTR_lvgl), (mp_obj_t)&mp_module_lvgl }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_SDL), (mp_obj_t)&mp_module_SDL }, 
#else 
#define LV_ROOTS
#define MICROPY_PY_LVGL_DEF
#endif


/* IO */
#define MICROPY_PY_IO               (1)
#define MICROPY_PY_IO_IOBASE        (1) 

#define MICROPY_GC_STACK_ENTRY_TYPE uint16_t
// type definitions for the specific machine

#define MICROPY_MAKE_POINTER_CALLABLE(p) ((void*)((mp_uint_t)(p) | 1))

// This port is intended to be 32-bit, but unfortunately, int32_t for
// different targets may be defined in different ways - either as int
// or as long. This requires different printf formatting specifiers
// to print such value. So, we avoid int32_t and use int directly.
#define UINT_FMT "%u"
#define INT_FMT "%d"
typedef int mp_int_t; // must be pointer size
typedef unsigned mp_uint_t; // must be pointer size

typedef long mp_off_t;

#define MP_PLAT_PRINT_STRN(str, len) mp_hal_stdout_tx_strn_cooked(str, len)

extern const struct _mp_obj_module_t pyb_module;
#if defined(MICROPY_PY_LVGL) && MICROPY_PY_LVGL
extern const struct _mp_obj_module_t mp_module_lvgl;
extern const struct _mp_obj_module_t mp_module_SDL;
#endif

extern const struct _mp_obj_module_t mp_module_uos;
// extra built in names to add to the global namespace
#define MICROPY_PORT_BUILTINS \
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&mp_builtin_open_obj) }, 

#define MICROPY_PORT_BUILTIN_MODULES \
    { MP_ROM_QSTR(MP_QSTR_pyb), MP_ROM_PTR(&pyb_module) }, \
    { MP_ROM_QSTR(MP_QSTR_uos), MP_ROM_PTR(&mp_module_uos) }, \
    MICROPY_PY_LVGL_DEF \

#define mp_import_stat mp_vfs_import_stat


// extra constants

#define MICROPY_PORT_CONSTANTS \
    { MP_ROM_QSTR(MP_QSTR_pyb), MP_ROM_PTR(&pyb_module) }, \

// We need to provide a declaration/definition of alloca()
#include <alloca.h>

#ifdef __thumb__
#define MICROPY_MIN_USE_CORTEX_CPU (1)
#endif

#define MP_STATE_PORT MP_STATE_VM

#define MICROPY_PORT_ROOT_POINTERS \
    LV_ROOTS \
    const char *readline_hist[8]; \
    void *mmap_region_head; \
    mp_obj_t pyb_config_main; \
    mp_obj_t pin_class_mapper; \
    mp_obj_t pin_class_map_dict; \
    mp_obj_t pyb_i2c_callback[MP_FEATURE_I2C_COUNTS]; \
    mp_obj_t pyb_gpt_callback[3]; \
    mp_obj_t pyb_spi_callback[MP_FEATURE_SPI_COUNTS]; \
    mp_obj_t pyb_usart_callback[MP_FEATURE_USART_COUNTS]; \

