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
#define MPY_VERSION (3)

// Macros to encode/decode flags to/from the feature byte
#define MPY_FEATURE_ENCODE_FLAGS(flags) (flags)
#define MPY_FEATURE_DECODE_FLAGS(feat) ((feat) & 3)

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

#if MICROPY_PERSISTENT_CODE_SAVE

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
    // get bytecode size and allocate memory for it
    size_t bc_len = read_uint(reader, NULL);
    byte *bytecode = m_new(byte, bc_len);

    // load prelude
    byte *ip = bytecode;
    byte *ip2;
    bytecode_prelude_t prelude;
    load_prelude(reader, &ip, &ip2, &prelude);

    // load bytecode
    load_bytecode(reader, qw, ip, bytecode + bc_len);

    // load qstrs and link global qstr ids into bytecode
    qstr simple_name = load_qstr(reader, qw);
    qstr source_file = load_qstr(reader, qw);
    ((byte*)ip2)[0] = simple_name; ((byte*)ip2)[1] = simple_name >> 8;
    ((byte*)ip2)[2] = source_file; ((byte*)ip2)[3] = source_file >> 8;

    // load constant table
    size_t n_obj = read_uint(reader, NULL);
    size_t n_raw_code = read_uint(reader, NULL);
    mp_uint_t *const_table = m_new(mp_uint_t, prelude.n_pos_args + prelude.n_kwonly_args + n_obj + n_raw_code);
    mp_uint_t *ct = const_table;
    for (size_t i = 0; i < prelude.n_pos_args + prelude.n_kwonly_args; ++i) {
        *ct++ = (mp_uint_t)MP_OBJ_NEW_QSTR(load_qstr(reader, qw));
    }
    for (size_t i = 0; i < n_obj; ++i) {
        *ct++ = (mp_uint_t)load_obj(reader);
    }
    for (size_t i = 0; i < n_raw_code; ++i) {
        *ct++ = (mp_uint_t)(uintptr_t)load_raw_code(reader, qw);
    }

    // create raw_code and return it
    mp_raw_code_t *rc = mp_emit_glue_new_raw_code();
    mp_emit_glue_assign_bytecode(rc, bytecode,
        #if MICROPY_PERSISTENT_CODE_SAVE || MICROPY_DEBUG_PRINTERS
        bc_len,
        #endif
        const_table,
        #if MICROPY_PERSISTENT_CODE_SAVE
        n_obj, n_raw_code,
        #endif
        prelude.scope_flags);
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
    if (rc->kind != MP_CODE_BYTECODE) {
        mp_raise_ValueError("can only save bytecode");
    }

    // extract prelude
    const byte *ip = rc->fun_data;
    const byte *ip2;
    bytecode_prelude_t prelude;
    extract_prelude(&ip, &ip2, &prelude);

    // save prelude
    size_t prelude_len = ip - rc->fun_data;
    const byte *ip_top = rc->fun_data + rc->fun_data_len;
    mp_print_uint(print, rc->fun_data_len);
    mp_print_bytes(print, rc->fun_data, prelude_len);

    // save bytecode
    save_bytecode(print, qstr_window, ip, ip_top);

    // save qstrs
    save_qstr(print, qstr_window, ip2[0] | (ip2[1] << 8)); // simple_name
    save_qstr(print, qstr_window, ip2[2] | (ip2[3] << 8)); // source_file

    // save constant table
    mp_print_uint(print, rc->n_obj);
    mp_print_uint(print, rc->n_raw_code);
    const mp_uint_t *const_table = rc->const_table;
    for (uint i = 0; i < prelude.n_pos_args + prelude.n_kwonly_args; ++i) {
        mp_obj_t o = (mp_obj_t)*const_table++;
        save_qstr(print, qstr_window, MP_OBJ_QSTR_VALUE(o));
    }
    for (uint i = 0; i < rc->n_obj; ++i) {
        save_obj(print, (mp_obj_t)*const_table++);
    }
    for (uint i = 0; i < rc->n_raw_code; ++i) {
        save_raw_code(print, (mp_raw_code_t*)(uintptr_t)*const_table++, qstr_window);
    }
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
