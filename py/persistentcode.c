/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2020 Damien P. George
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
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "py/reader.h"
#include "py/nativeglue.h"
#include "py/persistentcode.h"
#include "py/bc0.h"
#include "py/objstr.h"
#include "py/mpthread.h"

#if MICROPY_PERSISTENT_CODE_LOAD || MICROPY_PERSISTENT_CODE_SAVE

#include "py/smallint.h"

// makeqstrdata.py has a fixed list of qstrs at the start that we can assume
// are available with know indices on all MicroPython implementations, and
// avoid needing to duplicate the string data in the .mpy file. This is the
// last one in that list (anything with a qstr less than or equal to this is
// assumed to be in the list).
#define QSTR_LAST_STATIC MP_QSTR_zip

#if MICROPY_DYNAMIC_COMPILER
#define MPY_FEATURE_ARCH_DYNAMIC mp_dynamic_compiler.native_arch
#else
#define MPY_FEATURE_ARCH_DYNAMIC MPY_FEATURE_ARCH
#endif

typedef struct _bytecode_prelude_t {
    uint n_state;
    uint n_exc_stack;
    uint scope_flags;
    uint n_pos_args;
    uint n_kwonly_args;
    uint n_def_pos_args;
    uint code_info_size;
} bytecode_prelude_t;

#endif // MICROPY_PERSISTENT_CODE_LOAD || MICROPY_PERSISTENT_CODE_SAVE

#if MICROPY_PERSISTENT_CODE_LOAD

#include "py/parsenum.h"

static int read_byte(mp_reader_t *reader);
static size_t read_uint(mp_reader_t *reader);

#if MICROPY_EMIT_MACHINE_CODE

typedef struct _reloc_info_t {
    mp_reader_t *reader;
    mp_module_context_t *context;
    uint8_t *rodata;
    uint8_t *bss;
} reloc_info_t;

void mp_native_relocate(void *ri_in, uint8_t *text, uintptr_t reloc_text) {
    // Relocate native code
    reloc_info_t *ri = ri_in;
    uint8_t op;
    uintptr_t *addr_to_adjust = NULL;
    while ((op = read_byte(ri->reader)) != 0xff) {
        if (op & 1) {
            // Point to new location to make adjustments
            size_t addr = read_uint(ri->reader);
            if ((addr & 1) == 0) {
                // Point to somewhere in text
                addr_to_adjust = &((uintptr_t *)text)[addr >> 1];
            } else {
                // Point to somewhere in rodata
                addr_to_adjust = &((uintptr_t *)ri->rodata)[addr >> 1];
            }
        }
        op >>= 1;
        uintptr_t dest;
        size_t n = 1;
        if (op <= 5) {
            if (op & 1) {
                // Read in number of adjustments to make
                n = read_uint(ri->reader);
            }
            op >>= 1;
            if (op == 0) {
                // Destination is text
                dest = reloc_text;
            } else if (op == 1) {
                // Destination is rodata
                dest = (uintptr_t)ri->rodata;
            } else {
                // Destination is bss
                dest = (uintptr_t)ri->bss;
            }
        } else if (op == 6) {
            // Destination is qstr_table
            dest = (uintptr_t)ri->context->constants.qstr_table;
        } else if (op == 7) {
            // Destination is obj_table
            dest = (uintptr_t)ri->context->constants.obj_table;
        } else if (op == 8) {
            // Destination is mp_fun_table itself
            dest = (uintptr_t)&mp_fun_table;
        } else {
            // Destination is an entry in mp_fun_table
            dest = ((uintptr_t *)&mp_fun_table)[op - 9];
        }
        while (n--) {
            *addr_to_adjust++ += dest;
        }
    }
}

#endif

static int read_byte(mp_reader_t *reader) {
    return reader->readbyte(reader->data);
}

static void read_bytes(mp_reader_t *reader, byte *buf, size_t len) {
    while (len-- > 0) {
        *buf++ = reader->readbyte(reader->data);
    }
}

static size_t read_uint(mp_reader_t *reader) {
    size_t unum = 0;
    for (;;) {
        byte b = reader->readbyte(reader->data);
        unum = (unum << 7) | (b & 0x7f);
        if ((b & 0x80) == 0) {
            break;
        }
    }
    return unum;
}

