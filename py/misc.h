/*
 * This file is part of the MicroPython project, http://micropython.org/
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
#ifndef MICROPY_INCLUDED_PY_MISC_H
#define MICROPY_INCLUDED_PY_MISC_H

#include "py/mpconfig.h"

// a mini library of useful types and functions

/** types *******************************************************/

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#if __cplusplus // Required on at least one compiler to get ULLONG_MAX
#include <climits>
#else
#include <limits.h>
#endif

typedef unsigned char byte;
typedef unsigned int uint;

#ifndef __has_builtin
#define __has_builtin(x) (0)
#endif
#ifndef __has_feature
// This macro is supported by Clang and gcc>=14
#define __has_feature(x) (0)
#endif


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
#if __cplusplus
#define MP_STATIC_ASSERT(cond) static_assert((cond), #cond)
#elif __GNUC__ >= 5 || __STDC_VERSION__ >= 201112L
#define MP_STATIC_ASSERT(cond) _Static_assert((cond), #cond)
#else
#define MP_STATIC_ASSERT(cond) ((void)sizeof(char[1 - 2 * !(cond)]))
#endif

// In C++ things like comparing extern const pointers are not constant-expressions so cannot be used
// in MP_STATIC_ASSERT. Note that not all possible compiler versions will reject this. Some gcc versions
// do, others only with -Werror=vla, msvc always does.
// The (void) is needed to avoid "left operand of comma operator has no effect [-Werror=unused-value]"
// when using this macro on the left-hand side of a comma.
#if defined(_MSC_VER) || defined(__cplusplus)
#define MP_STATIC_ASSERT_NONCONSTEXPR(cond) ((void)1)
#else
#if __clang__
#pragma GCC diagnostic ignored "-Wgnu-folding-constant"
#endif
#define MP_STATIC_ASSERT_NONCONSTEXPR(cond) ((void)sizeof(char[1 - 2 * !(cond)]))
#endif

// Round-up integer division
#define MP_CEIL_DIVIDE(a, b) (((a) + (b) - 1) / (b))
#define MP_ROUND_DIVIDE(a, b) (((a) + (b) / 2) / (b))

/** memory allocation ******************************************/

// TODO make a lazy m_renew that can increase by a smaller amount than requested (but by at least 1 more element)

#define m_new(type, num) ((type *)(m_malloc(sizeof(type) * (num))))
#define m_new_maybe(type, num) ((type *)(m_malloc_maybe(sizeof(type) * (num))))
#define m_new0(type, num) ((type *)(m_malloc0(sizeof(type) * (num))))
#define m_new_obj(type) (m_new(type, 1))
#define m_new_obj_maybe(type) (m_new_maybe(type, 1))
#define m_new_obj_var(obj_type, var_field, var_type, var_num) ((obj_type *)m_malloc(offsetof(obj_type, var_field) + sizeof(var_type) * (var_num)))
#define m_new_obj_var0(obj_type, var_field, var_type, var_num) ((obj_type *)m_malloc0(offsetof(obj_type, var_field) + sizeof(var_type) * (var_num)))
#define m_new_obj_var_maybe(obj_type, var_field, var_type, var_num) ((obj_type *)m_malloc_maybe(offsetof(obj_type, var_field) + sizeof(var_type) * (var_num)))
#if MICROPY_MALLOC_USES_ALLOCATED_SIZE
#define m_renew(type, ptr, old_num, new_num) ((type *)(m_realloc((ptr), sizeof(type) * (old_num), sizeof(type) * (new_num))))
#define m_renew_maybe(type, ptr, old_num, new_num, allow_move) ((type *)(m_realloc_maybe((ptr), sizeof(type) * (old_num), sizeof(type) * (new_num), (allow_move))))
#define m_del(type, ptr, num) m_free(ptr, sizeof(type) * (num))
#define m_del_var(obj_type, var_field, var_type, var_num, ptr) (m_free(ptr, offsetof(obj_type, var_field) + sizeof(var_type) * (var_num)))
#else
#define m_renew(type, ptr, old_num, new_num) ((type *)(m_realloc((ptr), sizeof(type) * (new_num))))
#define m_renew_maybe(type, ptr, old_num, new_num, allow_move) ((type *)(m_realloc_maybe((ptr), sizeof(type) * (new_num), (allow_move))))
#define m_del(type, ptr, num) ((void)(num), m_free(ptr))
#define m_del_var(obj_type, var_field, var_type, var_num, ptr) ((void)(var_num), m_free(ptr))
#endif
#define m_del_obj(type, ptr) (m_del(type, ptr, 1))

