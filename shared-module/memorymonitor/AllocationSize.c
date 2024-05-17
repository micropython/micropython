// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

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
        mp_raise_RuntimeError(MP_ERROR_TEXT("Already running"));
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

MP_REGISTER_ROOT_POINTER(mp_obj_t active_allocationsizes);
MP_REGISTER_ROOT_POINTER(mp_obj_t active_allocationalarms);
