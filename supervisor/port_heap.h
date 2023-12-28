/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Scott Shawcroft for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

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

void port_realloc(void *ptr, size_t size);

size_t port_heap_get_largest_free_size(void);
