/**
 * \file
 *
 * \brief System-specific implementation of the \ref _write function used by
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
 * \addtogroup group_common_utils_stdio
 *
 * \{
 */

volatile void *volatile stdio_base;
int (*ptr_put)(void volatile*, char);


#if ( defined(__ICCARM__))

#include <yfuns.h>

_STD_BEGIN

#pragma module_name = "?__write"

/*! \brief Writes a number of bytes, at most \a size, from the memory area
 *         pointed to by \a buffer.
 *
 * If \a buffer is zero then \ref __write performs flushing of internal buffers,
 * if any. In this case, \a handle can be \c -1 to indicate that all handles
 * should be flushed.
 *
 * \param handle File handle to write to.
 * \param buffer Pointer to buffer to read bytes to write from.
 * \param size Number of bytes to write.
 *
 * \return The number of bytes written, or \c _LLIO_ERROR on failure.
 */
size_t __write(int handle, const unsigned char *buffer, size_t size)
{
	size_t nChars = 0;

	if (buffer == 0) {
		// This means that we should flush internal buffers.
		return 0;
	}

	// This implementation only writes to stdout and stderr.
	// For all other file handles, it returns failure.
	if (handle != _LLIO_STDOUT && handle != _LLIO_STDERR) {
		return _LLIO_ERROR;
	}

	for (; size != 0; --size) {
		if (ptr_put(stdio_base, *buffer++) < 0) {
			return _LLIO_ERROR;
		}
		++nChars;
	}
	return nChars;
}

_STD_END


#elif (defined(__GNUC__))

int __attribute__((weak))
_write (int file, char * ptr, int len);

int __attribute__((weak))
_write (int file, char * ptr, int len)
{
	int nChars = 0;

	if ((file != 1) && (file != 2) && (file!=3)) {
		return -1;
	}

	for (; len != 0; --len) {
		if (ptr_put(stdio_base, *ptr++) < 0) {
			return -1;
		}
		++nChars;
	}
	return nChars;
}

#endif

/**
 * \}
 */