static qstr load_qstr(mp_reader_t *reader) {
    size_t len = read_uint(reader);
    if (len & 1) {
        // static qstr
        return len >> 1;
    }
    len >>= 1;
    char *str = m_new(char, len);
    read_bytes(reader, (byte *)str, len);
    read_byte(reader); // read and discard null terminator
    qstr qst = qstr_from_strn(str, len);
    m_del(char, str, len);
    return qst;
}

static mp_obj_t load_obj(mp_reader_t *reader) {
    byte obj_type = read_byte(reader);
    #if MICROPY_EMIT_MACHINE_CODE
    if (obj_type == MP_PERSISTENT_OBJ_FUN_TABLE) {
        return MP_OBJ_FROM_PTR(&mp_fun_table);
    } else
    #endif
    if (obj_type == MP_PERSISTENT_OBJ_NONE) {
        return mp_const_none;
    } else if (obj_type == MP_PERSISTENT_OBJ_FALSE) {
        return mp_const_false;
    } else if (obj_type == MP_PERSISTENT_OBJ_TRUE) {
        return mp_const_true;
    } else if (obj_type == MP_PERSISTENT_OBJ_ELLIPSIS) {
        return MP_OBJ_FROM_PTR(&mp_const_ellipsis_obj);
    } else {
        size_t len = read_uint(reader);
        if (len == 0 && obj_type == MP_PERSISTENT_OBJ_BYTES) {
            read_byte(reader); // skip null terminator
            return mp_const_empty_bytes;
        } else if (obj_type == MP_PERSISTENT_OBJ_TUPLE) {
            mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR(mp_obj_new_tuple(len, NULL));
            for (size_t i = 0; i < len; ++i) {
                tuple->items[i] = load_obj(reader);
            }
            return MP_OBJ_FROM_PTR(tuple);
        }
        vstr_t vstr;
        vstr_init_len(&vstr, len);
        read_bytes(reader, (byte *)vstr.buf, len);
        if (obj_type == MP_PERSISTENT_OBJ_STR || obj_type == MP_PERSISTENT_OBJ_BYTES) {
            read_byte(reader); // skip null terminator
            if (obj_type == MP_PERSISTENT_OBJ_STR) {
                return mp_obj_new_str_from_utf8_vstr(&vstr);
            } else {
                return mp_obj_new_bytes_from_vstr(&vstr);
            }
        } else if (obj_type == MP_PERSISTENT_OBJ_INT) {
            return mp_parse_num_integer(vstr.buf, vstr.len, 10, NULL);
        } else {
            assert(obj_type == MP_PERSISTENT_OBJ_FLOAT || obj_type == MP_PERSISTENT_OBJ_COMPLEX);
            return mp_parse_num_float(vstr.buf, vstr.len, obj_type == MP_PERSISTENT_OBJ_COMPLEX, NULL);
        }
    }
}

