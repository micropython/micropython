/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2013-2020 Damien P. George
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

#include "supervisor/shared/translate.h"

#if MICROPY_PERSISTENT_CODE_LOAD || MICROPY_PERSISTENT_CODE_SAVE

#include "py/smallint.h"

#define QSTR_LAST_STATIC MP_QSTR_zip

#if MICROPY_DYNAMIC_COMPILER
#define MPY_FEATURE_ARCH_DYNAMIC mp_dynamic_compiler.native_arch
#else
#define MPY_FEATURE_ARCH_DYNAMIC MPY_FEATURE_ARCH
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

// ip will point to start of opcodes
// return value will point to simple_name, source_file qstrs
STATIC byte *extract_prelude(const byte **ip, bytecode_prelude_t *prelude) {
    MP_BC_PRELUDE_SIG_DECODE(*ip);
    prelude->n_state = n_state;
    prelude->n_exc_stack = n_exc_stack;
    prelude->scope_flags = scope_flags;
    prelude->n_pos_args = n_pos_args;
    prelude->n_kwonly_args = n_kwonly_args;
    prelude->n_def_pos_args = n_def_pos_args;
    MP_BC_PRELUDE_SIZE_DECODE(*ip);
    byte *ip_info = (byte *)*ip;
    *ip += n_info;
    *ip += n_cell;
    return ip_info;
}

#endif // MICROPY_PERSISTENT_CODE_LOAD || MICROPY_PERSISTENT_CODE_SAVE

#if MICROPY_PERSISTENT_CODE_LOAD

#include "py/parsenum.h"

STATIC void raise_corrupt_mpy(void) {
    mp_raise_RuntimeError(MP_ERROR_TEXT("Corrupt .mpy file"));
}

STATIC int read_byte(mp_reader_t *reader);
STATIC size_t read_uint(mp_reader_t *reader, byte **out);

#if MICROPY_EMIT_MACHINE_CODE

typedef struct _reloc_info_t {
    mp_reader_t *reader;
    mp_uint_t *const_table;
} reloc_info_t;

#if MICROPY_EMIT_THUMB
STATIC void asm_thumb_rewrite_mov(uint8_t *pc, uint16_t val) {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcast-align"
    // high part
    *(uint16_t *)pc = (*(uint16_t *)pc & 0xfbf0) | (val >> 1 & 0x0400) | (val >> 12);
    // low part
    *(uint16_t *)(pc + 2) = (*(uint16_t *)(pc + 2) & 0x0f00) | (val << 4 & 0x7000) | (val & 0x00ff);
    #pragma GCC diagnostic pop
}
#endif

