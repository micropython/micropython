/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2016 Damien P. George
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
#include "py/emitglue.h"
#include "py/persistentcode.h"
#include "py/bc.h"

#if MICROPY_PERSISTENT_CODE_LOAD || MICROPY_PERSISTENT_CODE_SAVE

#include "py/smallint.h"

#define QSTR_LAST_STATIC MP_QSTR_zip

// The current version of .mpy files
#define MPY_VERSION (4)

// Macros to encode/decode flags to/from the feature byte
#define MPY_FEATURE_ENCODE_FLAGS(flags) (flags)
#define MPY_FEATURE_DECODE_FLAGS(feat) ((feat) & 3)

// Macros to encode/decode native architecture to/from the feature byte
#define MPY_FEATURE_ENCODE_ARCH(arch) ((arch) << 2)
#define MPY_FEATURE_DECODE_ARCH(feat) ((feat) >> 2)

// The feature flag bits encode the compile-time config options that
// affect the generate bytecode.
#define MPY_FEATURE_FLAGS ( \
    ((MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE) << 0) \
    | ((MICROPY_PY_BUILTINS_STR_UNICODE) << 1) \
    )
// This is a version of the flags that can be configured at runtime.
#define MPY_FEATURE_FLAGS_DYNAMIC ( \
    ((MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE_DYNAMIC) << 0) \
    | ((MICROPY_PY_BUILTINS_STR_UNICODE_DYNAMIC) << 1) \
    )

// Define the host architecture
#if MICROPY_EMIT_X86
#define MPY_FEATURE_ARCH (MP_NATIVE_ARCH_X86)
#elif MICROPY_EMIT_X64
#define MPY_FEATURE_ARCH (MP_NATIVE_ARCH_X64)
#elif MICROPY_EMIT_THUMB
#define MPY_FEATURE_ARCH (MP_NATIVE_ARCH_ARMV7M)
#elif MICROPY_EMIT_ARM
#define MPY_FEATURE_ARCH (MP_NATIVE_ARCH_ARMV6)
#elif MICROPY_EMIT_XTENSA
#define MPY_FEATURE_ARCH (MP_NATIVE_ARCH_XTENSA)
#else
#define MPY_FEATURE_ARCH (MP_NATIVE_ARCH_NONE)
#endif

#if MICROPY_PERSISTENT_CODE_LOAD || (MICROPY_PERSISTENT_CODE_SAVE && !MICROPY_DYNAMIC_COMPILER)
// The bytecode will depend on the number of bits in a small-int, and
// this function computes that (could make it a fixed constant, but it
// would need to be defined in mpconfigport.h).
STATIC int mp_small_int_bits(void) {
    mp_int_t i = MP_SMALL_INT_MAX;
    int n = 1;
    while (i != 0) {
        i >>= 1;
        ++n;
    }
    return n;
}
#endif

#define QSTR_WINDOW_SIZE (32)

typedef struct _qstr_window_t {
    uint16_t idx; // indexes the head of the window
    uint16_t window[QSTR_WINDOW_SIZE];
} qstr_window_t;

// Push a qstr to the head of the window, and the tail qstr is overwritten
STATIC void qstr_window_push(qstr_window_t *qw, qstr qst) {
    qw->idx = (qw->idx + 1) % QSTR_WINDOW_SIZE;
    qw->window[qw->idx] = qst;
}

// Pull an existing qstr from within the window to the head of the window
STATIC qstr qstr_window_pull(qstr_window_t *qw, size_t idx) {
    qstr qst = qw->window[idx];
    if (idx > qw->idx) {
        memmove(&qw->window[idx], &qw->window[idx + 1], (QSTR_WINDOW_SIZE - idx - 1) * sizeof(uint16_t));
        qw->window[QSTR_WINDOW_SIZE - 1] = qw->window[0];
        idx = 0;
    }
    memmove(&qw->window[idx], &qw->window[idx + 1], (qw->idx - idx) * sizeof(uint16_t));
    qw->window[qw->idx] = qst;
    return qst;
}

#if MICROPY_PERSISTENT_CODE_LOAD

