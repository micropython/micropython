/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
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
#ifndef MICROPY_INCLUDED_PY_MISC_H
#define MICROPY_INCLUDED_PY_MISC_H

// a mini library of useful types and functions

/** types *******************************************************/

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "mpconfig.h"

typedef unsigned char byte;
typedef unsigned int uint;

/** generic ops *************************************************/

#ifndef MIN
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#endif
#ifndef MAX
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#endif

// Classical double-indirection stringification of preprocessor macro's value
#define MP_STRINGIFY_HELPER(x) #x
#define MP_STRINGIFY(x) MP_STRINGIFY_HELPER(x)

// Static assertion macro
#define MP_STATIC_ASSERT(cond) ((void)sizeof(char[1 - 2 * !(cond)]))

// Round-up integer division
#define MP_CEIL_DIVIDE(a, b) (((a) + (b) - 1) / (b))

/** memory allocation ******************************************/

// TODO make a lazy m_renew that can increase by a smaller amount than requested (but by at least 1 more element)

#define m_new(type, num) ((type *)(m_malloc(sizeof(type) * (num), false)))
#define m_new_ll(type, num) ((type *)(m_malloc(sizeof(type) * (num), true)))
#define m_new_maybe(type, num) ((type *)(m_malloc_maybe(sizeof(type) * (num), false)))
#define m_new_ll_maybe(type, num) ((type *)(m_malloc_maybe(sizeof(type) * (num), true)))
#define m_new0(type, num) ((type *)(m_malloc0(sizeof(type) * (num), false)))
#define m_new0_ll(type, num) ((type *)(m_malloc0(sizeof(type) * (num), true)))
#define m_new_obj(type) (m_new(type, 1))
#define m_new_ll_obj(type) (m_new_ll(type, 1))
#define m_new_obj_maybe(type) (m_new_maybe(type, 1))
#define m_new_obj_var(obj_type, var_type, var_num) ((obj_type *)m_malloc(sizeof(obj_type) + sizeof(var_type) * (var_num), false))
#define m_new_obj_var_maybe(obj_type, var_type, var_num) ((obj_type *)m_malloc_maybe(sizeof(obj_type) + sizeof(var_type) * (var_num), false))
#define m_new_ll_obj_var_maybe(obj_type, var_type, var_num) ((obj_type *)m_malloc_maybe(sizeof(obj_type) + sizeof(var_type) * (var_num), true))
#if MICROPY_ENABLE_FINALISER
#define m_new_obj_with_finaliser(type) ((type *)(m_malloc_with_finaliser(sizeof(type), false)))
#define m_new_obj_var_with_finaliser(type, var_type, var_num) ((type *)m_malloc_with_finaliser(sizeof(type) + sizeof(var_type) * (var_num), false))
#define m_new_ll_obj_with_finaliser(type) ((type *)(m_malloc_with_finaliser(sizeof(type), true)))
#else
#define m_new_obj_with_finaliser(type) m_new_obj(type)
#define m_new_obj_var_with_finaliser(type, var_type, var_num) m_new_obj_var(type, var_type, var_num)
#define m_new_ll_obj_with_finaliser(type) m_new_ll_obj(type)
#endif
#if MICROPY_MALLOC_USES_ALLOCATED_SIZE
#define m_renew(type, ptr, old_num, new_num) ((type *)(m_realloc((ptr), sizeof(type) * (old_num), sizeof(type) * (new_num))))
#define m_renew_maybe(type, ptr, old_num, new_num, allow_move) ((type *)(m_realloc_maybe((ptr), sizeof(type) * (old_num), sizeof(type) * (new_num), (allow_move))))
#define m_del(type, ptr, num) m_free(ptr, sizeof(type) * (num))
#define m_del_var(obj_type, var_type, var_num, ptr) (m_free(ptr, sizeof(obj_type) + sizeof(var_type) * (var_num)))
#else
#define m_renew(type, ptr, old_num, new_num) ((type *)(m_realloc((ptr), sizeof(type) * (new_num))))
#define m_renew_maybe(type, ptr, old_num, new_num, allow_move) ((type *)(m_realloc_maybe((ptr), sizeof(type) * (new_num), (allow_move))))
#define m_del(type, ptr, num) ((void)(num), m_free(ptr))
#define m_del_var(obj_type, var_type, var_num, ptr) ((void)(var_num), m_free(ptr))
#endif
#define m_del_obj(type, ptr) (m_del(type, ptr, 1))

