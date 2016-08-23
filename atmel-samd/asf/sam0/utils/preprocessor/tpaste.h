/**
 * \file
 *
 * \brief Preprocessor token pasting utils.
 *
 * Copyright (C) 2012-2015 Atmel Corporation. All rights reserved.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef _TPASTE_H_
#define _TPASTE_H_

/**
 * \defgroup group_sam0_utils_tpaste Preprocessor - Token Paste
 *
 * \ingroup group_sam0_utils
 *
 * @{
 */

/** \name Token Paste
 *
 * Paste N preprocessing tokens together, these tokens being allowed to be \#defined.
 *
 * May be used only within macros with the tokens passed as arguments if the tokens are \#defined.
 *
 * For example, writing TPASTE2(U, WIDTH) within a macro \#defined by
 * UTYPE(WIDTH) and invoked as UTYPE(UL_WIDTH) with UL_WIDTH \#defined as 32 is
 * equivalent to writing U32.
 *
 * @{ */
#define TPASTE2( a, b)                            a##b
#define TPASTE3( a, b, c)                         a##b##c
#define TPASTE4( a, b, c, d)                      a##b##c##d
#define TPASTE5( a, b, c, d, e)                   a##b##c##d##e
#define TPASTE6( a, b, c, d, e, f)                a##b##c##d##e##f
#define TPASTE7( a, b, c, d, e, f, g)             a##b##c##d##e##f##g
#define TPASTE8( a, b, c, d, e, f, g, h)          a##b##c##d##e##f##g##h
#define TPASTE9( a, b, c, d, e, f, g, h, i)       a##b##c##d##e##f##g##h##i
#define TPASTE10(a, b, c, d, e, f, g, h, i, j)    a##b##c##d##e##f##g##h##i##j
/** @} */

/** \name Absolute Token Paste
 *
 * Paste N preprocessing tokens together, these tokens being allowed to be \#defined.
 *
 * No restriction of use if the tokens are \#defined.
 *
 * For example, writing ATPASTE2(U, UL_WIDTH) anywhere with UL_WIDTH \#defined
 * as 32 is equivalent to writing U32.
 *
 * @{ */
#define ATPASTE2( a, b)                           TPASTE2( a, b)
#define ATPASTE3( a, b, c)                        TPASTE3( a, b, c)
#define ATPASTE4( a, b, c, d)                     TPASTE4( a, b, c, d)
#define ATPASTE5( a, b, c, d, e)                  TPASTE5( a, b, c, d, e)
#define ATPASTE6( a, b, c, d, e, f)               TPASTE6( a, b, c, d, e, f)
#define ATPASTE7( a, b, c, d, e, f, g)            TPASTE7( a, b, c, d, e, f, g)
#define ATPASTE8( a, b, c, d, e, f, g, h)         TPASTE8( a, b, c, d, e, f, g, h)
#define ATPASTE9( a, b, c, d, e, f, g, h, i)      TPASTE9( a, b, c, d, e, f, g, h, i)
#define ATPASTE10(a, b, c, d, e, f, g, h, i, j)   TPASTE10(a, b, c, d, e, f, g, h, i, j)
/** @} */

/** @} */

#endif  // _TPASTE_H_
