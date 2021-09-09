/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Philipp Ebensberger
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

#include <stdint.h>
#include <string.h>
#include "mboot_validate.h"

// --------------------------------------------------------------------+
// Local Variables
// --------------------------------------------------------------------+
const uint32_t crc_polynomial = 0x04C11DB7;
const uint32_t crc_initial = 0xFFFFFFFF;
const uint32_t crc_final_xor = 0xFFFFFFFF;

// --------------------------------------------------------------------+
// Local Function Declarations
// --------------------------------------------------------------------+
static uint8_t reflect8(uint8_t val);
static uint32_t reflect32(uint32_t val);
static uint32_t crc32(const uint8_t *data, size_t len);

// --------------------------------------------------------------------+
// Global Function Definitions
// --------------------------------------------------------------------+
mboot_validate_status_t mboot_validate_firmware(fw_header_t *img_header, uintptr_t start_addr) {
    uint32_t crc_result = 0UL;
    mboot_validate_status_t header_status = mboot_validate_header(img_header);

    if (header_status == FWV_STATUS_OK) {
        crc_result = crc32((uint8_t *)start_addr, img_header->size - FW_HEADER_SIZE);

        if (crc_result == img_header->crc) {
            return FWV_STATUS_OK;
        } else {
            return FWV_STATUS_IMAGE_INVALID_CRC_ERROR;
        }
    } else {
        return header_status;
    }
}

mboot_validate_status_t mboot_validate_header(fw_header_t *img_header) {
    uint32_t crc_result = 0UL;

    if (strncmp(img_header->magic, FW_HEADER_MAGIC, FW_HEADER_MAGIC_SIZE) == 0) {
        crc_result = crc32((uint8_t *)img_header, FW_HEADER_SIZE - FW_HEADER_CRC_SIZE);

        if (crc_result == img_header->header_crc) {
            return FWV_STATUS_OK;
        } else {
            return FWV_STATUS_HEADER_INVALID_CRC_ERROR;
        }
    } else {
        return FWV_STATUS_HEADER_INVALID_MAGIC_ERROR;
    }
}

// --------------------------------------------------------------------+
// Local Function Definitions
// --------------------------------------------------------------------+
static uint8_t reflect8(uint8_t val) {
    uint8_t resVal = 0;

    for (int i = 0; i < 8; i++)
    {
        if ((val & (1 << i)) != 0) {
            resVal |= (uint8_t)(1 << (7 - i));
        }
    }

    return resVal;
}

static uint32_t reflect32(uint32_t val) {
    uint32_t resVal = 0;

    for (int i = 0; i < 32; i++)
    {
        if ((val & (1 << i)) != 0) {
            resVal |= (uint32_t)(1 << (31 - i));
        }
    }

    return resVal;
}

uint32_t crc32(const uint8_t *data, size_t len) {
    uint32_t crc = crc_initial;

    for (int i = 0; i < len; ++i)
    {
        uint8_t curByte = reflect8(data[i]);

        crc ^= (uint32_t)(curByte << 24); /* move byte into MSB of 32bit CRC */

        for (int i = 0; i < 8; i++)
        {
            if ((crc & 0x80000000) != 0) { /* test for MSB = bit 31 */
                crc = (uint32_t)((crc << 1) ^ crc_polynomial);
            } else {
                crc <<= 1;
            }
        }
    }
    crc = reflect32(crc);
    return (uint32_t)(crc ^ crc_final_xor);
}