void *m_malloc(size_t num_bytes);
void *m_malloc_maybe(size_t num_bytes);
void *m_malloc_with_finaliser(size_t num_bytes);
void *m_malloc0(size_t num_bytes);
#if MICROPY_MALLOC_USES_ALLOCATED_SIZE
void *m_realloc(void *ptr, size_t old_num_bytes, size_t new_num_bytes);
void *m_realloc_maybe(void *ptr, size_t old_num_bytes, size_t new_num_bytes, bool allow_move);
void m_free(void *ptr, size_t num_bytes);
#else
void *m_realloc(void *ptr, size_t new_num_bytes);
void *m_realloc_maybe(void *ptr, size_t new_num_bytes, bool allow_move);
void m_free(void *ptr);
#endif
MP_NORETURN void m_malloc_fail(size_t num_bytes);

#if MICROPY_TRACKED_ALLOC
// These alloc/free functions track the pointers in a linked list so the GC does not reclaim
// them.  They can be used by code that requires traditional C malloc/free semantics.
void *m_tracked_calloc(size_t nmemb, size_t size);
void m_tracked_free(void *ptr_in);
#endif

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
    bool fixed_buf;
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
#define MP_FLOAT_EXP_OFFSET (1023)
#define MP_FLOAT_FRAC_BITS (52)
typedef uint64_t mp_float_uint_t;
#elif MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
#define MP_FLOAT_EXP_BITS (8)
#define MP_FLOAT_EXP_OFFSET (127)
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

#if MICROPY_FLOAT_FORMAT_IMPL == MICROPY_FLOAT_FORMAT_IMPL_EXACT

#if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
// Exact float conversion requires using internally a bigger sort of floating point
typedef double mp_large_float_t;
#elif MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_DOUBLE
typedef long double mp_large_float_t;
#endif
// Always use a 64 bit mantissa for formatting and parsing
typedef uint64_t mp_large_float_uint_t;

#else // MICROPY_FLOAT_FORMAT_IMPL != MICROPY_FLOAT_FORMAT_IMPL_EXACT

// No bigger floating points
typedef mp_float_t mp_large_float_t;
typedef mp_float_uint_t mp_large_float_uint_t;

#endif

#endif // MICROPY_PY_BUILTINS_FLOAT

/** ROM string compression *************/

#if MICROPY_ROM_TEXT_COMPRESSION

#if MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_NONE
#error "MICROPY_ERROR_REPORTING_NONE requires MICROPY_ROM_TEXT_COMPRESSION disabled"
#endif

#ifdef NO_QSTR

// Compression enabled but doing QSTR extraction.
// So leave MP_COMPRESSED_ROM_TEXT in place for makeqstrdefs.py / makecompresseddata.py to find them.

#else

// Compression enabled and doing a regular build.
// Map MP_COMPRESSED_ROM_TEXT to the compressed strings.

// Force usage of the MP_ERROR_TEXT macro by requiring an opaque type.
typedef struct {
    #if defined(__clang__) || defined(_MSC_VER)
    // Fix "error: empty struct has size 0 in C, size 1 in C++", and the msvc counterpart
    // "C requires that a struct or union have at least one member"
    char dummy;
    #endif
} *mp_rom_error_text_t;

