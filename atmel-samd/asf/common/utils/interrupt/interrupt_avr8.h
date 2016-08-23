/**
 * \file
 *
 * \brief Global interrupt management for 8-bit AVR
 *
 * Copyright (C) 2010-2016 Atmel Corporation. All rights reserved.
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
#ifndef UTILS_INTERRUPT_INTERRUPT_H
#define UTILS_INTERRUPT_INTERRUPT_H

#include <compiler.h>
#include <parts.h>

/**
 * \weakgroup interrupt_group
 *
 * @{
 */

#ifdef ISR_CUSTOM_H
#  include ISR_CUSTOM_H
#else

/**
 * \def ISR
 * \brief Define service routine for specified interrupt vector
 *
 * Usage:
 * \code
	ISR(FOO_vect)
	{
	    ...
	}
\endcode
 *
 * \param vect Interrupt vector name as found in the device header files.
 */
#if defined(__DOXYGEN__)
#  define ISR(vect)
#elif defined(__GNUC__)
#  include <avr/interrupt.h>
#elif defined(__ICCAVR__)
#  define __ISR(x) _Pragma(#x)
#  define ISR(vect) __ISR(vector=vect) __interrupt void handler_##vect(void)
#endif
#endif // ISR_CUSTOM_H

#if XMEGA
/**
 * \brief Initialize interrupt vectors
 * Enables all interrupt levels, with vectors located in the application section
 * and fixed priority scheduling.
 */
#define irq_initialize_vectors() \
	PMIC.CTRL = PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;
#elif MEGA_RF
#define irq_initialize_vectors()
#endif

#ifdef __GNUC__
#  define cpu_irq_enable()     sei()
#  define cpu_irq_disable()    cli()
#else
#  define cpu_irq_enable()     __enable_interrupt()
#  define cpu_irq_disable()    __disable_interrupt()
#endif

typedef uint8_t irqflags_t;

static inline irqflags_t cpu_irq_save(void)
{
	volatile irqflags_t flags = SREG;
	cpu_irq_disable();
	return flags;
}

static inline void cpu_irq_restore(irqflags_t flags)
{
	barrier();
	SREG = flags;
}

static inline bool cpu_irq_is_enabled_flags(irqflags_t flags)
{
#if XMEGA
#  ifdef __GNUC__
	return flags & CPU_I_bm;
#  else
	return flags & I_bm;
#  endif
#elif MEGA || TINY
	return flags & (1 << SREG_I);
#endif
}

#define cpu_irq_is_enabled()             cpu_irq_is_enabled_flags(SREG)

//! @}

/**
 * \weakgroup interrupt_deprecated_group
 * @{
 */
// Deprecated definitions.
#define Enable_global_interrupt()        cpu_irq_enable()
#define Disable_global_interrupt()       cpu_irq_disable()
#define Is_global_interrupt_enabled()    cpu_irq_is_enabled()
//! @}

#endif /* UTILS_INTERRUPT_INTERRUPT_H */
