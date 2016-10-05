/**
 * \file
 *
 * \brief SAM3/SAM4 Sleep manager implementation.
 *
 * Copyright (c) 2012-2016 Atmel Corporation. All rights reserved.
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

#ifndef SAM_SLEEPMGR_INCLUDED
#define SAM_SLEEPMGR_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <compiler.h>
#include <conf_sleepmgr.h>
#include <sleep.h>
#include <interrupt.h>

/**
 * \weakgroup sleepmgr_group
 * @{
 */
enum sleepmgr_mode {
	//! Active mode.
	SLEEPMGR_ACTIVE = 0,
	/*! WFE sleep mode.
	 *  Potential Wake Up sources:
	 *  fast startup events (USB, RTC, RTT, WKUPs),
	 *  interrupt, and events. */
	SLEEPMGR_SLEEP_WFE,
	/*! WFI sleep mode.
	 * Potential Wake Up sources: fast startup events and interrupt. */
	SLEEPMGR_SLEEP_WFI,
	/*! Wait mode, wakeup fast (in 3ms).
	 *  XTAL is not disabled when sleep.
	 *  Potential Wake Up sources: fast startup events */
	SLEEPMGR_WAIT_FAST,
	/*! Wait mode.
	 *  Potential Wake Up sources: fast startup events */
	SLEEPMGR_WAIT,
#if (!(SAMG51 || SAMG53 || SAMG54))
	//! Backup mode. Potential Wake Up sources: WKUPs, SM, RTT, RTC.
	SLEEPMGR_BACKUP,
#endif
	SLEEPMGR_NR_OF_MODES,
};

/**
 * \internal
 * \name Internal arrays
 * @{
 */
#if defined(CONFIG_SLEEPMGR_ENABLE) || defined(__DOXYGEN__)
//! Sleep mode lock counters
extern uint8_t sleepmgr_locks[];
#endif /* CONFIG_SLEEPMGR_ENABLE */
//! @}


static inline void sleepmgr_sleep(const enum sleepmgr_mode sleep_mode)
{
	Assert(sleep_mode != SLEEPMGR_ACTIVE);
#ifdef CONFIG_SLEEPMGR_ENABLE
	cpu_irq_disable();

	// Atomically enable the global interrupts and enter the sleep mode.
	pmc_sleep(sleep_mode);
#else
	UNUSED(sleep_mode);
	cpu_irq_enable();
#endif /* CONFIG_SLEEPMGR_ENABLE */

}

//! @}

#ifdef __cplusplus
}
#endif

#endif /* SAM_SLEEPMGR_INCLUDED */
