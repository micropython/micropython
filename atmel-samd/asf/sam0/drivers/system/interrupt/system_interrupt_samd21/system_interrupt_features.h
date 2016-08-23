/**
 * \file
 *
 * \brief SAM D21 System Interrupt Driver
 *
 * Copyright (C) 2013-2015 Atmel Corporation. All rights reserved.
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

#ifndef SYSTEM_INTERRUPT_FEATURES_H_INCLUDED
#define SYSTEM_INTERRUPT_FEATURES_H_INCLUDED

#if !defined(__DOXYGEN__)

/* Generates a interrupt vector table enum list entry for a given module type
   and index (e.g. "SYSTEM_INTERRUPT_MODULE_TC0 = TC0_IRQn,"). */
#  define _MODULE_IRQn(n, module) \
		SYSTEM_INTERRUPT_MODULE_##module##n = module##n##_IRQn,

/* Generates interrupt vector table enum list entries for all instances of a
   given module type on the selected device. */
#  define _SYSTEM_INTERRUPT_MODULES(name) \
		MREPEAT(name##_INST_NUM, _MODULE_IRQn, name)

#  define _SYSTEM_INTERRUPT_IPSR_MASK              0x0000003f
#  define _SYSTEM_INTERRUPT_PRIORITY_MASK          0x00000003

#  define _SYSTEM_INTERRUPT_EXTERNAL_VECTOR_START  0

#  define _SYSTEM_INTERRUPT_SYSTICK_PRI_POS        30
#endif

/**
 * \addtogroup asfdoc_sam0_system_interrupt_group
 * @{
 */

/**
 * \brief Table of possible system interrupt/exception vector numbers.
 *
 * Table of all possible interrupt and exception vector indexes within the
 * SAM D21 device. Check peripherals configuration in SAM D21 datasheet for
 * available vector index for specific device.
 *
 */
#if defined(__DOXYGEN__)
/** \note The actual enumeration name is "system_interrupt_vector". */
enum system_interrupt_vector_samd21 {
#else
enum system_interrupt_vector {
#endif
	/** Interrupt vector index for a NMI interrupt */
	SYSTEM_INTERRUPT_NON_MASKABLE      = NonMaskableInt_IRQn,
	/** Interrupt vector index for a Hard Fault memory access exception */
	SYSTEM_INTERRUPT_HARD_FAULT        = HardFault_IRQn,
	/** Interrupt vector index for a Supervisor Call exception */
	SYSTEM_INTERRUPT_SV_CALL           = SVCall_IRQn,
	/** Interrupt vector index for a Pending Supervisor interrupt */
	SYSTEM_INTERRUPT_PENDING_SV        = PendSV_IRQn,
	/** Interrupt vector index for a System Tick interrupt */
	SYSTEM_INTERRUPT_SYSTICK           = SysTick_IRQn,

	/** Interrupt vector index for a Power Manager peripheral interrupt */
	SYSTEM_INTERRUPT_MODULE_PM         = PM_IRQn,
	/** Interrupt vector index for a System Control peripheral interrupt */
	SYSTEM_INTERRUPT_MODULE_SYSCTRL    = SYSCTRL_IRQn,
	/** Interrupt vector index for a Watch Dog peripheral interrupt */
	SYSTEM_INTERRUPT_MODULE_WDT        = WDT_IRQn,
	/** Interrupt vector index for a Real Time Clock peripheral interrupt */
	SYSTEM_INTERRUPT_MODULE_RTC        = RTC_IRQn,
	/** Interrupt vector index for an External Interrupt peripheral interrupt */
	SYSTEM_INTERRUPT_MODULE_EIC        = EIC_IRQn,
	/** Interrupt vector index for a Non Volatile Memory Controller interrupt */
	SYSTEM_INTERRUPT_MODULE_NVMCTRL    = NVMCTRL_IRQn,
	/** Interrupt vector index for a Direct Memory Access interrupt */
	SYSTEM_INTERRUPT_MODULE_DMA        = DMAC_IRQn,
#if defined(__DOXYGEN__) || defined(ID_USB)
	/** Interrupt vector index for a Universal Serial Bus interrupt */
	SYSTEM_INTERRUPT_MODULE_USB        = USB_IRQn,
#endif
	/** Interrupt vector index for an Event System interrupt */
	SYSTEM_INTERRUPT_MODULE_EVSYS      = EVSYS_IRQn,
#if defined(__DOXYGEN__)
	/** Interrupt vector index for a SERCOM peripheral interrupt.
	 *
	 *  Each specific device may contain several SERCOM peripherals; each module
	 *  instance will have its own entry in the table, with the instance number
	 *  substituted for "n" in the entry name (e.g.
	 *  \c SYSTEM_INTERRUPT_MODULE_SERCOM0).
	 */
	SYSTEM_INTERRUPT_MODULE_SERCOMn    = SERCOMn_IRQn,

	/** Interrupt vector index for a Timer/Counter Control peripheral interrupt.
	 *
	 *  Each specific device may contain several TCC peripherals; each module
	 *  instance will have its own entry in the table, with the instance number
	 *  substituted for "n" in the entry name (e.g.
	 *  \c SYSTEM_INTERRUPT_MODULE_TCC0).
	 */
	SYSTEM_INTERRUPT_MODULE_TCCn        = TCCn_IRQn,

	/** Interrupt vector index for a Timer/Counter peripheral interrupt.
	 *
	 *  Each specific device may contain several TC peripherals; each module
	 *  instance will have its own entry in the table, with the instance number
	 *  substituted for "n" in the entry name (e.g.
	 *  \c SYSTEM_INTERRUPT_MODULE_TC3).
	 */
	SYSTEM_INTERRUPT_MODULE_TCn        = TCn_IRQn,
#else
	_SYSTEM_INTERRUPT_MODULES(SERCOM)

	_SYSTEM_INTERRUPT_MODULES(TCC)

	SYSTEM_INTERRUPT_MODULE_TC3        = TC3_IRQn,
	SYSTEM_INTERRUPT_MODULE_TC4        = TC4_IRQn,
	SYSTEM_INTERRUPT_MODULE_TC5        = TC5_IRQn,
#  if defined(ID_TC6)
	SYSTEM_INTERRUPT_MODULE_TC6        = TC6_IRQn,
#  endif
#  if defined(ID_TC7)
	SYSTEM_INTERRUPT_MODULE_TC7        = TC7_IRQn,
#  endif
#endif

#if defined(__DOXYGEN__) || defined(ID_ADC)
	/** Interrupt vector index for an Analog-to-Digital peripheral interrupt */
	SYSTEM_INTERRUPT_MODULE_ADC        = ADC_IRQn,
#endif

#if defined(__DOXYGEN__) || defined(ID_AC)
	/** Interrupt vector index for an Analog Comparator peripheral interrupt */
	SYSTEM_INTERRUPT_MODULE_AC         = AC_IRQn,
#endif

#if defined(__DOXYGEN__) || defined(ID_DAC)
	/** Interrupt vector index for a Digital-to-Analog peripheral interrupt */
	SYSTEM_INTERRUPT_MODULE_DAC        = DAC_IRQn,
#endif
#if defined(__DOXYGEN__) || defined(ID_PTC)
	/** Interrupt vector index for a Peripheral Touch Controller peripheral
	 *  interrupt */
	SYSTEM_INTERRUPT_MODULE_PTC        = PTC_IRQn,
#endif
#if defined(__DOXYGEN__) || defined(ID_I2S)
	/** Interrupt vector index for a Inter-IC Sound Interface peripheral
	 *  interrupt */
	SYSTEM_INTERRUPT_MODULE_I2S        = I2S_IRQn,
#endif
#if defined(__DOXYGEN__) || defined(ID_AC1)
	/** Interrupt vector index for an Analog Comparator 1 peripheral interrupt */
	SYSTEM_INTERRUPT_MODULE_AC1        = AC1_IRQn,
#endif
};

/** @} */

#endif
