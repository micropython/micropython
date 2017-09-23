/**
 * \file
 *
 * \brief Different macros.
 *
 * Copyright (C) 2014-2016 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup doc_driver_hal_utils_macro
 *
 * @{
 */

/**
 * \brief Retrieve pointer to parent structure
 */
#define CONTAINER_OF(ptr, type, field_name) ((type *)(((uint8_t *)ptr) - offsetof(type, field_name)))

/**
 * \brief Retrieve array size
 */
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

/**
 * \brief Emit the compiler pragma \a arg.
 *
 * \param[in] arg  The pragma directive as it would appear after \e \#pragma
 *             (i.e. not stringified).
 */
#define COMPILER_PRAGMA(arg) _Pragma(#arg)

/**
 * \def COMPILER_PACK_SET(alignment)
 * \brief Set maximum alignment for subsequent struct and union definitions to \a alignment.
 */
#define COMPILER_PACK_SET(alignment) COMPILER_PRAGMA(pack(alignment))

/**
 * \def COMPILER_PACK_RESET()
 * \brief Set default alignment for subsequent struct and union definitions.
 */
#define COMPILER_PACK_RESET() COMPILER_PRAGMA(pack())

/**
 * \brief Set aligned boundary.
 */
#if defined __GNUC__
#define COMPILER_ALIGNED(a) __attribute__((__aligned__(a)))
#elif defined __ICCARM__
#define COMPILER_ALIGNED(a) COMPILER_PRAGMA(data_alignment = a)
#elif defined __CC_ARM
#define COMPILER_ALIGNED(a) __attribute__((__aligned__(a)))
#endif

/**
 * \brief Flash located data macros
 */
#if defined __GNUC__
#define PROGMEM_DECLARE(type, name) const type name
#define PROGMEM_T const
#define PROGMEM_READ_BYTE(x) *((uint8_t *)(x))
#define PROGMEM_PTR_T const *
#define PROGMEM_STRING_T const uint8_t *
#elif defined __ICCARM__
#define PROGMEM_DECLARE(type, name) const type name
#define PROGMEM_T const
#define PROGMEM_READ_BYTE(x) *((uint8_t *)(x))
#define PROGMEM_PTR_T const *
#define PROGMEM_STRING_T const uint8_t *
#elif defined __CC_ARM
#define PROGMEM_DECLARE(type, name) const type name
#define PROGMEM_T const
#define PROGMEM_READ_BYTE(x) *((uint8_t *)(x))
#define PROGMEM_PTR_T const *
#define PROGMEM_STRING_T const uint8_t *
#endif

/**
 * \brief Optimization
 */
#if defined __GNUC__
#define OPTIMIZE_HIGH __attribute__((optimize(s)))
#elif defined __CC_ARM
#define OPTIMIZE_HIGH _Pragma("O3")
#elif defined __ICCARM__
#define OPTIMIZE_HIGH _Pragma("optimize=high")
#endif

/**
 * \brief RAM located function attribute
 */
#if defined(__CC_ARM) /* Keil ?Vision 4 */
#define RAMFUNC __attribute__((section(".ramfunc")))
#elif defined(__ICCARM__) /* IAR Ewarm 5.41+ */
#define RAMFUNC __ramfunc
#elif defined(__GNUC__) /* GCC CS3 2009q3-68 */
#define RAMFUNC __attribute__((section(".ramfunc")))
#endif

/**
 * \brief No-init section.
 * Place a data object or a function in a no-init section.
 */
#if defined(__CC_ARM)
#define NO_INIT(a) __attribute__((zero_init))
#elif defined(__ICCARM__)
#define NO_INIT(a) __no_init
#elif defined(__GNUC__)
#define NO_INIT(a) __attribute__((section(".no_init")))
#endif

/**
 * \brief Set user-defined section.
 * Place a data object or a function in a user-defined section.
 */
#if defined(__CC_ARM)
#define COMPILER_SECTION(a) __attribute__((__section__(a)))
#elif defined(__ICCARM__)
#define COMPILER_SECTION(a) COMPILER_PRAGMA(location = a)
#elif defined(__GNUC__)
#define COMPILER_SECTION(a) __attribute__((__section__(a)))
#endif

/**
 * \brief Define WEAK attribute.
 */
#if defined(__CC_ARM) /* Keil ?Vision 4 */
#define WEAK __attribute__((weak))
#elif defined(__ICCARM__) /* IAR Ewarm 5.41+ */
#define WEAK __weak
#elif defined(__GNUC__) /* GCC CS3 2009q3-68 */
#define WEAK __attribute__((weak))
#endif