void *m_malloc(size_t num_bytes, bool long_lived);
void *m_malloc_maybe(size_t num_bytes, bool long_lived);
void *m_malloc_with_finaliser(size_t num_bytes, bool long_lived);
void *m_malloc0(size_t num_bytes, bool long_lived);
#if MICROPY_MALLOC_USES_ALLOCATED_SIZE
void *m_realloc(void *ptr, size_t old_num_bytes, size_t new_num_bytes);
void *m_realloc_maybe(void *ptr, size_t old_num_bytes, size_t new_num_bytes, bool allow_move);
void m_free(void *ptr, size_t num_bytes);
#else
void *m_realloc(void *ptr, size_t new_num_bytes);
void *m_realloc_maybe(void *ptr, size_t new_num_bytes, bool allow_move);
void m_free(void *ptr);
#endif
NORETURN void m_malloc_fail(size_t num_bytes);

#if MICROPY_MEM_STATS
size_t m_get_total_bytes_allocated(void);
size_t m_get_current_bytes_allocated(void);
size_t m_get_peak_bytes_allocated(void);
#endif

/** array helpers ***********************************************/

// get the number of elements in a fixed-size array
#define MP_ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

// align ptr to the nearest multiple of "alignment"
#define MP_ALIGN(ptr, alignment) (void *)(((uintptr_t)(ptr) + ((alignment) - 1)) & ~((alignment) - 1))

#define sizeof_field(TYPE, MEMBER) sizeof((((TYPE *)0)->MEMBER))

/** unichar / UTF-8 *********************************************/

#if MICROPY_PY_BUILTINS_STR_UNICODE
// with unicode enabled we need a type which can fit chars up to 0x10ffff
typedef uint32_t unichar;
#else
// without unicode enabled we can only need to fit chars up to 0xff
// (on 16-bit archs uint is 16-bits and more efficient than uint32_t)
typedef uint unichar;
#endif

#if MICROPY_PY_BUILTINS_STR_UNICODE
unichar utf8_get_char(const byte *s);
const byte *utf8_next_char(const byte *s);
size_t utf8_charlen(const byte *str, size_t len);
#else
static inline unichar utf8_get_char(const byte *s) {
    return *s;
}
static inline const byte *utf8_next_char(const byte *s) {
    return s + 1;
}
static inline size_t utf8_charlen(const byte *str, size_t len) {
    (void)str;
    return len;
}
#endif

bool unichar_isspace(unichar c);
bool unichar_isalpha(unichar c);
bool unichar_isprint(unichar c);
bool unichar_isdigit(unichar c);
bool unichar_isxdigit(unichar c);
bool unichar_isident(unichar c);
bool unichar_isalnum(unichar c);
bool unichar_isupper(unichar c);
bool unichar_islower(unichar c);
unichar unichar_tolower(unichar c);
unichar unichar_toupper(unichar c);
mp_uint_t unichar_xdigit_value(unichar c);
#define UTF8_IS_NONASCII(ch) ((ch) & 0x80)
#define UTF8_IS_CONT(ch) (((ch) & 0xC0) == 0x80)

/** variable string *********************************************/

typedef struct _vstr_t {
    size_t alloc;
    size_t len;
    char *buf;
    bool fixed_buf : 1;
} vstr_t;

// convenience macro to declare a vstr with a fixed size buffer on the stack
#define VSTR_FIXED(vstr, alloc) vstr_t vstr; char vstr##_buf[(alloc)]; vstr_init_fixed_buf(&vstr, (alloc), vstr##_buf);

