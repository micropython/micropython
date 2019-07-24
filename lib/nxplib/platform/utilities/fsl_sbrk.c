/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#if defined(__GNUC__)
#include <stdio.h>
#include <errno.h>
#endif

#if defined(__GNUC__)
/*!
 * @brief Function to override ARMGCC default function _sbrk
 *
 * _sbrk is called by malloc. ARMGCC default _sbrk compares "SP" register and
 * heap end, if heap end is larger than "SP", then _sbrk returns error and
 * memory allocation failed. This function changes to compare __HeapLimit with
 * heap end.
 */
caddr_t _sbrk(int incr)
{
    extern char end __asm("end");
    extern char heap_limit __asm("__HeapLimit");
    static char *heap_end;
    char *prev_heap_end;

    if (heap_end == NULL)
        heap_end = &end;

    prev_heap_end = heap_end;

    if ((unsigned int)heap_end + incr > (unsigned int)(&heap_limit))
    {
        errno = ENOMEM;
        return (caddr_t)-1;
    }

    heap_end = (char *)((unsigned int)heap_end + incr);

    return (caddr_t)prev_heap_end;
}
#endif