#include <string.h>

inline MP_ALWAYSINLINE const char *MP_COMPRESSED_ROM_TEXT(const char *msg) {
    // "genhdr/compressed.data.h" contains an invocation of the MP_MATCH_COMPRESSED macro for each compressed string.
    // The giant if(strcmp) tree is optimized by the compiler, which turns this into a direct return of the compressed data.
    #define MP_MATCH_COMPRESSED(a, b) if (strcmp(msg, a) == 0) { return b; } else

    // It also contains a single invocation of the MP_COMPRESSED_DATA macro, we don't need that here.
    #define MP_COMPRESSED_DATA(x)

    #include "genhdr/compressed.data.h"

#undef MP_COMPRESSED_DATA
#undef MP_MATCH_COMPRESSED

    return msg;
}

#endif

#else

// Compression not enabled, just make it a no-op.

typedef const char *mp_rom_error_text_t;
#define MP_COMPRESSED_ROM_TEXT(x) x

#endif // MICROPY_ROM_TEXT_COMPRESSION

// Might add more types of compressed text in the future.
// For now, forward directly to MP_COMPRESSED_ROM_TEXT.
#define MP_ERROR_TEXT(x) (mp_rom_error_text_t)MP_COMPRESSED_ROM_TEXT(x)

// Portable implementations of CLZ and CTZ intrinsics
#ifdef _MSC_VER
#include <intrin.h>

static inline uint32_t mp_clz(uint32_t x) {
    unsigned long lz = 0;
    return _BitScanReverse(&lz, x) ? (sizeof(x) * 8 - 1) - lz : 0;
}

static inline uint32_t mp_clzl(unsigned long x) {
    unsigned long lz = 0;
    return _BitScanReverse(&lz, x) ? (sizeof(x) * 8 - 1) - lz : 0;
}

#ifdef _WIN64
static inline uint32_t mp_clzll(unsigned long long x) {
    unsigned long lz = 0;
    return _BitScanReverse64(&lz, x) ? (sizeof(x) * 8 - 1) - lz : 0;
}
#else
// Microsoft don't ship _BitScanReverse64 on Win32, so emulate it
static inline uint32_t mp_clzll(unsigned long long x) {
    unsigned long h = x >> 32;
    return h ? mp_clzl(h) : (mp_clzl((unsigned long)x) + 32);
}
#endif

static inline uint32_t mp_ctz(uint32_t x) {
    unsigned long tz = 0;
    return _BitScanForward(&tz, x) ? tz : 0;
}

// Workaround for 'warning C4127: conditional expression is constant'.
static inline bool mp_check(bool value) {
    return value;
}

static inline uint32_t mp_popcount(uint32_t x) {
    return __popcnt(x);
}
#else // _MSC_VER
#define mp_clz(x) __builtin_clz(x)
#define mp_clzl(x) __builtin_clzl(x)
#define mp_clzll(x) __builtin_clzll(x)
#define mp_ctz(x) __builtin_ctz(x)
#define mp_check(x) (x)
#if __has_builtin(__builtin_popcount)
#define mp_popcount(x) __builtin_popcount(x)
#else
static inline uint32_t mp_popcount(uint32_t x) {
    x = x - ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x + (x >> 4)) & 0x0F0F0F0F;
    return (x * 0x01010101) >> 24;
}
#endif // __has_builtin(__builtin_popcount)
#endif // _MSC_VER

#define MP_FIT_UNSIGNED(bits, value) (((value) & (~0U << (bits))) == 0)
#define MP_FIT_SIGNED(bits, value) \
    (MP_FIT_UNSIGNED(((bits) - 1), (value)) || \
    (((value) & (~0U << ((bits) - 1))) == (~0U << ((bits) - 1))))