void vstr_init(vstr_t *vstr, size_t alloc);
void vstr_init_len(vstr_t *vstr, size_t len);
void vstr_init_fixed_buf(vstr_t *vstr, size_t alloc, char *buf);
struct _mp_print_t;
void vstr_init_print(vstr_t *vstr, size_t alloc, struct _mp_print_t *print);
void vstr_clear(vstr_t *vstr);
vstr_t *vstr_new(size_t alloc);
void vstr_free(vstr_t *vstr);
static inline void vstr_reset(vstr_t *vstr) {
    vstr->len = 0;
}
static inline char *vstr_str(vstr_t *vstr) {
    return vstr->buf;
}
static inline size_t vstr_len(vstr_t *vstr) {
    return vstr->len;
}
void vstr_hint_size(vstr_t *vstr, size_t size);
char *vstr_extend(vstr_t *vstr, size_t size);
char *vstr_add_len(vstr_t *vstr, size_t len);
char *vstr_null_terminated_str(vstr_t *vstr);
void vstr_add_byte(vstr_t *vstr, byte v);
void vstr_add_char(vstr_t *vstr, unichar chr);
void vstr_add_str(vstr_t *vstr, const char *str);
void vstr_add_strn(vstr_t *vstr, const char *str, size_t len);
void vstr_ins_byte(vstr_t *vstr, size_t byte_pos, byte b);
void vstr_ins_char(vstr_t *vstr, size_t char_pos, unichar chr);
void vstr_cut_head_bytes(vstr_t *vstr, size_t bytes_to_cut);
void vstr_cut_tail_bytes(vstr_t *vstr, size_t bytes_to_cut);
void vstr_cut_out_bytes(vstr_t *vstr, size_t byte_pos, size_t bytes_to_cut);
void vstr_printf(vstr_t *vstr, const char *fmt, ...);

/** non-dynamic size-bounded variable buffer/string *************/

#define CHECKBUF(buf, max_size) char buf[max_size + 1]; size_t buf##_len = max_size; char *buf##_p = buf;
#define CHECKBUF_RESET(buf, max_size) buf##_len = max_size; buf##_p = buf;
#define CHECKBUF_APPEND(buf, src, src_len) \
    { size_t l = MIN(src_len, buf##_len); \
      memcpy(buf##_p, src, l); \
      buf##_len -= l; \
      buf##_p += l; }
#define CHECKBUF_APPEND_0(buf) { *buf##_p = 0; }
#define CHECKBUF_LEN(buf) (buf##_p - buf)

#ifdef va_start
void vstr_vprintf(vstr_t *vstr, const char *fmt, va_list ap);
#endif

// Debugging helpers
int DEBUG_printf(const char *fmt, ...);

extern mp_uint_t mp_verbose_flag;

/** float internals *************/

#if MICROPY_PY_BUILTINS_FLOAT

#if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_DOUBLE
#define MP_FLOAT_EXP_BITS (11)
#define MP_FLOAT_FRAC_BITS (52)
typedef uint64_t mp_float_uint_t;
#elif MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
#define MP_FLOAT_EXP_BITS (8)
#define MP_FLOAT_FRAC_BITS (23)
typedef uint32_t mp_float_uint_t;
#endif

#define MP_FLOAT_EXP_BIAS ((1 << (MP_FLOAT_EXP_BITS - 1)) - 1)

typedef union _mp_float_union_t {
    mp_float_t f;
    #if MP_ENDIANNESS_LITTLE
    struct {
        mp_float_uint_t frc : MP_FLOAT_FRAC_BITS;
        mp_float_uint_t exp : MP_FLOAT_EXP_BITS;
        mp_float_uint_t sgn : 1;
    } p;
    #else
    struct {
        mp_float_uint_t sgn : 1;
        mp_float_uint_t exp : MP_FLOAT_EXP_BITS;
        mp_float_uint_t frc : MP_FLOAT_FRAC_BITS;
    } p;
    #endif
    mp_float_uint_t i;
} mp_float_union_t;

#endif // MICROPY_PY_BUILTINS_FLOAT

#endif // MICROPY_INCLUDED_PY_MISC_H