/**
 * \brief Pointer to function
 */
typedef void (*FUNC_PTR)(void);

#define LE_BYTE0(a) ((uint8_t)(a))
#define LE_BYTE1(a) ((uint8_t)((a) >> 8))
#define LE_BYTE2(a) ((uint8_t)((a) >> 16))
#define LE_BYTE3(a) ((uint8_t)((a) >> 24))

#define LE_2_U16(p) ((p)[0] + ((p)[1] << 8))
#define LE_2_U32(p) ((p)[0] + ((p)[1] << 8) + ((p)[2] << 16) + ((p)[3] << 24))

/** \name Zero-Bit Counting
 *
 * Under GCC, __builtin_clz and __builtin_ctz behave like macros when
 * applied to constant expressions (values known at compile time), so they are
 * more optimized than the use of the corresponding assembly instructions and
 * they can be used as constant expressions e.g. to initialize objects having
 * static storage duration, and like the corresponding assembly instructions
 * when applied to non-constant expressions (values unknown at compile time), so
 * they are more optimized than an assembly periphrasis. Hence, clz and ctz
 * ensure a possible and optimized behavior for both constant and non-constant
 * expressions.
 *
 * @{ */

/** \brief Counts the leading zero bits of the given value considered as a 32-bit integer.
 *
 * \param[in] u Value of which to count the leading zero bits.
 *
 * \return The count of leading zero bits in \a u.
 */
#if (defined __GNUC__) || (defined __CC_ARM)
#define clz(u) __builtin_clz(u)
#else
#define clz(u)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    \
	(((u) == 0)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   \
	     ? 32                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     \
	     : ((u) & (1ul << 31)) ? 0 : ((u) & (1ul << 30)) ? 1 : ((u) & (1ul << 29))                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                \
	                                                               ? 2                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            \
	                                                               : ((u) & (1ul << 28))                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          \
	                                                                     ? 3                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      \
	                                                                     : ((u) & (1ul << 27))                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    \
	                                                                           ? 4                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                \
	                                                                           : ((u) & (1ul << 26))                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              \
	                                                                                 ? 5                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          \
	                                                                                 : ((u) & (1ul << 25)) ? 6                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    \
	                                                                                                       : ((u) & (1ul << 24)) ? 7 : ((u) & (1ul << 23)) ? 8 : ((u) & (1ul << 22)) ? 9 : ((u) & (1ul << 21)) ? 10 : ((u) & (1ul << 20)) ? 11 : ((                                                                                                                                                                                                                                                                                                                                               \
	                                                                                                                                                                                                                                                  u)                                                                                                                                                                                                                                                                                                                                          \
	                                                                                                                                                                                                                                              & (1ul                                                                                                                                                                                                                                                                                                                                          \
	                                                                                                                                                                                                                                                 << 19))                                                                                                                                                                                                                                                                                                                                      \
	                                                                                                                                                                                                                                                 ? 12                                                                                                                                                                                                                                                                                                                                         \
	                                                                                                                                                                                                                                                 : ((u)                                                                                                                                                                                                                                                                                                                                       \
	                                                                                                                                                                                                                                                    & (1ul                                                                                                                                                                                                                                                                                                                                    \
	                                                                                                                                                                                                                                                       << 18))                                                                                                                                                                                                                                                                                                                                \
	                                                                                                                                                                                                                                                       ? 13                                                                                                                                                                                                                                                                                                                                   \
	                                                                                                                                                                                                                                                       : (                                                                                                                                                                                                                                                                                                                                    \
	                                                                                                                                                                                                                                                             (u)                                                                                                                                                                                                                                                                                                                              \
	                                                                                                                                                                                                                                                             & (1ul                                                                                                                                                                                                                                                                                                                           \
	                                                                                                                                                                                                                                                                << 17))                                                                                                                                                                                                                                                                                                                       \
	                                                                                                                                                                                                                                                             ? 14                                                                                                                                                                                                                                                                                                                             \
	                                                                                                                                                                                                                                                             : ((u) & (1ul << 16)) ? 15                                                                                                                                                                                                                                                                                                       \
	                                                                                                                                                                                                                                                                                   : ((u)                                                                                                                                                                                                                                                                                                     \
	                                                                                                                                                                                                                                                                                      & (1ul                                                                                                                                                                                                                                                                                                  \
	                                                                                                                                                                                                                                                                                         << 15))                                                                                                                                                                                                                                                                                              \
	                                                                                                                                                                                                                                                                                         ? 16                                                                                                                                                                                                                                                                                                 \
	                                                                                                                                                                                                                                                                                         : ((u) & (1ul << 14)) ? 17                                                                                                                                                                                                                                                                           \
	                                                                                                                                                                                                                                                                                                               : ((u) & (1ul << 13)) ? 18 : ((u) & (1ul << 12)) ? 19 : ((u) & (1ul << 11)) ? 20                                                                                                                                                                                               \
	                                                                                                                                                                                                                                                                                                                                                                                           : ((u)                                                                                                                                                                                             \
	                                                                                                                                                                                                                                                                                                                                                                                              & (1ul                                                                                                                                                                                          \
	                                                                                                                                                                                                                                                                                                                                                                                                 << 10))                                                                                                                                                                                      \
	                                                                                                                                                                                                                                                                                                                                                                                                 ? 21                                                                                                                                                                                         \
	                                                                                                                                                                                                                                                                                                                                                                                                 : ((u) & (1ul << 9)) ? 22                                                                                                                                                                    \
	                                                                                                                                                                                                                                                                                                                                                                                                                      : ((u) & (1ul << 8)) ? 23 : ((u)                                                                                                                                        \
	                                                                                                                                                                                                                                                                                                                                                                                                                                                   & (1ul                                                                                                                                     \
	                                                                                                                                                                                                                                                                                                                                                                                                                                                      << 7))                                                                                                                                  \
	                                                                                                                                                                                                                                                                                                                                                                                                                                                      ? 24                                                                                                                                    \
	                                                                                                                                                                                                                                                                                                                                                                                                                                                      : ((u) & (1ul << 6)) ? 25 : ((u) & (1ul << 5)) ? 26 : ((u) & (1ul << 4)) ? 27 : ((u) & (1ul << 3)) ? 28 : ((u)                          \
	                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 & (1ul                       \
	                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    << 2))                    \
	                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    ? 29                      \
	                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    : ((u) & (1ul << 1)) ? 30 \
	                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         : 31)