static mp_raw_code_t *load_raw_code(mp_reader_t *reader, mp_module_context_t *context) {
    // Load function kind and data length
    size_t kind_len = read_uint(reader);
    int kind = (kind_len & 3) + MP_CODE_BYTECODE;
    bool has_children = !!(kind_len & 4);
    size_t fun_data_len = kind_len >> 3;

    #if !MICROPY_EMIT_MACHINE_CODE
    if (kind != MP_CODE_BYTECODE) {
        mp_raise_ValueError(MP_ERROR_TEXT("incompatible .mpy file"));
    }
    #endif

    uint8_t *fun_data = NULL;
    #if MICROPY_EMIT_MACHINE_CODE
    size_t prelude_offset = 0;
    mp_uint_t native_scope_flags = 0;
    mp_uint_t native_n_pos_args = 0;
    mp_uint_t native_type_sig = 0;
    #endif

    if (kind == MP_CODE_BYTECODE) {
        // Allocate memory for the bytecode
        fun_data = m_new(uint8_t, fun_data_len);
        // Load bytecode
        read_bytes(reader, fun_data, fun_data_len);

    #if MICROPY_EMIT_MACHINE_CODE
    } else {
        // Allocate memory for native data and load it
        size_t fun_alloc;
        MP_PLAT_ALLOC_EXEC(fun_data_len, (void **)&fun_data, &fun_alloc);
        read_bytes(reader, fun_data, fun_data_len);

        if (kind == MP_CODE_NATIVE_PY) {
            // Read prelude offset within fun_data, and extract scope flags.
            prelude_offset = read_uint(reader);
            const byte *ip = fun_data + prelude_offset;
            MP_BC_PRELUDE_SIG_DECODE(ip);
            native_scope_flags = scope_flags;
        } else {
            // Load basic scope info for viper and asm.
            native_scope_flags = read_uint(reader);
            if (kind == MP_CODE_NATIVE_ASM) {
                native_n_pos_args = read_uint(reader);
                native_type_sig = read_uint(reader);
            }
        }
    #endif
    }

    size_t n_children = 0;
    mp_raw_code_t **children = NULL;

    #if MICROPY_EMIT_MACHINE_CODE
    // Load optional BSS/rodata for viper.
    uint8_t *rodata = NULL;
    uint8_t *bss = NULL;
    if (kind == MP_CODE_NATIVE_VIPER) {
        size_t rodata_size = 0;
        if (native_scope_flags & MP_SCOPE_FLAG_VIPERRODATA) {
            rodata_size = read_uint(reader);
        }

        size_t bss_size = 0;
        if (native_scope_flags & MP_SCOPE_FLAG_VIPERBSS) {
            bss_size = read_uint(reader);
        }

        if (rodata_size + bss_size != 0) {
            bss_size = (uintptr_t)MP_ALIGN(bss_size, sizeof(uintptr_t));
            uint8_t *data = m_new0(uint8_t, bss_size + rodata_size);
            bss = data;
            rodata = bss + bss_size;
            if (native_scope_flags & MP_SCOPE_FLAG_VIPERRODATA) {
                read_bytes(reader, rodata, rodata_size);
            }

            // Viper code with BSS/rodata should not have any children.
            // Reuse the children pointer to reference the BSS/rodata
            // memory so that it is not reclaimed by the GC.
            assert(!has_children);
            children = (void *)data;
        }
    }
    #endif

    // Load children if any.
    if (has_children) {
        n_children = read_uint(reader);
        children = m_new(mp_raw_code_t *, n_children + (kind == MP_CODE_NATIVE_PY));
        for (size_t i = 0; i < n_children; ++i) {
            children[i] = load_raw_code(reader, context);
        }
    }

    // Create raw_code and return it
    mp_raw_code_t *rc = mp_emit_glue_new_raw_code();
    if (kind == MP_CODE_BYTECODE) {
        const byte *ip = fun_data;
        MP_BC_PRELUDE_SIG_DECODE(ip);
        // Assign bytecode to raw code object
        mp_emit_glue_assign_bytecode(rc, fun_data,
            children,
            #if MICROPY_PERSISTENT_CODE_SAVE
            fun_data_len,
            n_children,
            #endif
            scope_flags);

    #if MICROPY_EMIT_MACHINE_CODE
    } else {
        const uint8_t *prelude_ptr;
        #if MICROPY_EMIT_NATIVE_PRELUDE_SEPARATE_FROM_MACHINE_CODE
        if (kind == MP_CODE_NATIVE_PY) {
            // Executable code cannot be accessed byte-wise on this architecture, so copy
            // the prelude to a separate memory region that is byte-wise readable.
            void *buf = fun_data + prelude_offset;
            size_t n = fun_data_len - prelude_offset;
            prelude_ptr = memcpy(m_new(uint8_t, n), buf, n);
        }
        #endif

        // Relocate and commit code to executable address space
        reloc_info_t ri = {reader, context, rodata, bss};
        #if defined(MP_PLAT_COMMIT_EXEC)
        void *opt_ri = (native_scope_flags & MP_SCOPE_FLAG_VIPERRELOC) ? &ri : NULL;
        fun_data = MP_PLAT_COMMIT_EXEC(fun_data, fun_data_len, opt_ri);
        #else
        if (native_scope_flags & MP_SCOPE_FLAG_VIPERRELOC) {
            #if MICROPY_PERSISTENT_CODE_TRACK_RELOC_CODE
            // If native code needs relocations then it's not guaranteed that a pointer to
            // the head of `buf` (containing the machine code) will be retained for the GC
            // to trace.  This is because native functions can start inside `buf` and so
            // it's possible that the only GC-reachable pointers are pointers inside `buf`.
            // So put this `buf` on a list of reachable root pointers.
            if (MP_STATE_PORT(track_reloc_code_list) == MP_OBJ_NULL) {
                MP_STATE_PORT(track_reloc_code_list) = mp_obj_new_list(0, NULL);
            }
            mp_obj_list_append(MP_STATE_PORT(track_reloc_code_list), MP_OBJ_FROM_PTR(fun_data));
            #endif
            // Do the relocations.
            mp_native_relocate(&ri, fun_data, (uintptr_t)fun_data);
        }
        #endif

        if (kind == MP_CODE_NATIVE_PY) {
            #if !MICROPY_EMIT_NATIVE_PRELUDE_SEPARATE_FROM_MACHINE_CODE
            prelude_ptr = fun_data + prelude_offset;
            #endif
            if (n_children == 0) {
                children = (void *)prelude_ptr;
            } else {
                children[n_children] = (void *)prelude_ptr;
            }
        }

        // Assign native code to raw code object
        mp_emit_glue_assign_native(rc, kind,
            fun_data, fun_data_len,
            children,
            #if MICROPY_PERSISTENT_CODE_SAVE
            n_children,
            prelude_offset,
            #endif
            native_scope_flags, native_n_pos_args, native_type_sig
            );
    #endif
    }
    return rc;
}

