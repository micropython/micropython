/**
 * \file
 *
 * \brief Global interrupt management for SAM D20, SAM3 and SAM4 (NVIC based)
 *
 * Copyright (c) 2012-2015 Atmel Corporation. All rights reserved.
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

#include "interrupt_sam_nvic.h"

#if !defined(__DOXYGEN__)
/* Deprecated - global flag to determine the global interrupt state. Required by
 * QTouch library, however new applications should use cpu_irq_is_enabled()
 * which probes the true global interrupt state from the CPU special registers.
 */
volatile bool g_interrupt_enabled = true;
#endif

void cpu_irq_enter_critical(void)
{
	if (cpu_irq_critical_section_counter == 0) {
		if (cpu_irq_is_enabled()) {
			cpu_irq_disable();
			cpu_irq_prev_interrupt_state = true;
		} else {
			/* Make sure the to save the prev state as false */
			cpu_irq_prev_interrupt_state = false;
		}

	}

	cpu_irq_critical_section_counter++;
}

void cpu_irq_leave_critical(void)
{
	/* Check if the user is trying to leave a critical section when not in a critical section */
	Assert(cpu_irq_critical_section_counter > 0);

	cpu_irq_critical_section_counter--;

	/* Only enable global interrupts when the counter reaches 0 and the state of the global interrupt flag
	   was enabled when entering critical state */
	if ((cpu_irq_critical_section_counter == 0) && (cpu_irq_prev_interrupt_state)) {
		cpu_irq_enable();
	}
}

