/**
 * \file
 *
 * \brief Memory bag allocator
 *
 * Copyright (C) 2012-2015 Atmel Corporation. All rights reserved.
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

#include <compiler.h>
#include <membag.h>
#include "conf_membag.h"

/** \internal
 *
 *  Retrieves the number of elements in a statically declared array.
 */
#define ARRAY_LEN(a) (sizeof(a) / sizeof((a)[0]))

/**
 * Static address space which is split up into usable chunks by membag.
 * For configuration details, see \ref membag_list.
 */
static uint8_t membag_pool[CONF_MEMBAG_POOL_SIZE];

/**
 * Internal structure used by membag to keep track of memory,
 * with maximum 32 blocks per membag.
 */
struct membag {
	/*! Number of bytes per block in this bag. */
	size_t block_size;
	/*! Total number of blocks. */
	size_t num_blocks;
	/*! Pointer to start of this bag. */
	uintptr_t start;
	/*! Pointer to end of this bag. */
	uintptr_t end;
	/*! 32-bit integer used to keep track of allocations. */
	uint32_t allocated;
	/*! Counter for number of free blocks. */
	uint8_t blocks_free;
};

/**
 * Array of available membags, provided by the user in the applications
 * conf_membag.h header file. Example:
 *
 * \code
	 #define CONF_MEMBAG_ARRAY \
	     MEMBAG(32, 4),        \
	     MEMBAG(16, 2),

	 #define CONF_MEMBAG_POOL_SIZE \
	     MEMBAG_SIZE(32, 4) +      \
	     MEMBAG_SIZE(16, 2)
\endcode
 *
 */
static struct membag membag_list[] = {
	CONF_MEMBAG_ARRAY
};

/**
 * \brief Initialize the membag system.
 *
 * This function sets up the membags, allocates memory from the memory pool, and
 * initializes them. Any existing allocations are destroyed and all memory pools
 * reset to their initial states.
 */
void membag_init(void)
{
	uint8_t i;
	uintptr_t poolptr;

	poolptr = (uintptr_t)membag_pool;

	for (i = 0; i < ARRAY_LEN(membag_list); i++) {
		Assert(membag_list[i].block_size > 0);
		Assert(membag_list[i].num_blocks > 0);
		Assert(membag_list[i].num_blocks <= 32);
	
		membag_list[i].start = poolptr;
		poolptr += (membag_list[i].block_size *
				membag_list[i].num_blocks);
		membag_list[i].end = poolptr;
		membag_list[i].blocks_free = membag_list[i].num_blocks;

		/* Mark all blocks as free. */
		membag_list[i].allocated = 0;
	}
}

/**
 * \brief Determine the total remaining free memory from all membags.
 *
 * \return Sum of all free memory, in bytes.
 */
size_t membag_get_total_free(void)
{
	uint8_t i;
	size_t total_free = 0;

	for (i = 0; i < ARRAY_LEN(membag_list); i++) {
		total_free += membag_list[i].blocks_free *
				membag_list[i].block_size;
	}

	return total_free;
}

/**
 * \brief Determine the total memory from all membags.
 *
 * \return Sum of all blocks in all bags, in bytes.
 */
size_t membag_get_total(void)
{
	uint8_t i;
	size_t total = 0;

	for (i = 0; i < ARRAY_LEN(membag_list); i++) {
		total += membag_list[i].num_blocks * membag_list[i].block_size;
	}

	return total;
}

/**
 * \brief Determine the smallest available block size.
 *
 * Calculates the smallest block which can be allocated by the Membag allocator
 * if requested. Allocations larger than this amount are not guaranteed to
 * complete successfully.
 *
 * \return Size of the smallest available block, in bytes.
 */
size_t membag_get_smallest_free_block_size(void)
{
	uint8_t i;
	struct membag *smallest_bag = NULL;

	for (i = 0; i < ARRAY_LEN(membag_list); i++) {
		if (membag_list[i].blocks_free == 0) {
			continue;
		}

		if (!smallest_bag ||
				(smallest_bag->block_size > membag_list[i].block_size)) {
			smallest_bag = &membag_list[i];
		}
	}

	if (smallest_bag) {
		return smallest_bag->block_size;
	}

	return 0;
}

