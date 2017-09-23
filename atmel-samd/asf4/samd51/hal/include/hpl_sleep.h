/**
 * \file
 *
 * \brief Sleep related functionality declaration.
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

#ifndef _HPL_SLEEP_H_INCLUDED
#define _HPL_SLEEP_H_INCLUDED

/**
 * \addtogroup HPL Sleep
 *
 * \section hpl_sleep_rev Revision History
 * - v1.0.0 Initial Release
 *
 *@{
 */

#ifndef _UNIT_TEST_
#include <compiler.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \name HPL functions
 */
//@{
/**
 * \brief Set the sleep mode for the device
 *
 * This function sets the sleep mode for the device.
 * For an overview of which systems are disabled in sleep for the different
 * sleep modes see datasheet.
 *
 * \param[in] mode Sleep mode to use
 *
 * \return the status of a sleep request
 * \retval -1 The requested sleep mode was invalid
 * \retval  0 The operation completed successfully, sleep mode is set
 */
int32_t _set_sleep_mode(const uint8_t mode);

/**
 * \brief Reset MCU
 */
void _reset_mcu(void);

/**
 * \brief Put MCU to sleep
 */
void _go_to_sleep(void);
//@}

#ifdef __cplusplus
}
#endif
/**@}*/
#endif /* _HPL_SLEEP_H_INCLUDED */