// Access a qstr at the given index, relative to the head of the window (0=head)
STATIC qstr qstr_window_access(qstr_window_t *qw, size_t idx) {
    return qstr_window_pull(qw, (qw->idx + QSTR_WINDOW_SIZE - idx) % QSTR_WINDOW_SIZE);
}

#endif

#if MICROPY_PERSISTENT_CODE_SAVE

// Insert a qstr at the head of the window, either by pulling an existing one or pushing a new one
STATIC size_t qstr_window_insert(qstr_window_t *qw, qstr qst) {
    for (size_t idx = 0; idx < QSTR_WINDOW_SIZE; ++idx) {
        if (qw->window[idx] == qst) {
            qstr_window_pull(qw, idx);
            return (qw->idx + QSTR_WINDOW_SIZE - idx) % QSTR_WINDOW_SIZE;
        }
    }
    qstr_window_push(qw, qst);
    return QSTR_WINDOW_SIZE;
}

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

#if MICROPY_PERSISTENT_CODE_SAVE || MICROPY_EMIT_NATIVE

// ip will point to start of opcodes
// ip2 will point to simple_name, source_file qstrs
STATIC void extract_prelude(const byte **ip, const byte **ip2, bytecode_prelude_t *prelude) {
    prelude->n_state = mp_decode_uint(ip);
    prelude->n_exc_stack = mp_decode_uint(ip);
    prelude->scope_flags = *(*ip)++;
    prelude->n_pos_args = *(*ip)++;
    prelude->n_kwonly_args = *(*ip)++;
    prelude->n_def_pos_args = *(*ip)++;
    *ip2 = *ip;
    prelude->code_info_size = mp_decode_uint(ip2);
    *ip += prelude->code_info_size;
    while (*(*ip)++ != 255) {
    }
}

#endif

#endif // MICROPY_PERSISTENT_CODE_LOAD || MICROPY_PERSISTENT_CODE_SAVE

#if MICROPY_PERSISTENT_CODE_LOAD

#include "py/parsenum.h"

#if MICROPY_EMIT_NATIVE

#if MICROPY_EMIT_THUMB
STATIC void asm_thumb_rewrite_mov(uint8_t *pc, uint16_t val) {
    // high part
    *(uint16_t*)pc = (*(uint16_t*)pc & 0xfbf0) | (val >> 1 & 0x0400) | (val >> 12);
    // low part
    *(uint16_t*)(pc + 2) = (*(uint16_t*)(pc + 2) & 0x0f00) | (val << 4 & 0x7000) | (val & 0x00ff);

}
#endif

STATIC void arch_link_qstr(uint8_t *pc, bool is_obj, qstr qst) {
    mp_uint_t val = qst;
    if (is_obj) {
        val = (mp_uint_t)MP_OBJ_NEW_QSTR(qst);
    }
    #if MICROPY_EMIT_X86 || MICROPY_EMIT_X64 || MICROPY_EMIT_ARM || MICROPY_EMIT_XTENSA
    pc[0] = val & 0xff;
    pc[1] = (val >> 8) & 0xff;
    pc[2] = (val >> 16) & 0xff;
    pc[3] = (val >> 24) & 0xff;
    #elif MICROPY_EMIT_THUMB
    if (is_obj) {
        // qstr object, movw and movt
        asm_thumb_rewrite_mov(pc, val); // movw
        asm_thumb_rewrite_mov(pc + 4, val >> 16); // movt
    } else {
        // qstr number, movw instruction
        asm_thumb_rewrite_mov(pc, val); // movw
    }
    #endif
}

#endif

STATIC int read_byte(mp_reader_t *reader) {
    return reader->readbyte(reader->data);
}

STATIC void read_bytes(mp_reader_t *reader, byte *buf, size_t len) {
    while (len-- > 0) {
        *buf++ = reader->readbyte(reader->data);
    }
}

STATIC size_t read_uint(mp_reader_t *reader, byte **out) {
    size_t unum = 0;
    for (;;) {
        byte b = reader->readbyte(reader->data);
        if (out != NULL) {
            **out = b;
            ++*out;
        }
        unum = (unum << 7) | (b & 0x7f);
        if ((b & 0x80) == 0) {
            break;
        }
    }
    return unum;
}

