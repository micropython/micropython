// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stddef.h>

// Ports provide a heap for allocations that live outside the VM. The VM heap
// is allocated into it in split chunks. The supervisor provides a default heap
// implementation for ports that don't have one of their own. Allocations done
// to the outer heap *must* be explicitly managed. Only VM allocations are garbage
// collected.

// Called after port_init(). Ports can init the heap earlier in `port_init()` if
// they need and leave this empty. Splitting this out allows us to provide a weak
// implementation to use by default.
void port_heap_init(void);

void *port_malloc(size_t size, bool dma_capable);

void port_free(void *ptr);

void *port_realloc(void *ptr, size_t size);

size_t port_heap_get_largest_free_size(void);