void mp_raw_code_load(mp_reader_t *reader, mp_compiled_module_t *cm) {
    // Set exception handler to close the reader if an exception is raised.
    MP_DEFINE_NLR_JUMP_CALLBACK_FUNCTION_1(ctx, reader->close, reader->data);
    nlr_push_jump_callback(&ctx.callback, mp_call_function_1_from_nlr_jump_callback);

    byte header[4];
    read_bytes(reader, header, sizeof(header));
    byte arch = MPY_FEATURE_DECODE_ARCH(header[2]);
    if (header[0] != 'M'
        || header[1] != MPY_VERSION
        || (arch != MP_NATIVE_ARCH_NONE && MPY_FEATURE_DECODE_SUB_VERSION(header[2]) != MPY_SUB_VERSION)
        || header[3] > MP_SMALL_INT_BITS) {
        mp_raise_ValueError(MP_ERROR_TEXT("incompatible .mpy file"));
    }
    if (MPY_FEATURE_DECODE_ARCH(header[2]) != MP_NATIVE_ARCH_NONE) {
        if (!MPY_FEATURE_ARCH_TEST(arch)) {
            if (MPY_FEATURE_ARCH_TEST(MP_NATIVE_ARCH_NONE)) {
                // On supported ports this can be resolved by enabling feature, eg
                // mpconfigboard.h: MICROPY_EMIT_THUMB (1)
                mp_raise_ValueError(MP_ERROR_TEXT("native code in .mpy unsupported"));
            } else {
                mp_raise_ValueError(MP_ERROR_TEXT("incompatible .mpy arch"));
            }
        }
    }

    size_t n_qstr = read_uint(reader);
    size_t n_obj = read_uint(reader);
    mp_module_context_alloc_tables(cm->context, n_qstr, n_obj);

    // Load qstrs.
    for (size_t i = 0; i < n_qstr; ++i) {
        cm->context->constants.qstr_table[i] = load_qstr(reader);
    }

    // Load constant objects.
    for (size_t i = 0; i < n_obj; ++i) {
        cm->context->constants.obj_table[i] = load_obj(reader);
    }

    // Load top-level module.
    cm->rc = load_raw_code(reader, cm->context);

    #if MICROPY_PERSISTENT_CODE_SAVE
    cm->has_native = MPY_FEATURE_DECODE_ARCH(header[2]) != MP_NATIVE_ARCH_NONE;
    cm->n_qstr = n_qstr;
    cm->n_obj = n_obj;
    #endif

    // Deregister exception handler and close the reader.
    nlr_pop_jump_callback(true);
}

void mp_raw_code_load_mem(const byte *buf, size_t len, mp_compiled_module_t *context) {
    mp_reader_t reader;
    mp_reader_new_mem(&reader, buf, len, 0);
    mp_raw_code_load(&reader, context);
}

#if MICROPY_HAS_FILE_READER

void mp_raw_code_load_file(qstr filename, mp_compiled_module_t *context) {
    mp_reader_t reader;
    mp_reader_new_file(&reader, filename);
    mp_raw_code_load(&reader, context);
}

