/**
 * \file
 *
 * \brief Time measure related functionality declaration.
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

#ifndef _HPL_TIME_MEASURE_H_INCLUDED
#define _HPL_TIME_MEASURE_H_INCLUDED

/**
 * \addtogroup HPL Time measure
 *
 * \section hpl_time_measure_rev Revision History
 * - v1.0.0 Initial Release
 *
 *@{
 */

#include <compiler.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief System time type
 */
typedef uint32_t system_time_t;

/**
 * \name HPL functions
 */
//@{
/**
 * \brief Initialize system time module
 *
 * \param[in] hw The pointer to hardware instance to initialize
 */
void _system_time_init(void *const hw);

/**
 * \brief Deinitialize system time module
 *
 * \param[in] hw The pointer to hardware instance to initialize
 */
void _system_time_deinit(void *const hw);

/**
 * \brief Get system time
 *
 * \param[in] hw The pointer to hardware instance to initialize
 */
system_time_t _system_time_get(const void *const hw);

/**
 * \brief Get maximum possible system time
 *
 * \param[in] hw The pointer to hardware instance to initialize
 */
system_time_t _system_time_get_max_time_value(const void *const hw);
//@}

#ifdef __cplusplus
}
#endif
/**@}*/
#endif /* _HPL_TIME_MEASURE_H_INCLUDED */