STATIC qstr load_qstr(mp_reader_t *reader, qstr_window_t *qw) {
    size_t len = read_uint(reader, NULL);
    if (len == 0) {
        // static qstr
        return read_byte(reader);
    }
    if (len & 1) {
        // qstr in window
        return qstr_window_access(qw, len >> 1);
    }
    len >>= 1;
    char *str = m_new(char, len);
    read_bytes(reader, (byte*)str, len);
    qstr qst = qstr_from_strn(str, len);
    m_del(char, str, len);
    qstr_window_push(qw, qst);
    return qst;
}

STATIC mp_obj_t load_obj(mp_reader_t *reader) {
    byte obj_type = read_byte(reader);
    if (obj_type == 'e') {
        return MP_OBJ_FROM_PTR(&mp_const_ellipsis_obj);
    } else {
        size_t len = read_uint(reader, NULL);
        vstr_t vstr;
        vstr_init_len(&vstr, len);
        read_bytes(reader, (byte*)vstr.buf, len);
        if (obj_type == 's' || obj_type == 'b') {
            return mp_obj_new_str_from_vstr(obj_type == 's' ? &mp_type_str : &mp_type_bytes, &vstr);
        } else if (obj_type == 'i') {
            return mp_parse_num_integer(vstr.buf, vstr.len, 10, NULL);
        } else {
            assert(obj_type == 'f' || obj_type == 'c');
            return mp_parse_num_decimal(vstr.buf, vstr.len, obj_type == 'c', false, NULL);
        }
    }
}

STATIC void load_prelude(mp_reader_t *reader, byte **ip, byte **ip2, bytecode_prelude_t *prelude) {
    prelude->n_state = read_uint(reader, ip);
    prelude->n_exc_stack = read_uint(reader, ip);
    read_bytes(reader, *ip, 4);
    prelude->scope_flags = *(*ip)++;
    prelude->n_pos_args = *(*ip)++;
    prelude->n_kwonly_args = *(*ip)++;
    prelude->n_def_pos_args = *(*ip)++;
    *ip2 = *ip;
    prelude->code_info_size = read_uint(reader, ip2);
    read_bytes(reader, *ip2, prelude->code_info_size - (*ip2 - *ip));
    *ip += prelude->code_info_size;
    while ((*(*ip)++ = read_byte(reader)) != 255) {
    }
}

STATIC void load_bytecode(mp_reader_t *reader, qstr_window_t *qw, byte *ip, byte *ip_top) {
    while (ip < ip_top) {
        *ip = read_byte(reader);
        size_t sz;
        uint f = mp_opcode_format(ip, &sz, false);
        ++ip;
        --sz;
        if (f == MP_OPCODE_QSTR) {
            qstr qst = load_qstr(reader, qw);
            *ip++ = qst;
            *ip++ = qst >> 8;
            sz -= 2;
        } else if (f == MP_OPCODE_VAR_UINT) {
            while ((*ip++ = read_byte(reader)) & 0x80) {
            }
        }
        read_bytes(reader, ip, sz);
        ip += sz;
    }
}

