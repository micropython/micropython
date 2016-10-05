/**
 * \file
 *
 * \brief System-specific implementation of the \ref _read function used by
 *         the standard library.
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "compiler.h"

/**
 * \defgroup group_common_utils_stdio Standard I/O (stdio)
 *
 * Common standard I/O driver that implements the stdio
 * read and write functions on AVR and SAM devices.
 *
 * \{
 */

extern volatile void *volatile stdio_base;
void (*ptr_get)(void volatile*, char*);


// IAR common implementation
#if ( defined(__ICCAVR32__) || defined(__ICCAVR__) || defined(__ICCARM__) )

#include <yfuns.h>

_STD_BEGIN

#pragma module_name = "?__read"

/*! \brief Reads a number of bytes, at most \a size, into the memory area
 *         pointed to by \a buffer.
 *
 * \param handle File handle to read from.
 * \param buffer Pointer to buffer to write read bytes to.
 * \param size Number of bytes to read.
 *
 * \return The number of bytes read, \c 0 at the end of the file, or
 *         \c _LLIO_ERROR on failure.
 */
size_t __read(int handle, unsigned char *buffer, size_t size)
{
	int nChars = 0;
	// This implementation only reads from stdin.
	// For all other file handles, it returns failure.
	if (handle != _LLIO_STDIN) {
		return _LLIO_ERROR;
	}
	for (; size > 0; --size) {
		ptr_get(stdio_base, (char*)buffer);
		buffer++;
		nChars++;
	}
	return nChars;
}

/*! \brief This routine is required by IAR DLIB library since EWAVR V6.10
 * the implementation is empty to be compatible with old IAR version.
 */
int __close(int handle)
{
	UNUSED(handle);
	return 0;
}

/*! \brief This routine is required by IAR DLIB library since EWAVR V6.10
 * the implementation is empty to be compatible with old IAR version.
 */
int remove(const char* val)
{
	UNUSED(val);
	return 0;
}

/*! \brief This routine is required by IAR DLIB library since EWAVR V6.10
 * the implementation is empty to be compatible with old IAR version.
 */
long __lseek(int handle, long val, int val2)
{
	UNUSED(handle);
	UNUSED(val2);
	return val;
}

_STD_END

// GCC AVR32 and SAM implementation
#elif (defined(__GNUC__) && !XMEGA && !MEGA) 

int __attribute__((weak))
_read (int file, char * ptr, int len); // Remove GCC compiler warning

int __attribute__((weak))
_read (int file, char * ptr, int len)
{
	int nChars = 0;

	if (file != 0) {
		return -1;
	}

	for (; len > 0; --len) {
		ptr_get(stdio_base, ptr);
		ptr++;
		nChars++;
	}
	return nChars;
}

// GCC AVR implementation
#elif (defined(__GNUC__) && (XMEGA || MEGA) )

int _read (int *f); // Remove GCC compiler warning

int _read (int *f)
{
	char c;
	ptr_get(stdio_base,&c);
	return c;
}
#endif

/**
 * \}
 */

