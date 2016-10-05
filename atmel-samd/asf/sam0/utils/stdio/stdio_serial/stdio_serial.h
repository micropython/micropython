/**
 *
 * \file
 *
 * \brief Common Standard I/O Serial Management.
 *
 * This file defines a useful set of functions for the Stdio Serial interface on
 * SAM0 devices.
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#ifndef STDIO_SERIAL_H_INCLUDED
#define STDIO_SERIAL_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup group_common_utils_stdio_stdio_serial Standard serial I/O (stdio)
 * \ingroup group_common_utils_stdio
 *
 * Common standard serial I/O management driver that
 * implements a stdio serial interface on AVR and SAM devices.
 *
 * @{
 */

#include <serial.h>
#include <stdio.h>
#include <compiler.h>

/** Pointer to the base of the USART module instance to use for stdio. */
extern volatile void *volatile stdio_base;

/** Pointer to the external low level write function. */
extern int (*ptr_put)(void volatile*, char);

/** Pointer to the external low level read function. */
extern void (*ptr_get)(void volatile*, char*);

#if SAM0
/** \brief Initializes the stdio in Serial Mode.
 *
 * \param module       Software USART instance to associate with the hardware.
 * \param hw           Base address of the USART hardware instance.
 * \param config       USART configuration parameters for the STDIO stream.
 *
 */
static inline void stdio_serial_init(
		struct usart_module *const module,
		usart_inst_t const hw,
		const struct usart_config *const config)
{
	stdio_base = (void *)module;
	ptr_put = (int (*)(void volatile*,char))&usart_serial_putchar;
	ptr_get = (void (*)(void volatile*,char*))&usart_serial_getchar;

	usart_serial_init(module, hw, config);
# if defined(__GNUC__)
	// Specify that stdout and stdin should not be buffered.
	setbuf(stdout, NULL);
	setbuf(stdin, NULL);
	// Note: Already the case in IAR's Normal DLIB default configuration
	// and AVR GCC library:
	// - printf() emits one character at a time.
	// - getchar() requests only 1 byte to exit.
#  endif
}
#endif

#if SAMB
/** \brief Initializes the stdio in Serial Mode.
 *
 * \param module       Software UART instance to associate with the hardware.
 * \param hw           Base address of the UART hardware instance.
 * \param config       UART configuration parameters for the STDIO stream.
 *
 */
static inline void stdio_serial_init(
		struct uart_module *const module,
		Uart * const hw,
		const struct uart_config *const config)
{
	stdio_base = (void *)module;
	ptr_put = (int (*)(void volatile*,char))&usart_serial_putchar;
	ptr_get = (void (*)(void volatile*,char*))&usart_serial_getchar;

	usart_serial_init(module, hw, config);
# if defined(__GNUC__)
	// Specify that stdout and stdin should not be buffered.
	setbuf(stdout, NULL);
	setbuf(stdin, NULL);
	// Note: Already the case in IAR's Normal DLIB default configuration
	// and AVR GCC library:
	// - printf() emits one character at a time.
	// - getchar() requests only 1 byte to exit.
#  endif
}
#endif

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif  // _STDIO_SERIAL_H_