#endif

/** \brief Counts the trailing zero bits of the given value considered as a 32-bit integer.
 *
 * \param[in] u Value of which to count the trailing zero bits.
 *
 * \return The count of trailing zero bits in \a u.
 */
#if (defined __GNUC__) || (defined __CC_ARM)
#define ctz(u) __builtin_ctz(u)
#else
#define ctz(u)                                                                                                                                                                                                                                                                                                                                                                                                                        \
	(                                                                                                                                                                                                                                                                                                                                                                                                                                 \
	    (u) & (1ul << 0)                                                                                                                                                                                                                                                                                                                                                                                                              \
	        ? 0                                                                                                                                                                                                                                                                                                                                                                                                                       \
	        : (u) & (1ul << 1)                                                                                                                                                                                                                                                                                                                                                                                                        \
	              ? 1                                                                                                                                                                                                                                                                                                                                                                                                                 \
	              : (u) & (1ul << 2)                                                                                                                                                                                                                                                                                                                                                                                                  \
	                    ? 2                                                                                                                                                                                                                                                                                                                                                                                                           \
	                    : (u) & (1ul << 3)                                                                                                                                                                                                                                                                                                                                                                                            \
	                          ? 3                                                                                                                                                                                                                                                                                                                                                                                                     \
	                          : (u) & (1ul << 4)                                                                                                                                                                                                                                                                                                                                                                                      \
	                                ? 4                                                                                                                                                                                                                                                                                                                                                                                               \
	                                : (u) & (1ul << 5)                                                                                                                                                                                                                                                                                                                                                                                \
	                                      ? 5                                                                                                                                                                                                                                                                                                                                                                                         \
	                                      : (u) & (1ul << 6)                                                                                                                                                                                                                                                                                                                                                                          \
	                                            ? 6                                                                                                                                                                                                                                                                                                                                                                                   \
	                                            : (u) & (1ul << 7)                                                                                                                                                                                                                                                                                                                                                                    \
	                                                  ? 7                                                                                                                                                                                                                                                                                                                                                                             \
	                                                  : (u) & (1ul << 8)                                                                                                                                                                                                                                                                                                                                                              \
	                                                        ? 8                                                                                                                                                                                                                                                                                                                                                                       \
	                                                        : (u) & (1ul << 9) ? 9                                                                                                                                                                                                                                                                                                                                                    \
	                                                                           : (u) & (1ul << 10)                                                                                                                                                                                                                                                                                                                                    \
	                                                                                 ? 10                                                                                                                                                                                                                                                                                                                                             \
	                                                                                 : (u) & (1ul << 11)                                                                                                                                                                                                                                                                                                                              \
	                                                                                       ? 11                                                                                                                                                                                                                                                                                                                                       \
	                                                                                       : (u) & (1ul << 12)                                                                                                                                                                                                                                                                                                                        \
	                                                                                             ? 12                                                                                                                                                                                                                                                                                                                                 \
	                                                                                             : (u) & (1ul << 13)                                                                                                                                                                                                                                                                                                                  \
	                                                                                                   ? 13                                                                                                                                                                                                                                                                                                                           \
	                                                                                                   : (u) & (1ul                                                                                                                                                                                                                                                                                                                   \
	                                                                                                            << 14)                                                                                                                                                                                                                                                                                                                \
	                                                                                                         ? 14                                                                                                                                                                                                                                                                                                                     \
	                                                                                                         : (u) & (1ul                                                                                                                                                                                                                                                                                                             \
	                                                                                                                  << 15)                                                                                                                                                                                                                                                                                                          \
	                                                                                                               ? 15                                                                                                                                                                                                                                                                                                               \
	                                                                                                               : (u) & (1ul << 16) ? 16 : (u) & (1ul << 17) ? 17 : (u) & (1ul << 18) ? 18 : (u) & (1ul << 19) ? 19 : (u) & (1ul << 20) ? 20 : (u)                                                                                                                                                                                 \
	                                                                                                                                                                                                                                                      & (1ul                                                                                                                                                                      \
	                                                                                                                                                                                                                                                         << 21)                                                                                                                                                                   \
	                                                                                                                                                                                                                                                  ? 21                                                                                                                                                                            \
	                                                                                                                                                                                                                                                  : (u) & (1ul << 22) ? 22 : (u)                                                                                                                                                  \
	                                                                                                                                                                                                                                                                                     & (1ul << 23)                                                                                                                                \
	                                                                                                                                                                                                                                                                                 ? 23                                                                                                                                             \
	                                                                                                                                                                                                                                                                                 : (u) & (1ul << 24) ? 24                                                                                                                         \
	                                                                                                                                                                                                                                                                                                     : (u) & (1ul << 25) ? 25                                                                                                     \
	                                                                                                                                                                                                                                                                                                                         : (u) & (1ul << 26) ? 26 : (u) & (1ul                                                                    \
	                                                                                                                                                                                                                                                                                                                                                           << 27)                                                                 \
	                                                                                                                                                                                                                                                                                                                                                        ? 27                                                                      \
	                                                                                                                                                                                                                                                                                                                                                        : (u) & (1ul                                                              \
	                                                                                                                                                                                                                                                                                                                                                                 << 28)                                                           \
	                                                                                                                                                                                                                                                                                                                                                              ? 28                                                                \
	                                                                                                                                                                                                                                                                                                                                                              : (u) & (1ul << 29) ? 29                                            \
	                                                                                                                                                                                                                                                                                                                                                                                  : (u) & (1ul << 30) ? 30 : (                    \
	                                                                                                                                                                                                                                                                                                                                                                                                                 u) & (1ul << 31) \
	                                                                                                                                                                                                                                                                                                                                                                                                                 ? 31             \
	                                                                                                                                                                                                                                                                                                                                                                                                                 : 32)
#endif
/** @} */

/**
 * \brief Counts the number of bits in a mask (no more than 32 bits)
 * \param[in] mask Mask of which to count the bits.
 */
#define size_of_mask(mask) (32 - clz(mask) - ctz(mask))

/**
 * \brief Retrieve the start position of bits mask (no more than 32 bits)
 * \param[in] mask Mask of which to retrieve the start position.
 */
#define pos_of_mask(mask) ctz(mask)

/**
 * \brief Return division result of a/b and round up the result to the closest
 *        number divisible by "b"
 */
#define round_up(a, b) (((a)-1) / (b) + 1)

/**
 * \brief Get the minimum of x and y
 */
#define min(x, y) ((x) > (y) ? (y) : (x))

/**
 * \brief Get the maximum of x and y
 */
#define max(x, y) ((x) > (y) ? (x) : (y))

/**@}*/

#ifdef __cplusplus
}
#endif
#endif /* UTILS_H_INCLUDED */