// mp_int_t can be larger than long, i.e. Windows 64-bit, nan-box variants
static inline uint32_t mp_clz_mpi(mp_int_t x) {
    #ifdef __XC16__
    mp_uint_t mask = MP_OBJ_WORD_MSBIT_HIGH;
    mp_uint_t zeroes = 0;
    while (mask != 0) {
        if (mask & (mp_uint_t)x) {
            break;
        }
        zeroes++;
        mask >>= 1;
    }
    return zeroes;
    #else
    #if MP_INT_MAX == INT_MAX
    return mp_clz((unsigned)x);
    #elif MP_INT_MAX == LONG_MAX
    return mp_clzl((unsigned long)x);
    #elif MP_INT_MAX == LLONG_MAX
    return mp_clzll((unsigned long long)x);
    #else
    #error Unexpected MP_INT_MAX value
    #endif
    #endif
}

// Overflow-checked operations

// Integer overflow builtins were added to GCC 5, but __has_builtin only in GCC 10
//
// Note that the builtins has a defined result when overflow occurs, whereas the custom
// functions below don't update the result if an overflow would occur (to avoid UB).
#define MP_GCC_HAS_BUILTIN_OVERFLOW (__GNUC__ >= 5)

#if MICROPY_USE_GCC_MUL_OVERFLOW_INTRINSIC

#define mp_mul_ull_overflow __builtin_umulll_overflow
#define mp_mul_ll_overflow __builtin_smulll_overflow
static inline bool mp_mul_mp_int_t_overflow(mp_int_t x, mp_int_t y, mp_int_t *res) {
    // __builtin_mul_overflow is a type-generic function, this inline ensures the argument
    // types are checked to match mp_int_t.
    return __builtin_mul_overflow(x, y, res);
}

#else

bool mp_mul_ll_overflow(long long int x, long long int y, long long int *res);
bool mp_mul_mp_int_t_overflow(mp_int_t x, mp_int_t y, mp_int_t *res);
static inline bool mp_mul_ull_overflow(unsigned long long int x, unsigned long long int y, unsigned long long int *res) {
    if (y > 0 && x > (ULLONG_MAX / y)) {
        return true; // overflow
    }
    *res = x * y;
    return false;
}

#endif

#if __has_builtin(__builtin_saddll_overflow) || MP_GCC_HAS_BUILTIN_OVERFLOW
#define mp_add_ll_overflow __builtin_saddll_overflow
#else
static inline bool mp_add_ll_overflow(long long int lhs, long long int rhs, long long int *res) {
    bool overflow;

    if (rhs > 0) {
        overflow = (lhs > LLONG_MAX - rhs);
    } else {
        overflow = (lhs < LLONG_MIN - rhs);
    }

    if (!overflow) {
        *res = lhs + rhs;
    }

    return overflow;
}
#endif

#if __has_builtin(__builtin_ssubll_overflow) || MP_GCC_HAS_BUILTIN_OVERFLOW
#define mp_sub_ll_overflow __builtin_ssubll_overflow
#else
static inline bool mp_sub_ll_overflow(long long int lhs, long long int rhs, long long int *res) {
    bool overflow;

    if (rhs > 0) {
        overflow = (lhs < LLONG_MIN + rhs);
    } else {
        overflow = (lhs > LLONG_MAX + rhs);
    }

    if (!overflow) {
        *res = lhs - rhs;
    }

    return overflow;
}
#endif


// Helper macros for detecting if sanitizers are enabled
//
// Use sparingly, not for masking issues reported by sanitizers!
//
// Can be detected automatically in Clang and gcc>=14, need to be
// set manually otherwise.
#ifndef MP_UBSAN
#define MP_UBSAN __has_feature(undefined_behavior_sanitizer)
#endif

#ifndef MP_ASAN
#define MP_ASAN __has_feature(address_sanitizer)
#endif

#ifndef MP_SANITIZER_BUILD
#define MP_SANITIZER_BUILD (MP_UBSAN || MP_ASAN)
#endif

#endif // MICROPY_INCLUDED_PY_MISC_H
