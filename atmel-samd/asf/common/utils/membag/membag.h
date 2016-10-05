/**
 * \file
 *
 * \brief Memory Bag allocator for 8-bit AVR, 32-bit AVR, SAM
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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

#ifndef UTILS_MEMBAG_H
#define UTILS_MEMBAG_H

#include <stddef.h>

/**
 * \defgroup membag_group Memory Bag Allocator
 *
 * The Membag allocator is a optimized, fragmentationless general purpose
 * memory allocator utility module designed to replace the standard C library
 * \c malloc() and \c free() functions in resource constrained environments.
 *
 *
 * Internally, the Membag allocator uses several user defined "bags" of one or
 * more fixed-size blocks to form a memory pool for use in a user application.
 * When an allocation is requested, the Membag module will search all available
 * bags and find the smallest unallocated block of sufficient size, and return
 * this block to the calling function. The size of each bag and number of blocks
 * in each bag is user configurable, via the \ref conf_membag.h header file
 * added to the user project.
 *
 * The allocator also has basic statistics functionality to obtain the size of
 * the entire memory pool, amount of free memory, and the size of the smallest
 * and largest free memory block.
 *
 * The memory bag allocator always allocates memory as a block from a fixed size
 * bag/pool. This helps reduce memory fragmentation compared to an generic
 * allocator that gives exactly the bytes requested. While this gives a
 * trade-off of the maximum number of concurrent allocations and the size of
 * allocations that are allowable, the allocator prevents memory fragmentation
 * from occuring in an embedded application.
 *
 * The allocation and deallocation of memory with the Membag module is
 * non-deterministic, however the module functions all have a maximum run time
 * that is dependent on the number of bags that have been configured.
 *
 * @{
 */

/**
 * Macro used to create memory bags in conf_membag.h
 *
 * \note Multiple bags of the same size are allowed, if more than 32 bags of a
 *       given size are required in an application.
 *
 * \param objsize  Size of each block in the bag
 * \param nr_objs  Number of blocks in the bag, a value less than 32
 */
#define MEMBAG(objsize, nr_objs)\
	{ .block_size = objsize, .num_blocks = nr_objs }

/**
 * Macro used to store the size of the membags in conf_membag.h
 *
 * \note Multiple bags of the same size are allowed, if more than 32 bags of a
 *       given size are required in an application.
 *
 * \param objsize  Size of each block in the bag
 * \param nr_objs  Number of blocks in the bag, a value less than 32
 */
#define MEMBAG_SIZE(objsize, nr_objs)\
	(objsize * nr_objs)

void membag_init(void);

size_t membag_get_total(void);
size_t membag_get_total_free(void);

size_t membag_get_smallest_free_block_size(void);
size_t membag_get_largest_free_block_size(void);

void *membag_alloc(const size_t size);
void membag_free(const void *ptr);

/** @} */

#endif /* UTILS_MEMBAG_H */
