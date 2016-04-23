/**
 ******************************************************************************
 * @file    newlib_stubs.cpp
 * @authors Matthew McGowan
 * @date    10 February 2015
 ******************************************************************************
  Copyright (c) 2015 Particle Industries, Inc.  All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************
 */

/* Define caddr_t as char* */
#include <sys/types.h>

/* Define abort() */
#include <stdlib.h>
#include "debug.h"

void *__dso_handle;

extern "C" {

/*
 * Implement C++ new/delete operators using the heap
 */

void *operator new(size_t size)
{
	return malloc(size);
}

void *operator new[](size_t size)
{
	return malloc(size);
}

void operator delete(void *p)
{
	free(p);
}

void operator delete[](void *p)
{
	free(p);
}


/* Bare metal, no processes, so error */
int _kill(int pid, int sig)
{
	return -1;
}

/* Bare metal, no processes, so always process id 1 */
int _getpid(void)
{
	return 1;
}

void _exit(int status) {
    PANIC(Exit,"Exit Called");

    while (1) {
        ;
    }
}

/* Default implementation for call made to pure virtual function. */
void __cxa_pure_virtual() {
  PANIC(PureVirtualCall,"Call on pure virtual");
  while (1);
}

/* Provide default implemenation for __cxa_guard_acquire() and
 * __cxa_guard_release(). Note: these must be revisited if a multitasking
 * OS is ported to this platform. */
__extension__ typedef int __guard __attribute__((mode (__DI__)));
int __cxa_guard_acquire(__guard *g) {return !*(char *)(g);};
void __cxa_guard_release (__guard *g) {*(char *)g = 1;};
void __cxa_guard_abort (__guard *) {};

}

namespace __gnu_cxx {

void __verbose_terminate_handler()
{
  abort();
}

}
