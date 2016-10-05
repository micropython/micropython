/**
 * \file
 *
 * \brief SAM System related functionality
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

#include <system.h>

/**
 * \internal
 * Dummy initialization function, used as a weak alias target for the various
 * init functions called by \ref system_init().
 */
void _system_dummy_init(void);
void _system_dummy_init(void)
{
	return;
}

#if !defined(__DOXYGEN__)
#  if defined(__GNUC__)
void system_clock_init(void) WEAK __attribute__((alias("_system_dummy_init")));
void system_board_init(void) WEAK __attribute__((alias("_system_dummy_init")));
void _system_events_init(void) WEAK __attribute__((alias("_system_dummy_init")));
void _system_extint_init(void) WEAK __attribute__((alias("_system_dummy_init")));
void _system_divas_init(void) WEAK __attribute__((alias("_system_dummy_init")));
#  elif defined(__ICCARM__)
void system_clock_init(void);
void system_board_init(void);
void _system_events_init(void);
void _system_extint_init(void);
void _system_divas_init(void);
#    pragma weak system_clock_init=_system_dummy_init
#    pragma weak system_board_init=_system_dummy_init
#    pragma weak _system_events_init=_system_dummy_init
#    pragma weak _system_extint_init=_system_dummy_init
#    pragma weak _system_divas_init=_system_dummy_init
#  endif
#endif

/**
 * \brief Initialize system
 *
 * This function will call the various initialization functions within the
 * system namespace. If a given optional system module is not available, the
 * associated call will effectively be a NOP (No Operation).
 *
 * Currently the following initialization functions are supported:
 *  - System clock initialization (via the SYSTEM CLOCK sub-module)
 *  - Board hardware initialization (via the Board module)
 *  - Event system driver initialization (via the EVSYS module)
 *  - External Interrupt driver initialization (via the EXTINT module)
 */
void system_init(void)
{
	/* Configure GCLK and clock sources according to conf_clocks.h */
	system_clock_init();

	/* Initialize board hardware */
	system_board_init();

	/* Initialize EVSYS hardware */
	_system_events_init();

	/* Initialize External hardware */
	_system_extint_init();
	
	/* Initialize DIVAS hardware */
	_system_divas_init();
}