STATIC mp_raw_code_t *load_raw_code(mp_reader_t *reader, qstr_window_t *qw) {
    // Load function kind and data length
    size_t kind_len = read_uint(reader, NULL);
    int kind = (kind_len & 3) + MP_CODE_BYTECODE;
    size_t fun_data_len = kind_len >> 2;

    #if !MICROPY_EMIT_NATIVE
    if (kind != MP_CODE_BYTECODE) {
        mp_raise_ValueError("incompatible .mpy file");
    }
    #endif

    uint8_t *fun_data = NULL;
    byte *ip2;
    bytecode_prelude_t prelude = {0};
    #if MICROPY_EMIT_NATIVE
    size_t prelude_offset;
    mp_uint_t type_sig = 0;
    size_t n_qstr_link = 0;
    #endif

    if (kind == MP_CODE_BYTECODE) {
        // Allocate memory for the bytecode
        fun_data = m_new(uint8_t, fun_data_len);

        // Load prelude
        byte *ip = fun_data;
        load_prelude(reader, &ip, &ip2, &prelude);

        // Load bytecode
        load_bytecode(reader, qw, ip, fun_data + fun_data_len);

    #if MICROPY_EMIT_NATIVE
    } else {
        // Allocate memory for native data and load it
        size_t fun_alloc;
        MP_PLAT_ALLOC_EXEC(fun_data_len, (void**)&fun_data, &fun_alloc);
        read_bytes(reader, fun_data, fun_data_len);

        if (kind == MP_CODE_NATIVE_PY || kind == MP_CODE_NATIVE_VIPER) {
            // Parse qstr link table and link native code
            n_qstr_link = read_uint(reader, NULL);
            for (size_t i = 0; i < n_qstr_link; ++i) {
                size_t off = read_uint(reader, NULL);
                qstr qst = load_qstr(reader, qw);
                uint8_t *dest = fun_data + (off >> 2);
                if ((off & 3) == 0) {
                    // Generic 16-bit link
                    dest[0] = qst & 0xff;
                    dest[1] = (qst >> 8) & 0xff;
                } else {
                    // Architecture-specific link
                    arch_link_qstr(dest, (off & 3) == 2, qst);
                }
            }
        }

        if (kind == MP_CODE_NATIVE_PY) {
            // Extract prelude for later use
            prelude_offset = read_uint(reader, NULL);
            const byte *ip = fun_data + prelude_offset;
            extract_prelude(&ip, (const byte**)&ip2, &prelude);
        } else {
            // Load basic scope info for viper and asm
            prelude.scope_flags = read_uint(reader, NULL);
            prelude.n_pos_args = 0;
            prelude.n_kwonly_args = 0;
            if (kind == MP_CODE_NATIVE_ASM) {
                prelude.n_pos_args = read_uint(reader, NULL);
                type_sig = read_uint(reader, NULL);
            }
        }
    #endif
    }

    if (kind == MP_CODE_BYTECODE || kind == MP_CODE_NATIVE_PY) {
        // Load qstrs in prelude
        qstr simple_name = load_qstr(reader, qw);
        qstr source_file = load_qstr(reader, qw);
        ip2[0] = simple_name; ip2[1] = simple_name >> 8;
        ip2[2] = source_file; ip2[3] = source_file >> 8;
    }

    mp_uint_t *const_table = NULL;
    if (kind != MP_CODE_NATIVE_ASM) {
        // Load constant table for bytecode, native and viper

        // Number of entries in constant table
        size_t n_obj = read_uint(reader, NULL);
        size_t n_raw_code = read_uint(reader, NULL);

        // Allocate constant table
        size_t n_alloc = prelude.n_pos_args + prelude.n_kwonly_args + n_obj + n_raw_code;
        if (kind != MP_CODE_BYTECODE) {
            ++n_alloc; // additional entry for mp_fun_table
        }
        const_table = m_new(mp_uint_t, n_alloc);
        mp_uint_t *ct = const_table;

        // Load function argument names (initial entries in const_table)
        // (viper has n_pos_args=n_kwonly_args=0 so doesn't load any qstrs here)
        for (size_t i = 0; i < prelude.n_pos_args + prelude.n_kwonly_args; ++i) {
            *ct++ = (mp_uint_t)MP_OBJ_NEW_QSTR(load_qstr(reader, qw));
        }

        #if MICROPY_EMIT_NATIVE
        if (kind != MP_CODE_BYTECODE) {
            // Populate mp_fun_table entry
            *ct++ = (mp_uint_t)(uintptr_t)mp_fun_table;
        }
        #endif

        // Load constant objects and raw code children
        for (size_t i = 0; i < n_obj; ++i) {
            *ct++ = (mp_uint_t)load_obj(reader);
        }
        for (size_t i = 0; i < n_raw_code; ++i) {
            *ct++ = (mp_uint_t)(uintptr_t)load_raw_code(reader, qw);
        }
    }

    // Create raw_code and return it
    mp_raw_code_t *rc = mp_emit_glue_new_raw_code();
    if (kind == MP_CODE_BYTECODE) {
        mp_emit_glue_assign_bytecode(rc, fun_data,
            #if MICROPY_PERSISTENT_CODE_SAVE || MICROPY_DEBUG_PRINTERS
            fun_data_len,
            #endif
            const_table,
            #if MICROPY_PERSISTENT_CODE_SAVE
            n_obj, n_raw_code,
            #endif
            prelude.scope_flags);

    #if MICROPY_EMIT_NATIVE
    } else {
        #if defined(MP_PLAT_COMMIT_EXEC)
        fun_data = MP_PLAT_COMMIT_EXEC(fun_data, fun_data_len);
        #endif

        mp_emit_glue_assign_native(rc, kind,
            fun_data, fun_data_len, const_table,
            #if MICROPY_PERSISTENT_CODE_SAVE
            prelude_offset,
            n_obj, n_raw_code,
            n_qstr_link, NULL,
            #endif
            prelude.n_pos_args, prelude.scope_flags, type_sig);
    #endif
    }
    return rc;
}

