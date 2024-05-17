// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "common-hal/nvm/ByteArray.h"

#include STM32_HAL_H
#include "supervisor/shared/stack.h"

#include <stdint.h>
#include <string.h>

uint32_t common_hal_nvm_bytearray_get_length(nvm_bytearray_obj_t *self) {
    return self->len;
}

bool common_hal_nvm_bytearray_set_bytes(nvm_bytearray_obj_t *self,
    uint32_t start_index, uint8_t *values, uint32_t len) {
    // Copy flash to buffer
    uint8_t buffer[self->len];
    memcpy(buffer, self->start_address, self->len);

    // Set bytes in buffer
    memmove(buffer + start_index, values, len);

    // Erase flash sector
    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
    FLASH_Erase_Sector(CIRCUITPY_INTERNAL_NVM_SECTOR, VOLTAGE_RANGE_3);

    // Write bytes to flash
    uint32_t i;
    uint32_t flash_addr = (uint32_t)self->start_address;
    for (i = 0; i < self->len; i++, flash_addr++) {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, flash_addr, buffer[i]) != HAL_OK) {
            HAL_FLASH_Lock();
            return false;
        }
    }

    // Finish up
    HAL_FLASH_Lock();
    return true;
}

// NVM memory is memory mapped so reading it is easy.
void common_hal_nvm_bytearray_get_bytes(nvm_bytearray_obj_t *self,
    uint32_t start_index, uint32_t len, uint8_t *values) {
    memcpy(values, self->start_address + start_index, len);
}