#endif // MICROPY_HAS_FILE_READER

#endif // MICROPY_PERSISTENT_CODE_LOAD

#if MICROPY_PERSISTENT_CODE_SAVE

#include "py/objstr.h"

static void mp_print_bytes(mp_print_t *print, const byte *data, size_t len) {
    print->print_strn(print->data, (const char *)data, len);
}

#define BYTES_FOR_INT ((MP_BYTES_PER_OBJ_WORD * 8 + 6) / 7)
static void mp_print_uint(mp_print_t *print, size_t n) {
    byte buf[BYTES_FOR_INT];
    byte *p = buf + sizeof(buf);
    *--p = n & 0x7f;
    n >>= 7;
    for (; n != 0; n >>= 7) {
        *--p = 0x80 | (n & 0x7f);
    }
    print->print_strn(print->data, (char *)p, buf + sizeof(buf) - p);
}

static void save_qstr(mp_print_t *print, qstr qst) {
    if (qst <= QSTR_LAST_STATIC) {
        // encode static qstr
        mp_print_uint(print, qst << 1 | 1);
        return;
    }
    size_t len;
    const byte *str = qstr_data(qst, &len);
    mp_print_uint(print, len << 1);
    mp_print_bytes(print, str, len + 1); // +1 to store null terminator
}

static void save_obj(mp_print_t *print, mp_obj_t o) {
    #if MICROPY_EMIT_MACHINE_CODE
    if (o == MP_OBJ_FROM_PTR(&mp_fun_table)) {
        byte obj_type = MP_PERSISTENT_OBJ_FUN_TABLE;
        mp_print_bytes(print, &obj_type, 1);
    } else
    #endif
    if (mp_obj_is_str_or_bytes(o)) {
        byte obj_type;
        if (mp_obj_is_str(o)) {
            obj_type = MP_PERSISTENT_OBJ_STR;
        } else {
            obj_type = MP_PERSISTENT_OBJ_BYTES;
        }
        size_t len;
        const char *str = mp_obj_str_get_data(o, &len);
        mp_print_bytes(print, &obj_type, 1);
        mp_print_uint(print, len);
        mp_print_bytes(print, (const byte *)str, len + 1); // +1 to store null terminator
    } else if (o == mp_const_none) {
        byte obj_type = MP_PERSISTENT_OBJ_NONE;
        mp_print_bytes(print, &obj_type, 1);
    } else if (o == mp_const_false) {
        byte obj_type = MP_PERSISTENT_OBJ_FALSE;
        mp_print_bytes(print, &obj_type, 1);
    } else if (o == mp_const_true) {
        byte obj_type = MP_PERSISTENT_OBJ_TRUE;
        mp_print_bytes(print, &obj_type, 1);
    } else if (MP_OBJ_TO_PTR(o) == &mp_const_ellipsis_obj) {
        byte obj_type = MP_PERSISTENT_OBJ_ELLIPSIS;
        mp_print_bytes(print, &obj_type, 1);
    } else if (mp_obj_is_type(o, &mp_type_tuple)) {
        size_t len;
        mp_obj_t *items;
        mp_obj_tuple_get(o, &len, &items);
        byte obj_type = MP_PERSISTENT_OBJ_TUPLE;
        mp_print_bytes(print, &obj_type, 1);
        mp_print_uint(print, len);
        for (size_t i = 0; i < len; ++i) {
            save_obj(print, items[i]);
        }
    } else {
        // we save numbers using a simplistic text representation
        // TODO could be improved
        byte obj_type;
        if (mp_obj_is_int(o)) {
            obj_type = MP_PERSISTENT_OBJ_INT;
        #if MICROPY_PY_BUILTINS_COMPLEX
        } else if (mp_obj_is_type(o, &mp_type_complex)) {
            obj_type = MP_PERSISTENT_OBJ_COMPLEX;
        #endif
        } else {
            assert(mp_obj_is_float(o));
            obj_type = MP_PERSISTENT_OBJ_FLOAT;
        }
        vstr_t vstr;
        mp_print_t pr;
        vstr_init_print(&vstr, 10, &pr);
        mp_obj_print_helper(&pr, o, PRINT_REPR);
        mp_print_bytes(print, &obj_type, 1);
        mp_print_uint(print, vstr.len);
        mp_print_bytes(print, (const byte *)vstr.buf, vstr.len);
        vstr_clear(&vstr);
    }
}

