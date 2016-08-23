/**
 * \file
 *
 * \brief Global interrupt management for 32-bit AVR
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
#ifndef UTILS_INTERRUPT_INTERRUPT_H
#define UTILS_INTERRUPT_INTERRUPT_H

#include <compiler.h>
#include <preprocessor/tpaste.h>

/**
 * \weakgroup interrupt_group
 *
 * @{
 */

//! Pointer to interrupt handler.
#if (defined __GNUC__)
typedef void (*__int_handler)(void);
#elif (defined __ICCAVR32__)
typedef void (__interrupt *__int_handler)(void);
#endif

/**
 * \name Interrupt Service Routine definition and registration
 *
 * @{
 */
#if defined(__GNUC__) || defined(__DOXYGEN__)

/**
 * \brief Macro to declare an interrupt service routine
 *
 * With GCC, this macro only causes the function to be defined as an interrupt
 * service routine, i.e., it does not add any initialization code. A valid
 * function name is required for use with \ref irq_register_handler.
 *
 * With IAR, this macro defines the function as an interrupt service routine and
 * causes the compiler to add initialization code for the interrupt controller
 * (INTC). The interrupt group and level, as well as a valid function name are
 * therefore required.\n
 * \note If \ref CONFIG_INTERRUPT_FORCE_INTC is defined, only a valid function
 * name is required for use with \ref irq_register_handler. The initialization
 * code will be handled by the interrupt controller itself.
 *
 * Usage:
 * \code
	ISR(foo_irq_handler, AVR32_xxx_IRQ_GROUP, n)
	{
	     // Function definition
	     ...
	}
\endcode
 *
 * \param func Name for the function, needed by \ref irq_register_handler.
 * \param int_grp Interrupt group to define service routine for.
 * \param int_lvl Priority level to set for the interrupt group, in the range
 * \c 0 to \c 3.
 *
 * \note The interrupt groups can be found in the device header files for the
 * IAR toolchain (avr32/io\<part\>.h).
 *
 * \todo Update to use IRQ numbers when these are made available in the
 * device header files of both IAR and GCC.
 */
#  define ISR(func, int_grp, int_lvl)    \
	__attribute__((__interrupt__)) static void func (void)

#elif defined(__ICCAVR32__) && defined(CONFIG_INTERRUPT_FORCE_INTC)
#  define ISR(func, int_grp, int_lvl) \
		__interrupt static void func (void)

#elif defined(__ICCAVR32__)
#  define ISR0(...) _Pragma(#__VA_ARGS__)
#  define ISR(func, int_grp, int_lvl)                                          \
		ISR0(handler=int_grp, int_lvl)                                 \
		__interrupt static void func (void)
#endif

#if defined(__GNUC__) || defined(__DOXYGEN__) || defined(CONFIG_INTERRUPT_FORCE_INTC)
#  include <intc.h>

/**
 * \brief Initialize interrupt vectors
 *
 * With GCC, this macro adds code for initialization of the interrupt vectors
 * with the driver for the interrupt controller (INTC).
 *
 * With IAR and unless \ref CONFIG_INTERRUPT_FORCE_INTC is defined this macro
 * adds no code, since initialization of the INTC is handled by the compiler.
 * \note Defining \ref CONFIG_INTERRUPT_FORCE_INTC will force the use of the
 * INTC driver, replacing the compiler built-in interrupt handler.
 *
 * This must be called prior to \ref irq_register_handler.
 */
#  define irq_initialize_vectors()    INTC_init_interrupts()

/**
 * \brief Register handler for interrupt
 *
 * With GCC, this macro adds code for registering an interrupt handler with the
 * driver for the interrupt controller (INTC).
 *
 * With IAR and unless \ref CONFIG_INTERRUPT_FORCE_INTC is defined this macro
 * adds no code, since initialization of the INTC is handled by the compiler.
 * \note Defining \ref CONFIG_INTERRUPT_FORCE_INTC will force the use of the
 * INTC driver, replacing the compiler built-in interrupt handler.
 *
 * \param func Name of handler function to register for interrupt.
 * \param int_num Number of the interrupt line to register function for.
 * \param int_lvl Priority level to set for the interrupt's group, in the range
 * \c 0 to \c 3.
 *
 * Usage:
 * \code
	irq_initialize_vectors();
	irq_register_handler(foo_irq_handler, AVR32_xxx_IRQ, n);
\endcode
 *
 * \note The function \a func must be defined with the \ref ISR macro.
 * \note The interrupt line number can be found in the device header files for
 * the GCC toolchain (avr32/\<part\>.h).
 */
#  define irq_register_handler(func, int_num, int_lvl)                         \
	INTC_register_interrupt(func, int_num,                                 \
			TPASTE2(AVR32_INTC_INT, int_lvl))

#elif defined(__ICCAVR32__)
#  define irq_initialize_vectors()                        do{ } while(0)
#  define irq_register_handler(func, int_num, int_lvl)    do{ } while(0)
#endif

//@}

#if (defined __GNUC__)
#  define cpu_irq_enable()                             \
	do {                                           \
		barrier();                             \
		__builtin_csrf(AVR32_SR_GM_OFFSET);    \
	} while (0)
