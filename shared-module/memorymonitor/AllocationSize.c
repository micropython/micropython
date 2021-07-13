/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
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

#include "shared-bindings/memorymonitor/AllocationSize.h"

#include "py/gc.h"
#include "py/mpstate.h"
#include "py/runtime.h"

void common_hal_memorymonitor_allocationsize_construct(memorymonitor_allocationsize_obj_t *self) {
    common_hal_memorymonitor_allocationsize_clear(self);
    self->next = NULL;
    self->previous = NULL;
}

void common_hal_memorymonitor_allocationsize_pause(memorymonitor_allocationsize_obj_t *self) {
    *self->previous = self->next;
    self->next = NULL;
    self->previous = NULL;
}

void common_hal_memorymonitor_allocationsize_resume(memorymonitor_allocationsize_obj_t *self) {
    if (self->previous != NULL) {
        mp_raise_RuntimeError(translate("Already running"));
    }
    self->next = MP_STATE_VM(active_allocationsizes);
    self->previous = (memorymonitor_allocationsize_obj_t **)&MP_STATE_VM(active_allocationsizes);
    if (self->next != NULL) {
        self->next->previous = &self->next;
    }
    MP_STATE_VM(active_allocationsizes) = self;
}

void common_hal_memorymonitor_allocationsize_clear(memorymonitor_allocationsize_obj_t *self) {
    for (size_t i = 0; i < ALLOCATION_SIZE_BUCKETS; i++) {
        self->buckets[i] = 0;
    }
}

uint16_t common_hal_memorymonitor_allocationsize_get_len(memorymonitor_allocationsize_obj_t *self) {
    return ALLOCATION_SIZE_BUCKETS;
}

size_t common_hal_memorymonitor_allocationsize_get_bytes_per_block(memorymonitor_allocationsize_obj_t *self) {
    return BYTES_PER_BLOCK;
}

uint16_t common_hal_memorymonitor_allocationsize_get_item(memorymonitor_allocationsize_obj_t *self, int16_t index) {
    return self->buckets[index];
}

void memorymonitor_allocationsizes_track_allocation(size_t block_count) {
    memorymonitor_allocationsize_obj_t *as = MP_OBJ_TO_PTR(MP_STATE_VM(active_allocationsizes));
    size_t power_of_two = 0;
    block_count >>= 1;
    while (block_count != 0) {
        power_of_two++;
        block_count >>= 1;
    }
    while (as != NULL) {
        as->buckets[power_of_two]++;
        as = as->next;
    }
}

void memorymonitor_allocationsizes_reset(void) {
    MP_STATE_VM(active_allocationsizes) = NULL;
}
