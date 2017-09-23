/**
 * \file
 *
 * \brief USB Include Header Files.
 *
 * This file contains the USB definitions and data structures provided by the
 * USB 2.0 specification.
 *
 * Copyright (C) 2015 - 2017 Atmel Corporation. All rights reserved.
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 * Atmel AVR product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

#ifndef _USB_INCLUDES_H_
#define _USB_INCLUDES_H_

#ifdef USB_USER_INCLUDES

#include "usb_user_includes.h"

#else
/* Include START headers */
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <utils.h>
#include <utils_list.h>
#include <utils_assert.h>
#include <hal_atomic.h>

typedef uint16_t le16_t;
typedef uint32_t le32_t;
typedef uint16_t be16_t;
typedef uint32_t be32_t;

#if (defined __GNUC__) || (defined __CC_ARM)
#define is_constant(exp) __builtin_constant_p(exp)
#else
#define is_constant(exp) (0)
#endif

/*! \brief Toggles the endianism of \a u16 (by swapping its bytes).
 *
 * \param u16 U16 of which to toggle the endianism.
 *
 * \return Value resulting from \a u16 with toggled endianism.
 *
 * \note More optimized if only used with values known at compile time.
 */
#define swap_u16(u16) ((uint16_t)(((uint16_t)(u16) >> 8) | ((uint16_t)(u16) << 8)))

/*! \brief Toggles the endianism of \a u32 (by swapping its bytes).
 *
 * \param u32 U32 of which to toggle the endianism.
 *
 * \return Value resulting from \a u32 with toggled endianism.
 *
 * \note More optimized if only used with values known at compile time.
 */
#if (defined __GNUC__)
#define swap_u32(u32)                                                                                                  \
	(is_constant(u32)                                                                                                  \
	     ? ((uint32_t)(((uint32_t)swap_u16((uint32_t)(u32) >> 16)) | ((uint32_t)swap_u16((uint32_t)(u32)) << 16)))     \
	     : ((uint32_t)__builtin_bswap32((uint32_t)(u32))))
#else
#define swap_u32(u32)                                                                                                  \
	((uint32_t)(((uint32_t)swap_u16((uint32_t)(u32) >> 16)) | ((uint32_t)swap_u16((uint32_t)(u32)) << 16)))
#endif

/** Get a value from/to LE16 data */
#define LE16(x) (x)
/** Get a value from/to LE32 data */
#define LE32(x) (x)
/** Get a value from/to BE16 data */
#define BE16(x) swap_u16(x)
/** Get a value from/to BE32 data */
#define BE32(x) swap_u32(x)

/** Get byte 0 for BE 16-bit value */
#define BE16B0(a) ((uint8_t)((a) >> 8))
/** Get byte 1 for BE 16-bit value */
#define BE16B1(a) ((uint8_t)((a) >> 0))

/** Get byte 0 for BE 32-bit value */
#define BE32B0(a) ((uint8_t)((a) >> 24))
/** Get byte 1 for BE 32-bit value */
#define BE32B1(a) ((uint8_t)((a) >> 16))
/** Get byte 2 for BE 32-bit value */
#define BE32B2(a) ((uint8_t)((a) >> 8))
/** Get byte 3 for BE 32-bit value */
#define BE32B3(a) ((uint8_t)((a) >> 0))

/** Get byte 0 for LE 16-bit value */
#define LE16B0(a) ((uint8_t)((a) >> 0))
/** Get byte 1 for LE 16-bit value */
#define LE16B1(a) ((uint8_t)((a) >> 8))

/** Get byte 0 for LE 32-bit value */
#define LE32B0(a) ((uint8_t)((a) >> 0))
/** Get byte 1 for LE 32-bit value */
#define LE32B1(a) ((uint8_t)((a) >> 8))
/** Get byte 2 for LE 32-bit value */
#define LE32B2(a) ((uint8_t)((a) >> 16))
/** Get byte 3 for LE 32-bit value */
#define LE32B3(a) ((uint8_t)((a) >> 24))

#endif /* USB_USER_INCLUDES */

#endif /* _USB_INCLUDES_H_ */
