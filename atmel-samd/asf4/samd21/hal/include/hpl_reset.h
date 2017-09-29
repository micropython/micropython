/**
 * \file
 *
 * \brief Reset related functionality declaration.
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

#ifndef _HPL_RESET_H_INCLUDED
#define _HPL_RESET_H_INCLUDED

/**
 * \addtogroup HPL Reset
 *
 * \section hpl_reset_rev Revision History
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
 * \brief Reset reason enumeration
 *
 * The list of possible reset reasons.
 */
enum reset_reason {
	RESET_REASON_POR   = 1,
	RESET_REASON_BOD12 = 2,
	RESET_REASON_BOD33 = 4,
	RESET_REASON_EXT   = 16,
	RESET_REASON_WDT   = 32,
	RESET_REASON_SYST  = 64,
};

/**
 * \name HPL functions
 */
//@{
/**
 * \brief Retrieve the reset reason
 *
 * Retrieves the reset reason of the last MCU reset.
 *
 *\return An enum value indicating the reason of the last reset.
 */
enum reset_reason _get_reset_reason(void);

/**
 * \brief Reset MCU
 */
void _reset_mcu(void);
//@}

#ifdef __cplusplus
}
#endif
/**@}*/
#endif /* _HPL_RESET_H_INCLUDED */
