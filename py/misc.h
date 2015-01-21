/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#ifndef __MICROPY_INCLUDED_PY_MISC_H__
#define __MICROPY_INCLUDED_PY_MISC_H__

// a mini library of useful types and functions

/** types *******************************************************/

#include <stdbool.h>
#include <stddef.h>

typedef unsigned char byte;
typedef unsigned int uint;

/** generic ops *************************************************/

#ifndef MIN
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#endif
#ifndef MAX
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#endif

/** memomry allocation ******************************************/

// TODO make a lazy m_renew that can increase by a smaller amount than requested (but by at least 1 more element)

#define m_new(type, num) ((type*)(m_malloc(sizeof(type) * (num))))
#define m_new_maybe(type, num) ((type*)(m_malloc_maybe(sizeof(type) * (num))))
#define m_new0(type, num) ((type*)(m_malloc0(sizeof(type) * (num))))
#define m_new_obj(type) (m_new(type, 1))
#define m_new_obj_maybe(type) (m_new_maybe(type, 1))
#define m_new_obj_var(obj_type, var_type, var_num) ((obj_type*)m_malloc(sizeof(obj_type) + sizeof(var_type) * (var_num)))
#define m_new_obj_var_maybe(obj_type, var_type, var_num) ((obj_type*)m_malloc_maybe(sizeof(obj_type) + sizeof(var_type) * (var_num)))
#if MICROPY_ENABLE_FINALISER
#define m_new_obj_with_finaliser(type) ((type*)(m_malloc_with_finaliser(sizeof(type))))
#else
#define m_new_obj_with_finaliser(type) m_new_obj(type)
#endif
#define m_renew(type, ptr, old_num, new_num) ((type*)(m_realloc((ptr), sizeof(type) * (old_num), sizeof(type) * (new_num))))
#define m_renew_maybe(type, ptr, old_num, new_num) ((type*)(m_realloc_maybe((ptr), sizeof(type) * (old_num), sizeof(type) * (new_num))))
#define m_del(type, ptr, num) m_free(ptr, sizeof(type) * (num))
#define m_del_obj(type, ptr) (m_del(type, ptr, 1))
#define m_del_var(obj_type, var_type, var_num, ptr) (m_free(ptr, sizeof(obj_type) + sizeof(var_type) * (var_num)))

void *m_malloc(size_t num_bytes);
void *m_malloc_maybe(size_t num_bytes);
void *m_malloc_with_finaliser(size_t num_bytes);
void *m_malloc0(size_t num_bytes);
void *m_realloc(void *ptr, size_t old_num_bytes, size_t new_num_bytes);
void *m_realloc_maybe(void *ptr, size_t old_num_bytes, size_t new_num_bytes);
void m_free(void *ptr, size_t num_bytes);
void *m_malloc_fail(size_t num_bytes);

#if MICROPY_MEM_STATS
size_t m_get_total_bytes_allocated(void);
size_t m_get_current_bytes_allocated(void);
size_t m_get_peak_bytes_allocated(void);
#endif

/** array helpers ***********************************************/

// get the number of elements in a fixed-size array
#define MP_ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

// align ptr to the nearest multiple of "alignment"
#define MP_ALIGN(ptr, alignment) (void*)(((mp_uint_t)(ptr) + ((alignment) - 1)) & ~((alignment) - 1))

/** unichar / UTF-8 *********************************************/

typedef int unichar; // TODO

unichar utf8_get_char(const byte *s);
const byte *utf8_next_char(const byte *s);

bool unichar_isspace(unichar c);
bool unichar_isalpha(unichar c);
bool unichar_isprint(unichar c);
bool unichar_isdigit(unichar c);
bool unichar_isxdigit(unichar c);
bool unichar_isupper(unichar c);
bool unichar_islower(unichar c);
unichar unichar_tolower(unichar c);
unichar unichar_toupper(unichar c);
mp_uint_t unichar_charlen(const char *str, mp_uint_t len);
#define UTF8_IS_NONASCII(ch) ((ch) & 0x80)
#define UTF8_IS_CONT(ch) (((ch) & 0xC0) == 0x80)

/** variable string *********************************************/

typedef struct _vstr_t {
    size_t alloc;
    size_t len;
    char *buf;
    bool had_error : 1;
    bool fixed_buf : 1;
} vstr_t;

// convenience macro to declare a vstr with a fixed size buffer on the stack
#define VSTR_FIXED(vstr, alloc) vstr_t vstr; char vstr##_buf[(alloc)]; vstr_init_fixed_buf(&vstr, (alloc), vstr##_buf);

void vstr_init(vstr_t *vstr, size_t alloc);
void vstr_init_len(vstr_t *vstr, size_t len);
void vstr_init_fixed_buf(vstr_t *vstr, size_t alloc, char *buf);
void vstr_clear(vstr_t *vstr);
vstr_t *vstr_new(void);
vstr_t *vstr_new_size(size_t alloc);
void vstr_free(vstr_t *vstr);
void vstr_reset(vstr_t *vstr);
bool vstr_had_error(vstr_t *vstr);
char *vstr_str(vstr_t *vstr);
size_t vstr_len(vstr_t *vstr);
void vstr_hint_size(vstr_t *vstr, size_t size);
char *vstr_extend(vstr_t *vstr, size_t size);
char *vstr_add_len(vstr_t *vstr, size_t len);
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

// This is useful for unicode handling. Some CPU archs has
// special instructions for efficient implentation of this
// function (e.g. CLZ on ARM).
// NOTE: this function is unused at the moment
#ifndef count_lead_ones
static inline mp_uint_t count_lead_ones(byte val) {
    mp_uint_t c = 0;
    for (byte mask = 0x80; val & mask; mask >>= 1) {
        c++;
    }
    return c;
}
#endif

#endif // __MICROPY_INCLUDED_PY_MISC_H__
