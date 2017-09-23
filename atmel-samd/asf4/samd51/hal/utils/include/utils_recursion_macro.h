/**
 * \file
 *
 * \brief Recursion macro.
 *
 * Copyright (C) 2014 Atmel Corporation. All rights reserved.
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

#ifndef _UTILS_RECURSION_MACRO_H
#define _UTILS_RECURSION_MACRO_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * \brief Macro recursion
 *
 * \param[in] macro Macro to be repeated recursively
 * \param[in] arg A recursive threshold, building on this to decline by times
 *                defined with parameter n
 * \param[in] n The number of repetitious calls to macro
 */
#define RECURSION_MACRO(macro, arg, n) RECURSION_MACRO_I(macro, arg, n)

/*
 * \brief Second level is needed to get integer literal from "n" if it is
 *        defined as macro
 */
#define RECURSION_MACRO_I(macro, arg, n) RECURSION##n(macro, arg)

#define RECURSION0(macro, arg)
#define RECURSION1(macro, arg) RECURSION0(macro, DEC_VALUE(arg)) macro(arg, 0)
#define RECURSION2(macro, arg) RECURSION1(macro, DEC_VALUE(arg)) macro(arg, 1)
#define RECURSION3(macro, arg) RECURSION2(macro, DEC_VALUE(arg)) macro(arg, 2)
#define RECURSION4(macro, arg) RECURSION3(macro, DEC_VALUE(arg)) macro(arg, 3)
#define RECURSION5(macro, arg) RECURSION4(macro, DEC_VALUE(arg)) macro(arg, 4)

#ifdef __cplusplus
}
#endif

#include <utils_decrement_macro.h>
#endif /* _UTILS_RECURSION_MACRO_H */