#  define cpu_irq_disable()                            \
	do {                                           \
		__builtin_ssrf(AVR32_SR_GM_OFFSET);    \
		barrier();                             \
	} while (0)
#elif (defined __ICCAVR32__)
#  if (defined CONFIG_INTERRUPT_FORCE_INTC)
#    define cpu_irq_enable()                                 \
	do {                                                 \
		barrier();                                   \
		__clear_status_flag(AVR32_SR_GM_OFFSET);     \
	} while(0)
#    define cpu_irq_disable()                                \
	do {                                                 \
		__set_status_flag(AVR32_SR_GM_OFFSET);       \
		barrier();                                   \
	} while (0)
#  else
#    define cpu_irq_enable()     __enable_interrupt()
#    define cpu_irq_disable()    __disable_interrupt()
#  endif
#endif

typedef uint32_t irqflags_t;

static inline irqflags_t cpu_irq_save(void)
{
	volatile irqflags_t flags;

	flags = sysreg_read(AVR32_SR);
	cpu_irq_disable();

	return flags;
}

static inline bool cpu_irq_is_enabled_flags(irqflags_t flags)
{
	return !(flags & AVR32_SR_GM_MASK);
}

static inline void cpu_irq_restore(irqflags_t flags)
{
	barrier();

   /* Restore the global IRQ mask status flag if it was previously set */
   if ( cpu_irq_is_enabled_flags(flags) ) {
      cpu_irq_enable();
   }

	barrier();
}

#define cpu_irq_is_enabled()    cpu_irq_is_enabled_flags(sysreg_read(AVR32_SR))

//! \name Global interrupt levels
//@{

/**
 * \brief Check if interrupt level is enabled in supplied flags
 *
 * \param flags State of interrupt flags.
 * \param level Bit position for interrupt level.
 *
 * \return True if interrupt level is enabled.
 */
static inline bool cpu_irq_level_is_enabled_flags(irqflags_t flags,
		uint32_t level)
{
	return !(flags & (1 << level));
}

/**
 * \brief Check if interrupt level is enabled
 *
 * \param level Interrupt level (0 to 3).
 *
 * \return True if interrupt level \a level is enabled.
 *
 * \note The interrupt level must be known at compile time.
 */
#define cpu_irq_level_is_enabled(level)                                  \
	cpu_irq_level_is_enabled_flags(sysreg_read(AVR32_SR),             \
			TPASTE3(AVR32_SR_I, level, M_OFFSET))

#if defined(__GNUC__) || defined(__DOXYGEN__)
/**
 * \brief Enable interrupt level
 *
 * \param level Interrupt level to enable (0 to 3).
 *
 * \note The interrupt level must be known at compile time.
 */
#  define cpu_irq_enable_level(level)                                    \
	do {                                                             \
		barrier();                                               \
		__builtin_csrf(TPASTE3(AVR32_SR_I, level, M_OFFSET));    \
	} while (0)

/**
 * \brief Disable interrupt level
 *
 * \param level Interrupt level to disable (0 to 3).
 *
 * \note The interrupt level must be known at compile time.
 */
#  define cpu_irq_disable_level(level)                                   \
	do {                                                             \
		__builtin_ssrf(TPASTE3(AVR32_SR_I, level, M_OFFSET));    \
		barrier();                                               \
	} while (0)

#elif (defined __ICCAVR32__)
#  define cpu_irq_enable_level(level)                                          \
	do {                                                                   \
		barrier();                                                     \
		__clear_status_flag(TPASTE3(AVR32_SR_I, level, M_OFFSET));     \
	} while(0)
#  define cpu_irq_disable_level(level)                                         \
	do {                                                                   \
		__set_status_flag(TPASTE3(AVR32_SR_I, level, M_OFFSET));       \
		barrier();                                                     \
	} while (0)
#endif

//@}

//@}

/**
 * \weakgroup interrupt_deprecated_group
 * @{
 */

#define Enable_global_interrupt()            cpu_irq_enable()
#define Disable_global_interrupt()           cpu_irq_disable()
#define Is_global_interrupt_enabled()        cpu_irq_is_enabled()

#define Enable_interrupt_level(level)        cpu_irq_enable_level(level)
#define Disable_interrupt_level(level)       cpu_irq_disable_level(level)
#define Is_interrupt_level_enabled(level)    cpu_irq_level_is_enabled(level)

/**
 * \name Interrupt protection of code sections
 * \note Use \ref cpu_irq_save and \ref cpu_irq_restore instead of these macros.
 * @{
 */

/**
 * \brief Start section with code protected against interrupts
 */
#define AVR32_ENTER_CRITICAL_REGION()                                          \
	{                                                                      \
		bool global_interrupt_enabled = Is_global_interrupt_enabled(); \
		Disable_global_interrupt();

/**
 * \brief End section with code protected against interrupts
 *
 * \note This macro must always be used in conjunction with
 * \ref AVR32_ENTER_CRITICAL_REGION so that interrupts are enabled again.
 */
#define AVR32_LEAVE_CRITICAL_REGION()                                          \
		if (global_interrupt_enabled) Enable_global_interrupt();       \
	}

//@}

//@}

#endif /* UTILS_INTERRUPT_INTERRUPT_H */