mp_raw_code_t *mp_raw_code_load(mp_reader_t *reader) {
    byte header[4];
    read_bytes(reader, header, sizeof(header));
    if (header[0] != 'M'
        || header[1] != MPY_VERSION
        || MPY_FEATURE_DECODE_FLAGS(header[2]) != MPY_FEATURE_FLAGS
        || header[3] > mp_small_int_bits()
        || read_uint(reader, NULL) > QSTR_WINDOW_SIZE) {
        mp_raise_ValueError("incompatible .mpy file");
    }
    if (MPY_FEATURE_DECODE_ARCH(header[2]) != MP_NATIVE_ARCH_NONE
        && MPY_FEATURE_DECODE_ARCH(header[2]) != MPY_FEATURE_ARCH) {
        mp_raise_ValueError("incompatible .mpy arch");
    }
    qstr_window_t qw;
    qw.idx = 0;
    mp_raw_code_t *rc = load_raw_code(reader, &qw);
    reader->close(reader->data);
    return rc;
}

mp_raw_code_t *mp_raw_code_load_mem(const byte *buf, size_t len) {
    mp_reader_t reader;
    mp_reader_new_mem(&reader, buf, len, 0);
    return mp_raw_code_load(&reader);
}

#if MICROPY_HAS_FILE_READER

mp_raw_code_t *mp_raw_code_load_file(const char *filename) {
    mp_reader_t reader;
    mp_reader_new_file(&reader, filename);
    return mp_raw_code_load(&reader);
}

#endif // MICROPY_HAS_FILE_READER

#endif // MICROPY_PERSISTENT_CODE_LOAD

#if MICROPY_PERSISTENT_CODE_SAVE

#include "py/objstr.h"

STATIC void mp_print_bytes(mp_print_t *print, const byte *data, size_t len) {
    print->print_strn(print->data, (const char*)data, len);
}

#define BYTES_FOR_INT ((BYTES_PER_WORD * 8 + 6) / 7)
STATIC void mp_print_uint(mp_print_t *print, size_t n) {
    byte buf[BYTES_FOR_INT];
    byte *p = buf + sizeof(buf);
    *--p = n & 0x7f;
    n >>= 7;
    for (; n != 0; n >>= 7) {
        *--p = 0x80 | (n & 0x7f);
    }
    print->print_strn(print->data, (char*)p, buf + sizeof(buf) - p);
}

STATIC void save_qstr(mp_print_t *print, qstr_window_t *qw, qstr qst) {
    if (qst <= QSTR_LAST_STATIC) {
        // encode static qstr
        byte buf[2] = {0, qst & 0xff};
        mp_print_bytes(print, buf, 2);
        return;
    }
    size_t idx = qstr_window_insert(qw, qst);
    if (idx < QSTR_WINDOW_SIZE) {
        // qstr found in window, encode index to it
        mp_print_uint(print, idx << 1 | 1);
        return;
    }
    size_t len;
    const byte *str = qstr_data(qst, &len);
    mp_print_uint(print, len << 1);
    mp_print_bytes(print, str, len);
}

