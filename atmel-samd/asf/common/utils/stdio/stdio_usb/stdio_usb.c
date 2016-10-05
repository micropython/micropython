/**
 * \file
 *
 * \brief USB CDC Standard I/O Serial Management.
 *
 * This module defines support routines for a stdio serial interface to the
 * Atmel Software Framework (ASF) common USB CDC service.
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


#include "stdio_usb.h"

static bool stdio_usb_interface_enable = false;

int stdio_usb_putchar (volatile void * unused, char data)
{
	/* A negative return value should be used to indicate that data
	 * was not written, but this doesn't seem to work with GCC libc.
	 */
	if (!stdio_usb_interface_enable) {
		return 0;  // -1
	}

	return udi_cdc_putc(data) ? 0 : -1;
}

void stdio_usb_getchar (void volatile * unused, char *data)
{
	/* A negative return value should be used to indicate that data
	 * was not read, but this doesn't seem to work with GCC libc.
	 */
	if (!stdio_usb_interface_enable) {
		*data = 0;  // -1
		return;
	}

	*data = (char)udi_cdc_getc();
}

bool stdio_usb_enable(void)
{
	stdio_usb_interface_enable = true;
	return true;
}

void stdio_usb_disable(void)
{
	stdio_usb_interface_enable = false;
}

void stdio_usb_init(void)
{
	stdio_base = NULL;
	ptr_put = stdio_usb_putchar;
	ptr_get = stdio_usb_getchar;

	/*
	 * Start and attach USB CDC device interface for devices with
	 * integrated USB interfaces.  Assume the VBUS is present if
	 * VBUS monitoring is not available.
	 */
	udc_start ();

#if defined(__GNUC__)
# if XMEGA
	// For AVR GCC libc print redirection uses fdevopen.
	fdevopen((int (*)(char, FILE*))(_write),(int (*)(FILE*))(_read));
# endif
# if UC3 || SAM
	// For AVR32 and SAM GCC
	// Specify that stdout and stdin should not be buffered.
	setbuf(stdout, NULL);
	setbuf(stdin, NULL);
	// Note: Already the case in IAR's Normal DLIB default configuration
	// and AVR GCC library:
	// - printf() emits one character at a time.
	// - getchar() requests only 1 byte to exit.
# endif
#endif
}

