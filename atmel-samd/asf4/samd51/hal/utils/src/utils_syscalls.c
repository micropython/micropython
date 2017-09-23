/**
 * \file
 *
 * \brief Syscalls for SAM0 (GCC).
 *
 * Copyright (C) 2015-2016 Atmel Corporation. All rights reserved.
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

#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef __cplusplus
extern "C" {
#endif

#undef errno
extern int errno;
extern int _end;

extern caddr_t _sbrk(int incr);
extern int link(char *old, char *_new);
extern int _close(int file);
extern int _fstat(int file, struct stat *st);
extern int _isatty(int file);
extern int _lseek(int file, int ptr, int dir);
extern void _exit(int status);
extern void _kill(int pid, int sig);
extern int _getpid(void);

/**
 * \brief Replacement of C library of _sbrk
 */
extern caddr_t _sbrk(int incr)
{
	static unsigned char *heap = NULL;
	unsigned char *       prev_heap;

	if (heap == NULL) {
		heap = (unsigned char *)&_end;
	}
	prev_heap = heap;

	heap += incr;

	return (caddr_t)prev_heap;
}

/**
 * \brief Replacement of C library of link
 */
extern int link(char *old, char *_new)
{
	(void)old, (void)_new;
	return -1;
}

/**
 * \brief Replacement of C library of _close
 */
extern int _close(int file)
{
	(void)file;
	return -1;
}

/**
 * \brief Replacement of C library of _fstat
 */
extern int _fstat(int file, struct stat *st)
{
	(void)file;
	st->st_mode = S_IFCHR;

	return 0;
}

/**
 * \brief Replacement of C library of _isatty
 */
extern int _isatty(int file)
{
	(void)file;
	return 1;
}

/**
 * \brief Replacement of C library of _lseek
 */
extern int _lseek(int file, int ptr, int dir)
{
	(void)file, (void)ptr, (void)dir;
	return 0;
}

/**
 * \brief Replacement of C library of _exit
 */
extern void _exit(int status)
{
	printf("Exiting with status %d.\n", status);

	for (;;)
		;
}

/**
 * \brief Replacement of C library of _kill
 */
extern void _kill(int pid, int sig)
{
	(void)pid, (void)sig;
	return;
}

/**
 * \brief Replacement of C library of _getpid
 */
extern int _getpid(void)
{
	return -1;
}

#ifdef __cplusplus
}
#endif