STATIC void save_obj(mp_print_t *print, mp_obj_t o) {
    if (mp_obj_is_str_or_bytes(o)) {
        byte obj_type;
        if (mp_obj_is_str(o)) {
            obj_type = 's';
        } else {
            obj_type = 'b';
        }
        mp_uint_t len;
        const char *str = mp_obj_str_get_data(o, &len);
        mp_print_bytes(print, &obj_type, 1);
        mp_print_uint(print, len);
        mp_print_bytes(print, (const byte*)str, len);
    } else if (MP_OBJ_TO_PTR(o) == &mp_const_ellipsis_obj) {
        byte obj_type = 'e';
        mp_print_bytes(print, &obj_type, 1);
    } else {
        // we save numbers using a simplistic text representation
        // TODO could be improved
        byte obj_type;
        if (mp_obj_is_type(o, &mp_type_int)) {
            obj_type = 'i';
        #if MICROPY_PY_BUILTINS_COMPLEX
        } else if (mp_obj_is_type(o, &mp_type_complex)) {
            obj_type = 'c';
        #endif
        } else {
            assert(mp_obj_is_float(o));
            obj_type = 'f';
        }
        vstr_t vstr;
        mp_print_t pr;
        vstr_init_print(&vstr, 10, &pr);
        mp_obj_print_helper(&pr, o, PRINT_REPR);
        mp_print_bytes(print, &obj_type, 1);
        mp_print_uint(print, vstr.len);
        mp_print_bytes(print, (const byte*)vstr.buf, vstr.len);
        vstr_clear(&vstr);
    }
}

STATIC void save_bytecode(mp_print_t *print, qstr_window_t *qw, const byte *ip, const byte *ip_top) {
    while (ip < ip_top) {
        size_t sz;
        uint f = mp_opcode_format(ip, &sz, true);
        if (f == MP_OPCODE_QSTR) {
            mp_print_bytes(print, ip, 1);
            qstr qst = ip[1] | (ip[2] << 8);
            save_qstr(print, qw, qst);
            ip += 3;
            sz -= 3;
        }
        mp_print_bytes(print, ip, sz);
        ip += sz;
    }
}

STATIC void save_raw_code(mp_print_t *print, mp_raw_code_t *rc, qstr_window_t *qstr_window) {
    // Save function kind and data length
    mp_print_uint(print, (rc->fun_data_len << 2) | (rc->kind - MP_CODE_BYTECODE));

    const byte *ip2;
    bytecode_prelude_t prelude;

    if (rc->kind == MP_CODE_BYTECODE) {
        // Save prelude
        const byte *ip = rc->fun_data;
        extract_prelude(&ip, &ip2, &prelude);
        size_t prelude_len = ip - (const byte*)rc->fun_data;
        const byte *ip_top = (const byte*)rc->fun_data + rc->fun_data_len;
        mp_print_bytes(print, rc->fun_data, prelude_len);

        // Save bytecode
        save_bytecode(print, qstr_window, ip, ip_top);
    } else {
        // Save native code
        mp_print_bytes(print, rc->fun_data, rc->fun_data_len);

        if (rc->kind == MP_CODE_NATIVE_PY || rc->kind == MP_CODE_NATIVE_VIPER) {
            // Save qstr link table for native code
            mp_print_uint(print, rc->n_qstr);
            for (size_t i = 0; i < rc->n_qstr; ++i) {
                mp_print_uint(print, rc->qstr_link[i].off);
                save_qstr(print, qstr_window, rc->qstr_link[i].qst);
            }
        }

        if (rc->kind == MP_CODE_NATIVE_PY) {
            // Save prelude size, and extract prelude for later use
            mp_print_uint(print, rc->prelude_offset);
            const byte *ip = (const byte*)rc->fun_data + rc->prelude_offset;
            extract_prelude(&ip, &ip2, &prelude);
        } else {
            // Save basic scope info for viper and asm
            mp_print_uint(print, rc->scope_flags);
            prelude.n_pos_args = 0;
            prelude.n_kwonly_args = 0;
            if (rc->kind == MP_CODE_NATIVE_ASM) {
                mp_print_uint(print, rc->n_pos_args);
                mp_print_uint(print, rc->type_sig);
            }
        }
    }

    if (rc->kind == MP_CODE_BYTECODE || rc->kind == MP_CODE_NATIVE_PY) {
        // Save qstrs in prelude
        save_qstr(print, qstr_window, ip2[0] | (ip2[1] << 8)); // simple_name
        save_qstr(print, qstr_window, ip2[2] | (ip2[3] << 8)); // source_file
    }

    if (rc->kind != MP_CODE_NATIVE_ASM) {
        // Save constant table for bytecode, native and viper

        // Number of entries in constant table
        mp_print_uint(print, rc->n_obj);
        mp_print_uint(print, rc->n_raw_code);

        const mp_uint_t *const_table = rc->const_table;

        // Save function argument names (initial entries in const_table)
        // (viper has n_pos_args=n_kwonly_args=0 so doesn't save any qstrs here)
        for (size_t i = 0; i < prelude.n_pos_args + prelude.n_kwonly_args; ++i) {
            mp_obj_t o = (mp_obj_t)*const_table++;
            save_qstr(print, qstr_window, MP_OBJ_QSTR_VALUE(o));
        }

        if (rc->kind != MP_CODE_BYTECODE) {
            // Skip saving mp_fun_table entry
            ++const_table;
        }

        // Save constant objects and raw code children
        for (size_t i = 0; i < rc->n_obj; ++i) {
            save_obj(print, (mp_obj_t)*const_table++);
        }
        for (size_t i = 0; i < rc->n_raw_code; ++i) {
            save_raw_code(print, (mp_raw_code_t*)(uintptr_t)*const_table++, qstr_window);
        }
    }
}

