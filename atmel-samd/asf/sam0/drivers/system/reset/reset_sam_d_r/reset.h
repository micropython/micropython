/**
 * \file
 *
 * \brief SAM Reset related functionality
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
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
#ifndef RESET_H_INCLUDED
#define RESET_H_INCLUDED

#include <compiler.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup asfdoc_sam0_system_group
 * @{
 */

/**
 * \brief Reset causes of the system.
 *
 * List of possible reset causes of the system.
 */
enum system_reset_cause {
	/** The system was last reset by a software reset */
	SYSTEM_RESET_CAUSE_SOFTWARE       = PM_RCAUSE_SYST,
	/** The system was last reset by the watchdog timer */
	SYSTEM_RESET_CAUSE_WDT            = PM_RCAUSE_WDT,
	/** The system was last reset because the external reset line was pulled low */
	SYSTEM_RESET_CAUSE_EXTERNAL_RESET = PM_RCAUSE_EXT,
	/** The system was last reset by the BOD33 */
	SYSTEM_RESET_CAUSE_BOD33          = PM_RCAUSE_BOD33,
	/** The system was last reset by the BOD12 */
	SYSTEM_RESET_CAUSE_BOD12          = PM_RCAUSE_BOD12,
	/** The system was last reset by the POR (Power on reset) */
	SYSTEM_RESET_CAUSE_POR            = PM_RCAUSE_POR,
};


/**
 * \name Reset Control
 * @{
 */

/**
 * \brief Reset the MCU.
 *
 * Resets the MCU and all associated peripherals and registers, except RTC, all 32KHz sources,
 * WDT (if ALWAYSON is set) and GCLK (if WRTLOCK is set).
 *
 */
static inline void system_reset(void)
{
	NVIC_SystemReset();
}

/**
 * \brief Return the reset cause.
 *
 * Retrieves the cause of the last system reset.
 *
 * \return An enum value indicating the cause of the last system reset.
 */
static inline enum system_reset_cause system_get_reset_cause(void)
{
	return (enum system_reset_cause)PM->RCAUSE.reg;
}

/**
 * @}
 */

/** @} */
#ifdef __cplusplus
}
#endif

#endif /* RESET_H_INCLUDED */
