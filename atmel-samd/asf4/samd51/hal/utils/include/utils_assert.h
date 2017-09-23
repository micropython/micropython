/**
 * \file
 *
 * \brief Asserts related functionality.
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

#ifndef _ASSERT_H_INCLUDED
#define _ASSERT_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <compiler.h>

#ifndef USE_SIMPLE_ASSERT
//# define USE_SIMPLE_ASSERT
#endif

/**
 * \brief Assert macro
 *
 * This macro is used to throw asserts. It can be mapped to different function
 * based on debug level.
 *
 * \param[in] condition A condition to be checked;
 *                      assert is thrown if the given condition is false
 */
#define ASSERT(condition) ASSERT_IMPL((condition), __FILE__, __LINE__)

#ifdef DEBUG

#ifdef USE_SIMPLE_ASSERT
#define ASSERT_IMPL(condition, file, line)                                                                             \
	if (!(condition))                                                                                                  \
		__asm("BKPT #0");
#else
#define ASSERT_IMPL(condition, file, line) asf_assert((condition), file, line)
#endif

#else /* DEBUG */

#ifdef USE_SIMPLE_ASSERT
#define ASSERT_IMPL(condition, file, line) ((void)0)
#else
#define ASSERT_IMPL(condition, file, line) ((void)0)
#endif

#endif /* DEBUG */

/**
 * \brief Assert function
 *
 * This function is used to throw asserts.
 *
 * \param[in] condition A condition to be checked; assert is thrown if the given
 *                      condition is false
 * \param[in] file File name
 * \param[in] line Line number
 */
void asf_assert(const bool condition, const char *const file, const int line);

#ifdef __cplusplus
}
#endif
#endif /* _ASSERT_H_INCLUDED */