STATIC bool mp_raw_code_has_native(mp_raw_code_t *rc) {
    if (rc->kind != MP_CODE_BYTECODE) {
        return true;
    }

    const byte *ip = rc->fun_data;
    const byte *ip2;
    bytecode_prelude_t prelude;
    extract_prelude(&ip, &ip2, &prelude);

    const mp_uint_t *const_table = rc->const_table
        + prelude.n_pos_args + prelude.n_kwonly_args
        + rc->n_obj;

    for (size_t i = 0; i < rc->n_raw_code; ++i) {
        if (mp_raw_code_has_native((mp_raw_code_t*)(uintptr_t)*const_table++)) {
            return true;
        }
    }

    return false;
}

void mp_raw_code_save(mp_raw_code_t *rc, mp_print_t *print) {
    // header contains:
    //  byte  'M'
    //  byte  version
    //  byte  feature flags
    //  byte  number of bits in a small int
    //  uint  size of qstr window
    byte header[4] = {
        'M',
        MPY_VERSION,
        MPY_FEATURE_ENCODE_FLAGS(MPY_FEATURE_FLAGS_DYNAMIC),
        #if MICROPY_DYNAMIC_COMPILER
        mp_dynamic_compiler.small_int_bits,
        #else
        mp_small_int_bits(),
        #endif
    };
    if (mp_raw_code_has_native(rc)) {
        header[2] |= MPY_FEATURE_ENCODE_ARCH(MPY_FEATURE_ARCH);
    }
    mp_print_bytes(print, header, sizeof(header));
    mp_print_uint(print, QSTR_WINDOW_SIZE);

    qstr_window_t qw;
    qw.idx = 0;
    memset(qw.window, 0, sizeof(qw.window));
    save_raw_code(print, rc, &qw);
}

// here we define mp_raw_code_save_file depending on the port
// TODO abstract this away properly

#if defined(__i386__) || defined(__x86_64__) || defined(__unix__)

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

STATIC void fd_print_strn(void *env, const char *str, size_t len) {
    int fd = (intptr_t)env;
    ssize_t ret = write(fd, str, len);
    (void)ret;
}

void mp_raw_code_save_file(mp_raw_code_t *rc, const char *filename) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    mp_print_t fd_print = {(void*)(intptr_t)fd, fd_print_strn};
    mp_raw_code_save(rc, &fd_print);
    close(fd);
}

#else
#error mp_raw_code_save_file not implemented for this platform
#endif

#endif // MICROPY_PERSISTENT_CODE_SAVE
