/**
 * \file
 *
 * \brief USB Standard I/O Serial Management.
 *
 * This file defines a useful set of functions for the Stdio Serial
 * interface on AVR devices.
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
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

#ifndef _stdio_usb_h_
#define _stdio_usb_h_

/**
 * \defgroup group_common_utils_stdio_stdio_usb USB/CDC Standard I/O (stdio)
 * \ingroup group_common_utils_stdio
 *
 * Standard I/O (stdio) management component that implements a stdio
 * USB CDC interface on AVR devices.
 *
 * \{
 */

#include <compiler.h>

#include <stdio.h>

#include <udc.h>
#include <udi_cdc.h>

extern int _write (char c, int *f);
extern int _read (int *f);


//! Pointer to the base of the USART module instance to use for stdio.
extern volatile void *volatile stdio_base;
//! Pointer to the external low level write function.
extern int (*ptr_put)(void volatile*, char);
//! Pointer to the external low level read function.
extern void (*ptr_get)(void volatile*, char*);

/*! \brief Sends a character with the USART.
 *
 * \param usart   Base address of the USART instance.
 * \param data    Character to write.
 *
 * \return Status.
 *   \retval  0  The character was written.
 *   \retval -1  The function timed out before the transmitter became ready.
 */
int stdio_usb_putchar (volatile void * usart, char data);

/*! \brief Waits until a character is received, and returns it.
 *
 * \param usart   Base address of the USART instance.
 * \param data    Data to read
 *
 * \return Nothing.
 */
void stdio_usb_getchar (void volatile * usart, char * data);

/*! \brief Enables the stdio in USB Serial Mode.
 *
 * \return \c 1 if function was successfully done, otherwise \c 0.
 */
bool stdio_usb_enable(void);

/*! \brief Disables the stdio in USB Serial Mode.
 *
 * \return Nothing.
 */
void stdio_usb_disable(void);

/*! \brief Initializes the stdio in USB Serial Mode.
 *
 * \return Nothing.
 */
void stdio_usb_init(void);

/**
 * \}
 */

#endif  // _stdio_usb_h_
