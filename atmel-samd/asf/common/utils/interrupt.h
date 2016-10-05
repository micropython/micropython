/**
 * \file
 *
 * \brief Global interrupt management for 8- and 32-bit AVR
 *
 * Copyright (c) 2010-2016 Atmel Corporation. All rights reserved.
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
#ifndef UTILS_INTERRUPT_H
#define UTILS_INTERRUPT_H

#include <parts.h>

#if XMEGA || MEGA
#  include "interrupt/interrupt_avr8.h"
#elif UC3
#  include "interrupt/interrupt_avr32.h"
#elif SAM || SAMB
#  include "interrupt/interrupt_sam_nvic.h"
#else
#  error Unsupported device.
#endif

/**
 * \defgroup interrupt_group Global interrupt management
 *
 * This is a driver for global enabling and disabling of interrupts.
 *
 * @{
 */

#if defined(__DOXYGEN__)
/**
 * \def CONFIG_INTERRUPT_FORCE_INTC
 * \brief Force usage of the ASF INTC driver
 *
 * Predefine this symbol when preprocessing to force the use of the ASF INTC driver.
 * This is useful to ensure compatibility across compilers and shall be used only when required
 * by the application needs.
 */
#  define CONFIG_INTERRUPT_FORCE_INTC
#endif

//! \name Global interrupt flags
//@{
/**
 * \typedef irqflags_t
 * \brief Type used for holding state of interrupt flag
 */

/**
 * \def cpu_irq_enable
 * \brief Enable interrupts globally
 */

/**
 * \def cpu_irq_disable
 * \brief Disable interrupts globally
 */

/**
 * \fn irqflags_t cpu_irq_save(void)
 * \brief Get and clear the global interrupt flags
 *
 * Use in conjunction with \ref cpu_irq_restore.
 *
 * \return Current state of interrupt flags.
 *
 * \note This function leaves interrupts disabled.
 */

/**
 * \fn void cpu_irq_restore(irqflags_t flags)
 * \brief Restore global interrupt flags
 *
 * Use in conjunction with \ref cpu_irq_save.
 *
 * \param flags State to set interrupt flag to.
 */

/**
 * \fn bool cpu_irq_is_enabled_flags(irqflags_t flags)
 * \brief Check if interrupts are globally enabled in supplied flags
 *
 * \param flags Currents state of interrupt flags.
 *
 * \return True if interrupts are enabled.
 */

/**
 * \def cpu_irq_is_enabled
 * \brief Check if interrupts are globally enabled
 *
 * \return True if interrupts are enabled.
 */
//@}

//! @}

/**
 * \ingroup interrupt_group
 * \defgroup interrupt_deprecated_group Deprecated interrupt definitions
 */

#endif /* UTILS_INTERRUPT_H */