static void save_raw_code(mp_print_t *print, const mp_raw_code_t *rc) {
    // Save function kind and data length
    mp_print_uint(print, (rc->fun_data_len << 3) | ((rc->n_children != 0) << 2) | (rc->kind - MP_CODE_BYTECODE));

    // Save function code.
    mp_print_bytes(print, rc->fun_data, rc->fun_data_len);

    #if MICROPY_EMIT_MACHINE_CODE
    if (rc->kind == MP_CODE_NATIVE_PY) {
        // Save prelude size
        mp_print_uint(print, rc->prelude_offset);
    } else if (rc->kind == MP_CODE_NATIVE_VIPER || rc->kind == MP_CODE_NATIVE_ASM) {
        // Save basic scope info for viper and asm
        // Viper/asm functions don't support generator, variable args, or default keyword args
        // so (scope_flags & MP_SCOPE_FLAG_ALL_SIG) for these functions is always 0.
        mp_print_uint(print, 0);
        #if MICROPY_EMIT_INLINE_ASM
        if (rc->kind == MP_CODE_NATIVE_ASM) {
            mp_print_uint(print, rc->asm_n_pos_args);
            mp_print_uint(print, rc->asm_type_sig);
        }
        #endif
    }
    #endif

    if (rc->n_children) {
        mp_print_uint(print, rc->n_children);
        for (size_t i = 0; i < rc->n_children; ++i) {
            save_raw_code(print, rc->children[i]);
        }
    }
}

void mp_raw_code_save(mp_compiled_module_t *cm, mp_print_t *print) {
    // header contains:
    //  byte  'M'
    //  byte  version
    //  byte  native arch (and sub-version if native)
    //  byte  number of bits in a small int
    byte header[4] = {
        'M',
        MPY_VERSION,
        cm->has_native ? MPY_FEATURE_ENCODE_SUB_VERSION(MPY_SUB_VERSION) | MPY_FEATURE_ENCODE_ARCH(MPY_FEATURE_ARCH_DYNAMIC) : 0,
        #if MICROPY_DYNAMIC_COMPILER
        mp_dynamic_compiler.small_int_bits,
        #else
        MP_SMALL_INT_BITS,
        #endif
    };
    mp_print_bytes(print, header, sizeof(header));

    // Number of entries in constant table.
    mp_print_uint(print, cm->n_qstr);
    mp_print_uint(print, cm->n_obj);

    // Save qstrs.
    for (size_t i = 0; i < cm->n_qstr; ++i) {
        save_qstr(print, cm->context->constants.qstr_table[i]);
    }

    // Save constant objects.
    for (size_t i = 0; i < cm->n_obj; ++i) {
        save_obj(print, (mp_obj_t)cm->context->constants.obj_table[i]);
    }

    // Save outer raw code, which will save all its child raw codes.
    save_raw_code(print, cm->rc);
}

#if MICROPY_PERSISTENT_CODE_SAVE_FILE

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

static void fd_print_strn(void *env, const char *str, size_t len) {
    int fd = (intptr_t)env;
    MP_THREAD_GIL_EXIT();
    ssize_t ret = write(fd, str, len);
    MP_THREAD_GIL_ENTER();
    (void)ret;
}

void mp_raw_code_save_file(mp_compiled_module_t *cm, qstr filename) {
    MP_THREAD_GIL_EXIT();
    int fd = open(qstr_str(filename), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    MP_THREAD_GIL_ENTER();
    if (fd < 0) {
        mp_raise_OSError_with_filename(errno, qstr_str(filename));
    }
    mp_print_t fd_print = {(void *)(intptr_t)fd, fd_print_strn};
    mp_raw_code_save(cm, &fd_print);
    MP_THREAD_GIL_EXIT();
    close(fd);
    MP_THREAD_GIL_ENTER();
}

#endif // MICROPY_PERSISTENT_CODE_SAVE_FILE

#endif // MICROPY_PERSISTENT_CODE_SAVE

#if MICROPY_PERSISTENT_CODE_TRACK_RELOC_CODE
// An mp_obj_list_t that tracks relocated native code to prevent the GC from reclaiming them.
MP_REGISTER_ROOT_POINTER(mp_obj_t track_reloc_code_list);
#endif
