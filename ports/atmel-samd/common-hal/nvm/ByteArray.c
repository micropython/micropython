// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "common-hal/nvm/ByteArray.h"
#include "shared-bindings/nvm/ByteArray.h"

#include "hal_flash.h"

#include "supervisor/shared/stack.h"

#include <stdint.h>
#include <string.h>

uint32_t common_hal_nvm_bytearray_get_length(const nvm_bytearray_obj_t *self) {
    return self->len;
}

bool common_hal_nvm_bytearray_set_bytes(const nvm_bytearray_obj_t *self,
    uint32_t start_index, uint8_t *values, uint32_t len) {
    // We don't use features that use any advanced NVMCTRL features so we can fake the descriptor
    // whenever we need it instead of storing it long term.
    struct flash_descriptor desc;
    desc.dev.hw = NVMCTRL;
    bool status = flash_write(&desc, (uint32_t)self->start_address + start_index, values, len) == ERR_NONE;
    assert_heap_ok();
    return status;
}

// NVM memory is memory mapped so reading it is easy.
void common_hal_nvm_bytearray_get_bytes(const nvm_bytearray_obj_t *self,
    uint32_t start_index, uint32_t len, uint8_t *values) {
    memcpy(values, self->start_address + start_index, len);
}
