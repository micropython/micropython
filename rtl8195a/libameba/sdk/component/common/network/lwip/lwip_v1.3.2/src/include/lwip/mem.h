/**
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef __LWIP_MEM_H__
#define __LWIP_MEM_H__

#include "lwip/opt.h"

#ifdef __cplusplus
extern "C" {
#endif

#if MEM_LIBC_MALLOC

#include <stddef.h> /* for size_t */

typedef size_t mem_size_t;

/* aliases for C library malloc() */
#define mem_init()
/* in case C library malloc() needs extra protection,
 * allow these defines to be overridden.
 */
#ifndef mem_free
#define mem_free free
#endif
#ifndef mem_malloc
#define mem_malloc malloc
#endif
#ifndef mem_calloc
#define mem_calloc calloc
#endif
#ifndef mem_realloc
static void *mem_realloc(void *mem, mem_size_t size)
{
  LWIP_UNUSED_ARG(size);
  return mem;
}
#endif
#else /* MEM_LIBC_MALLOC */

/* MEM_SIZE would have to be aligned, but using 64000 here instead of
 * 65535 leaves some room for alignment...
 */
#if MEM_SIZE > 64000l
typedef u32_t mem_size_t;
#else
typedef u16_t mem_size_t;
#endif /* MEM_SIZE > 64000 */

#if MEM_USE_POOLS
/** mem_init is not used when using pools instead of a heap */
#define mem_init()
/** mem_realloc is not used when using pools instead of a heap:
    we can't free part of a pool element and don't want to copy the rest */
#define mem_realloc(mem, size) (mem)
#else /* MEM_USE_POOLS */
/* lwIP alternative malloc */
void  mem_init(void);
void *mem_realloc(void *mem, mem_size_t size);
#endif /* MEM_USE_POOLS */
void *mem_malloc(mem_size_t size);
void *mem_calloc(mem_size_t count, mem_size_t size);
void  mem_free(void *mem);
#endif /* MEM_LIBC_MALLOC */

#ifndef LWIP_MEM_ALIGN_SIZE
#define LWIP_MEM_ALIGN_SIZE(size) (((size) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT-1))
#endif

#ifndef LWIP_MEM_ALIGN
#define LWIP_MEM_ALIGN(addr) ((void *)(((mem_ptr_t)(addr) + MEM_ALIGNMENT - 1) & ~(mem_ptr_t)(MEM_ALIGNMENT-1)))
#endif

#ifdef __cplusplus
}
#endif

#endif /* __LWIP_MEM_H__ */
