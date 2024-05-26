// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 microDev
//
// SPDX-License-Identifier: MIT

#include <string.h>
#include "nrf.h"

#include "shared-bindings/memorymap/AddressRange.h"

#include "py/runtime.h"


#ifdef NRF51_SERIES
size_t allow_ranges[][2] = {
    // FLASH
    {0x00000000, 0x00040000},
    // FICR & UICR ranges
    {0x10000000, 0x10002000},
    // RAM
    {0x20000000, 0x20010000},
    // PERIPHERALS
    {0x40000000, 0x60000000}
};
#elif defined NRF52_SERIES
size_t allow_ranges[][2] = {
    // FLASH
    {0x00000000, 0x00100000},
    // FICR & UICR ranges
    {0x10000000, 0x10002000},
    // RAM
    {0x20000000, 0x20040000},
    // PERIPHERALS
    {0x40000000, 0x60000000}
};
#elif defined NRF53_SERIES
size_t allow_ranges[][2] = {
    // FLASH
    {0x00000000, 0x00100000},
    // FICR & UICR ranges
    {0x00FF0000, 0x01000000},
    // RAM
    {0x20000000, 0x20080000},
    // PERIPHERALS
    {0x40000000, 0x60000000},
    {0xE0000000, 0xE0100000}
};
#else
#error "Unsupported nRF variant"
#endif

void common_hal_memorymap_addressrange_construct(memorymap_addressrange_obj_t *self, uint8_t *start_address, size_t length) {
    bool allowed = false;
    for (size_t i = 0; i < MP_ARRAY_SIZE(allow_ranges); i++) {
        uint8_t *allowed_start = (uint8_t *)allow_ranges[i][0];
        uint8_t *allowed_end = (uint8_t *)allow_ranges[i][1];
        if (allowed_start <= start_address &&
            (start_address + length) <= allowed_end) {
            allowed = true;
            break;
        }
    }

    if (!allowed) {
        mp_raise_ValueError(MP_ERROR_TEXT("Address range not allowed"));
    }

    self->start_address = start_address;
    self->len = length;
}

size_t common_hal_memorymap_addressrange_get_length(const memorymap_addressrange_obj_t *self) {
    return self->len;
}


void common_hal_memorymap_addressrange_set_bytes(const memorymap_addressrange_obj_t *self,
    size_t start_index, uint8_t *values, size_t len) {
    uint8_t *address = self->start_address + start_index;
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcast-align"
    if (len == 1) {
        *((uint8_t *)address) = values[0];
    } else if (len == sizeof(uint16_t) && (((size_t)address) % sizeof(uint16_t)) == 0) {
        *((uint16_t *)address) = ((uint16_t *)values)[0];
    } else if (len == sizeof(uint32_t) && (((size_t)address) % sizeof(uint32_t)) == 0) {
        *((uint32_t *)address) = ((uint32_t *)values)[0];
    } else if (len == sizeof(uint64_t) && (((size_t)address) % sizeof(uint64_t)) == 0) {
        *((uint64_t *)address) = ((uint64_t *)values)[0];
    } else {
        memcpy(address, values, len);
    }
    #pragma GCC diagnostic pop
}

void common_hal_memorymap_addressrange_get_bytes(const memorymap_addressrange_obj_t *self,
    size_t start_index, size_t len, uint8_t *values) {
    uint8_t *address = self->start_address + start_index;
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcast-align"
    if (len == 1) {
        values[0] = *((uint8_t *)address);
    } else if (len == sizeof(uint16_t) && (((size_t)address) % sizeof(uint16_t)) == 0) {
        ((uint16_t *)values)[0] = *((uint16_t *)address);
    } else if (len == sizeof(uint32_t) && (((size_t)address) % sizeof(uint32_t)) == 0) {
        ((uint32_t *)values)[0] = *((uint32_t *)address);
    } else if (len == sizeof(uint64_t) && (((size_t)address) % sizeof(uint64_t)) == 0) {
        ((uint64_t *)values)[0] = *((uint64_t *)address);
    } else {
        memcpy(values, address, len);
    }
    #pragma GCC diagnostic pop
}