/**
 * \brief Determine the largest available block size.
 *
 * Calculates the largest block which can be allocated by the Membag allocator
 * if requested. Allocations larger than this amount are guaranteed to fail.
 *
 * \return Size of the largest available block, in bytes.
 */
size_t membag_get_largest_free_block_size(void)
{
	uint8_t i;
	struct membag *largest_bag = NULL;

	for (i = 0; i < ARRAY_LEN(membag_list); i++) {
		if (membag_list[i].blocks_free == 0) {
			continue;
		}

		if (!largest_bag ||
				(largest_bag->block_size < membag_list[i].block_size)) {
			largest_bag = &membag_list[i];
		}
	}

	if (largest_bag) {
		return largest_bag->block_size;
	}
	
	return 0;
}

/**
 * \brief Allocate a memory block via a block from the Membag pool
 *
 * Allocates memory to the user from one of the available Membag pools. Each
 * Membag pool is examined in sequence, and the first free block of sufficient
 * size (if any) is chosen for the allocation. Allocated blocks persist until
 * either the Membag module is re-initialized, or an allocation block is freed
 * via \ref membag_free().
 *
 * \note The execution cycle time for this function is not deterministic; each
 *       allocation request may take a variable amount of cycles to complete.
 *
 * \param size Size of memory block requested, in bytes
 *
 * \return Pointer to the start of an allocated block if one was found in the
 *         Membag pool, NULL if no suitable block was found.
 */
void *membag_alloc(const size_t size)
{
	uint8_t i;
	struct membag *smallest_bag = NULL;
	uintptr_t p;

	/* Find the smallest available block size big enough for the requested
	 * memory chunk size. */
	for (i = 0; i < ARRAY_LEN(membag_list); i++) {
		if (membag_list[i].blocks_free == 0) {
			continue;
		}

		if (membag_list[i].block_size >= size) {
			if (!smallest_bag ||
					(smallest_bag->block_size > membag_list[i].block_size)) {
				smallest_bag = &membag_list[i];
			}
		}
	}

	/* We return the first available block in the bag that has one, and if
	 * there is none, we return NULL.
	 */
	if (smallest_bag) {
		/* We know that there is a free block within the membag's
		 * memory, and we simply return the first one available.
		 */
		p = smallest_bag->start;

		for (i = 0; i < smallest_bag->num_blocks; i++) {
			/* Check the allocation byte to see whether the block is
			 * in use. */
			if (!(smallest_bag->allocated & ((uint32_t)1 << i))) {
				/* It is free, set it to used. */
				smallest_bag->allocated |= ((uint32_t)1 << i);
				smallest_bag->blocks_free--;

				return (void *)(p);
			}

			p += smallest_bag->block_size;
		}
	}

	/* There is no available memory. Return NULL. */
	return NULL;
}

/**
 * \brief Free a previously allocated memory block from the Membag pool
 *
 * This function frees memory which has been allocated previously via a
 * successful call to \ref membag_alloc(). Once deallocated, the given pointer
 * is no longer valid and should not be used in the user application unless
 * re-allocated.
 *
 * \note The execution cycle time for this function is not deterministic; each
 *       allocation request may take a variable amount of cycles to complete.
 *
 * \param ptr Pointer to an allocated memory block to free
 */
void membag_free(const void *ptr)
{
	uint8_t i;
	uintptr_t p = (uintptr_t)ptr;
	uint8_t block_index;

	for (i = 0; i < ARRAY_LEN(membag_list); i++) {
		if (p >= membag_list[i].start && p < membag_list[i].end) {
			block_index = (p - membag_list[i].start) / membag_list[i].block_size;
			
			/* Mark the memory as free. */
			membag_list[i].allocated &= ~((uint32_t)1 << block_index);
			membag_list[i].blocks_free++;

			return;
		}
	}
}
