/**
 *
 * \file
 *
 * \brief Common Standard I/O Serial Management.
 *
 * This file defines a useful set of functions for the Stdio Serial interface on AVR
 * and SAM devices.
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


#ifndef _STDIO_SERIAL_H_
#define _STDIO_SERIAL_H_

/**
 * \defgroup group_common_utils_stdio_stdio_serial Standard serial I/O (stdio)
 * \ingroup group_common_utils_stdio
 *
 * Common standard serial I/O management driver that
 * implements a stdio serial interface on AVR and SAM devices.
 *
 * \{
 */

#include <stdio.h>
#include "compiler.h"
#ifndef SAMD20
# include "sysclk.h"
#endif
#include "serial.h"

#if (XMEGA || MEGA_RF) && defined(__GNUC__)
	extern int _write (char c, int *f);
	extern int _read (int *f);
#endif


//! Pointer to the base of the USART module instance to use for stdio.
extern volatile void *volatile stdio_base;
//! Pointer to the external low level write function.
extern int (*ptr_put)(void volatile*, char);

//! Pointer to the external low level read function.
extern void (*ptr_get)(void volatile*, char*);

/*! \brief Initializes the stdio in Serial Mode.
 *
 * \param usart       Base address of the USART instance.
 * \param opt         Options needed to set up RS232 communication (see \ref usart_options_t).
 *
 */
static inline void stdio_serial_init(volatile void *usart, const usart_serial_options_t *opt)
{
	stdio_base = (void *)usart;
	ptr_put = (int (*)(void volatile*,char))&usart_serial_putchar;
	ptr_get = (void (*)(void volatile*,char*))&usart_serial_getchar;
# if (XMEGA || MEGA_RF)
	usart_serial_init((USART_t *)usart,opt);
# elif UC3
	usart_serial_init(usart,(usart_serial_options_t *)opt);
# elif SAM
	usart_serial_init((Usart *)usart,(usart_serial_options_t *)opt);
# else
#  error Unsupported chip type
# endif

# if defined(__GNUC__)
#  if (XMEGA || MEGA_RF)
	// For AVR GCC libc print redirection uses fdevopen.
	fdevopen((int (*)(char, FILE*))(_write),(int (*)(FILE*))(_read));
#  endif
#  if UC3 || SAM
	// For AVR32 and SAM GCC
	// Specify that stdout and stdin should not be buffered.
	setbuf(stdout, NULL);
	setbuf(stdin, NULL);
	// Note: Already the case in IAR's Normal DLIB default configuration
	// and AVR GCC library:
	// - printf() emits one character at a time.
	// - getchar() requests only 1 byte to exit.
#  endif
# endif
}

/**
 * \}
 */

#endif  // _STDIO_SERIAL_H_