STATIC void arch_link_qstr(uint8_t *pc, bool is_obj, qstr qst) {
    mp_uint_t val = qst;
    if (is_obj) {
        val = (mp_uint_t)MP_OBJ_NEW_QSTR(qst);
    }
    #if MICROPY_EMIT_X86 || MICROPY_EMIT_X64 || MICROPY_EMIT_ARM || MICROPY_EMIT_XTENSA || MICROPY_EMIT_XTENSAWIN
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

void mp_native_relocate(void *ri_in, uint8_t *text, uintptr_t reloc_text) {
    // Relocate native code
    reloc_info_t *ri = ri_in;
    uintptr_t *addr_to_adjust = NULL;

    // Read the byte directly so that we don't error on EOF.
    mp_uint_t op = ri->reader->readbyte(ri->reader->data);
    while (op != 0xff && op != MP_READER_EOF) {
        if (op & 1) {
            // Point to new location to make adjustments
            size_t addr = read_uint(ri->reader, NULL);
            if ((addr & 1) == 0) {
                // Point to somewhere in text
                #pragma GCC diagnostic push
                #pragma GCC diagnostic ignored "-Wcast-align"
                addr_to_adjust = &((uintptr_t *)text)[addr >> 1];
                #pragma GCC diagnostic pop
            } else {
                // Point to somewhere in rodata
                addr_to_adjust = &((uintptr_t *)ri->const_table[1])[addr >> 1];
            }
        }
        op >>= 1;
        uintptr_t dest;
        size_t n = 1;
        if (op <= 5) {
            if (op & 1) {
                // Read in number of adjustments to make
                n = read_uint(ri->reader, NULL);
            }
            op >>= 1;
            if (op == 0) {
                // Destination is text
                dest = reloc_text;
            } else {
                // Destination is rodata (op=1) or bss (op=1 if no rodata, else op=2)
                dest = ri->const_table[op];
            }
        } else if (op == 6) {
            // Destination is mp_fun_table itself
            dest = (uintptr_t)&mp_fun_table;
        } else {
            // Destination is an entry in mp_fun_table
            dest = ((uintptr_t *)&mp_fun_table)[op - 7];
        }
        while (n--) {
            *addr_to_adjust++ += dest;
        }
        op = ri->reader->readbyte(ri->reader->data);
    }
}

#endif

STATIC int read_byte(mp_reader_t *reader) {
    mp_uint_t b = reader->readbyte(reader->data);
    if (b == MP_READER_EOF) {
        raise_corrupt_mpy();
    }
    return b;
}

STATIC void read_bytes(mp_reader_t *reader, byte *buf, size_t len) {
    while (len-- > 0) {
        mp_uint_t b = reader->readbyte(reader->data);
        if (b == MP_READER_EOF) {
            raise_corrupt_mpy();
        }
        *buf++ = b;
    }
}

STATIC size_t read_uint(mp_reader_t *reader, byte **out) {
    size_t unum = 0;
    for (;;) {
        mp_uint_t b = reader->readbyte(reader->data);
        if (b == MP_READER_EOF) {
            raise_corrupt_mpy();
        }
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
    read_bytes(reader, (byte *)str, len);
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
        read_bytes(reader, (byte *)vstr.buf, len);
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

STATIC void load_prelude_qstrs(mp_reader_t *reader, qstr_window_t *qw, byte *ip) {
    qstr simple_name = load_qstr(reader, qw);
    ip[0] = simple_name;
    ip[1] = simple_name >> 8;
    qstr source_file = load_qstr(reader, qw);
    ip[2] = source_file;
    ip[3] = source_file >> 8;
}

STATIC void load_prelude(mp_reader_t *reader, qstr_window_t *qw, byte **ip, bytecode_prelude_t *prelude) {
    // Read in the prelude header
    byte *ip_read = *ip;
    read_uint(reader, &ip_read);                    // read in n_state/etc (is effectively a var-uint)
    read_uint(reader, &ip_read);                    // read in n_info/n_cell (is effectively a var-uint)

    // Prelude header has been read into *ip, now decode and extract values from it
    extract_prelude((const byte **)ip, prelude);

    // Load qstrs in prelude
    load_prelude_qstrs(reader, qw, ip_read);
    ip_read += 4;

    // Read remaining code info
    read_bytes(reader, ip_read, *ip - ip_read);
}

STATIC void load_bytecode(mp_reader_t *reader, qstr_window_t *qw, byte *ip, byte *ip_top) {
    while (ip < ip_top) {
        *ip = read_byte(reader);
        size_t sz;
        uint f = mp_opcode_format(ip, &sz, false);
        ++ip;
        --sz;
        if (f == MP_BC_FORMAT_QSTR) {
            qstr qst = load_qstr(reader, qw);
            *ip++ = qst;
            *ip++ = qst >> 8;
            sz -= 2;
        } else if (f == MP_BC_FORMAT_VAR_UINT) {
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

    #if !MICROPY_EMIT_MACHINE_CODE
    if (kind != MP_CODE_BYTECODE) {

    }
    #endif

    uint8_t *fun_data = NULL;
    bytecode_prelude_t prelude = {0};
    #if MICROPY_EMIT_MACHINE_CODE
    size_t prelude_offset = 0;
    mp_uint_t type_sig = 0;
    size_t n_qstr_link = 0;
    #endif

    if (kind == MP_CODE_BYTECODE) {
        // Allocate memory for the bytecode
        fun_data = m_new(uint8_t, fun_data_len);

        // Load prelude
        byte *ip = fun_data;
        load_prelude(reader, qw, &ip, &prelude);

        // Load bytecode
        load_bytecode(reader, qw, ip, fun_data + fun_data_len);

    #if MICROPY_EMIT_MACHINE_CODE
    } else {
        // Allocate memory for native data and load it
        size_t fun_alloc;
        MP_PLAT_ALLOC_EXEC(fun_data_len, (void **)&fun_data, &fun_alloc);
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
                } else if ((off & 3) == 3) {
                    // Generic, aligned qstr-object link
                    #pragma GCC diagnostic push
                    #pragma GCC diagnostic ignored "-Wcast-align"
                    *(mp_obj_t *)dest = MP_OBJ_NEW_QSTR(qst);
                    #pragma GCC diagnostic pop
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
            byte *ip_info = extract_prelude(&ip, &prelude);
            // Load qstrs in prelude
            load_prelude_qstrs(reader, qw, ip_info);
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

    size_t n_obj = 0;
    size_t n_raw_code = 0;
    mp_uint_t *const_table = NULL;

    if (kind != MP_CODE_NATIVE_ASM) {
        // Load constant table for bytecode, native and viper

        // Number of entries in constant table
        n_obj = read_uint(reader, NULL);
        n_raw_code = read_uint(reader, NULL);

        // Allocate constant table
        size_t n_alloc = prelude.n_pos_args + prelude.n_kwonly_args + n_obj + n_raw_code;
        #if MICROPY_EMIT_MACHINE_CODE
        if (kind != MP_CODE_BYTECODE) {
            ++n_alloc; // additional entry for mp_fun_table
            if (prelude.scope_flags & MP_SCOPE_FLAG_VIPERRODATA) {
                ++n_alloc; // additional entry for rodata
            }
            if (prelude.scope_flags & MP_SCOPE_FLAG_VIPERBSS) {
                ++n_alloc; // additional entry for BSS
            }
        }
        #endif

        const_table = m_new(mp_uint_t, n_alloc);
        mp_uint_t *ct = const_table;

        // Load function argument names (initial entries in const_table)
        // (viper has n_pos_args=n_kwonly_args=0 so doesn't load any qstrs here)
        for (size_t i = 0; i < prelude.n_pos_args + prelude.n_kwonly_args; ++i) {
            *ct++ = (mp_uint_t)MP_OBJ_NEW_QSTR(load_qstr(reader, qw));
        }

        #if MICROPY_EMIT_MACHINE_CODE
        if (kind != MP_CODE_BYTECODE) {
            // Populate mp_fun_table entry
            *ct++ = (mp_uint_t)(uintptr_t)&mp_fun_table;

            // Allocate and load rodata if needed
            if (prelude.scope_flags & MP_SCOPE_FLAG_VIPERRODATA) {
                size_t size = read_uint(reader, NULL);
                uint8_t *rodata = m_new(uint8_t, size);
                read_bytes(reader, rodata, size);
                *ct++ = (uintptr_t)rodata;
            }

            // Allocate BSS if needed
            if (prelude.scope_flags & MP_SCOPE_FLAG_VIPERBSS) {
                size_t size = read_uint(reader, NULL);
                uint8_t *bss = m_new0(uint8_t, size);
                *ct++ = (uintptr_t)bss;
            }
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
        // Assign bytecode to raw code object
        mp_emit_glue_assign_bytecode(rc, fun_data,
            #if MICROPY_PERSISTENT_CODE_SAVE || MICROPY_DEBUG_PRINTERS
            fun_data_len,
            #endif
            const_table,
            #if MICROPY_PERSISTENT_CODE_SAVE
            n_obj, n_raw_code,
            #endif
            prelude.scope_flags);

    #if MICROPY_EMIT_MACHINE_CODE
    } else {
        // Relocate and commit code to executable address space
        reloc_info_t ri = {reader, const_table};
        #if defined(MP_PLAT_COMMIT_EXEC)
        void *opt_ri = (prelude.scope_flags & MP_SCOPE_FLAG_VIPERRELOC) ? &ri : NULL;
        fun_data = MP_PLAT_COMMIT_EXEC(fun_data, fun_data_len, opt_ri);
        #else
        if (prelude.scope_flags & MP_SCOPE_FLAG_VIPERRELOC) {
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

        // Assign native code to raw code object
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
    if (header[0] != 'C'
        || header[1] != MPY_VERSION
        || MPY_FEATURE_DECODE_FLAGS(header[2]) != MPY_FEATURE_FLAGS
        || header[3] > mp_small_int_bits()
        || read_uint(reader, NULL) > QSTR_WINDOW_SIZE) {
        mp_raise_MpyError(MP_ERROR_TEXT("Incompatible .mpy file. Please update all .mpy files. See http://adafru.it/mpy-update for more info."));
    }
    if (MPY_FEATURE_DECODE_ARCH(header[2]) != MP_NATIVE_ARCH_NONE) {
        byte arch = MPY_FEATURE_DECODE_ARCH(header[2]);
        if (!MPY_FEATURE_ARCH_TEST(arch)) {
            mp_raise_ValueError(MP_ERROR_TEXT("incompatible native .mpy architecture"));
        }
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
    print->print_strn(print->data, (const char *)data, len);
}

#define BYTES_FOR_INT ((MP_BYTES_PER_OBJ_WORD * 8 + 6) / 7)
STATIC void mp_print_uint(mp_print_t *print, size_t n) {
    byte buf[BYTES_FOR_INT];
    byte *p = buf + sizeof(buf);
    *--p = n & 0x7f;
    n >>= 7;
    for (; n != 0; n >>= 7) {
        *--p = 0x80 | (n & 0x7f);
    }
    print->print_strn(print->data, (char *)p, buf + sizeof(buf) - p);
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
        size_t len;
        const char *str = mp_obj_str_get_data(o, &len);
        mp_print_bytes(print, &obj_type, 1);
        mp_print_uint(print, len);
        mp_print_bytes(print, (const byte *)str, len);
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
        mp_print_bytes(print, (const byte *)vstr.buf, vstr.len);
        vstr_clear(&vstr);
    }
}

STATIC void save_prelude_qstrs(mp_print_t *print, qstr_window_t *qw, const byte *ip) {
    save_qstr(print, qw, ip[0] | (ip[1] << 8)); // simple_name
    save_qstr(print, qw, ip[2] | (ip[3] << 8)); // source_file
}

STATIC void save_bytecode(mp_print_t *print, qstr_window_t *qw, const byte *ip, const byte *ip_top) {
    while (ip < ip_top) {
        size_t sz;
        uint f = mp_opcode_format(ip, &sz, true);
        if (f == MP_BC_FORMAT_QSTR) {
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

    bytecode_prelude_t prelude;

    if (rc->kind == MP_CODE_BYTECODE) {
        // Extract prelude
        const byte *ip = rc->fun_data;
        const byte *ip_info = extract_prelude(&ip, &prelude);

        // Save prelude
        mp_print_bytes(print, rc->fun_data, ip_info - (const byte *)rc->fun_data);
        save_prelude_qstrs(print, qstr_window, ip_info);
        ip_info += 4;
        mp_print_bytes(print, ip_info, ip - ip_info);

        // Save bytecode
        const byte *ip_top = (const byte *)rc->fun_data + rc->fun_data_len;
        save_bytecode(print, qstr_window, ip, ip_top);
    #if MICROPY_EMIT_MACHINE_CODE
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
            // Save prelude size
            mp_print_uint(print, rc->prelude_offset);

            // Extract prelude and save qstrs in prelude
            const byte *ip = (const byte *)rc->fun_data + rc->prelude_offset;
            const byte *ip_info = extract_prelude(&ip, &prelude);
            save_prelude_qstrs(print, qstr_window, ip_info);
        } else {
            // Save basic scope info for viper and asm
            mp_print_uint(print, rc->scope_flags & MP_SCOPE_FLAG_ALL_SIG);
            prelude.n_pos_args = 0;
            prelude.n_kwonly_args = 0;
            if (rc->kind == MP_CODE_NATIVE_ASM) {
                mp_print_uint(print, rc->n_pos_args);
                mp_print_uint(print, rc->type_sig);
            }
        }
    #endif
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
            save_raw_code(print, (mp_raw_code_t *)(uintptr_t)*const_table++, qstr_window);
        }
    }
}

STATIC bool mp_raw_code_has_native(mp_raw_code_t *rc) {
    if (rc->kind != MP_CODE_BYTECODE) {
        return true;
    }

    const byte *ip = rc->fun_data;
    bytecode_prelude_t prelude;
    extract_prelude(&ip, &prelude);

    const mp_uint_t *const_table = rc->const_table
        + prelude.n_pos_args + prelude.n_kwonly_args
        + rc->n_obj;

    for (size_t i = 0; i < rc->n_raw_code; ++i) {
        if (mp_raw_code_has_native((mp_raw_code_t *)(uintptr_t)*const_table++)) {
            return true;
        }
    }

    return false;
}

void mp_raw_code_save(mp_raw_code_t *rc, mp_print_t *print) {
    // header contains:
    //  byte  'C'
    //  byte  version
    //  byte  feature flags
    //  byte  number of bits in a small int
    //  uint  size of qstr window
    byte header[4] = {
        'C',
        MPY_VERSION,
        MPY_FEATURE_ENCODE_FLAGS(MPY_FEATURE_FLAGS_DYNAMIC),
        #if MICROPY_DYNAMIC_COMPILER
        mp_dynamic_compiler.small_int_bits,
        #else
        mp_small_int_bits(),
        #endif
    };
    if (mp_raw_code_has_native(rc)) {
        header[2] |= MPY_FEATURE_ENCODE_ARCH(MPY_FEATURE_ARCH_DYNAMIC);
    }
    mp_print_bytes(print, header, sizeof(header));
    mp_print_uint(print, QSTR_WINDOW_SIZE);

    qstr_window_t qw;
    qw.idx = 0;
    memset(qw.window, 0, sizeof(qw.window));
    save_raw_code(print, rc, &qw);
}

#if MICROPY_PERSISTENT_CODE_SAVE_FILE

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

STATIC void fd_print_strn(void *env, const char *str, size_t len) {
    int fd = (intptr_t)env;
    MP_THREAD_GIL_EXIT();
    ssize_t ret = write(fd, str, len);
    MP_THREAD_GIL_ENTER();
    (void)ret;
}

void mp_raw_code_save_file(mp_raw_code_t *rc, const char *filename) {
    MP_THREAD_GIL_EXIT();
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    MP_THREAD_GIL_ENTER();
    mp_print_t fd_print = {(void *)(intptr_t)fd, fd_print_strn};
    mp_raw_code_save(rc, &fd_print);
    MP_THREAD_GIL_EXIT();
    close(fd);
    MP_THREAD_GIL_ENTER();
}

#endif // MICROPY_PERSISTENT_CODE_SAVE_FILE

#endif // MICROPY_PERSISTENT_CODE_SAVE
