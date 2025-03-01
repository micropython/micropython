/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017-2019 Damien P. George
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
#ifndef MICROPY_INCLUDED_STM32_MBOOT_PACK_H
#define MICROPY_INCLUDED_STM32_MBOOT_PACK_H

#include <stdint.h>
#include "py/mphal.h"

#if MBOOT_ENABLE_PACKING

#include "lib/libhydrogen/hydrogen.h"

// Encrypted & signed bootloader support

/******************************************************************************/
// Interface

#define MBOOT_PACK_HEADER_VERSION (1)

// Used by libhydrogen for signing and secretbox context.
#define MBOOT_PACK_HYDRO_CONTEXT "mbootenc"

// Maximum size of the firmware payload.
#define MBOOT_PACK_DFU_CHUNK_BUF_SIZE (MBOOT_PACK_CHUNKSIZE + hydro_secretbox_HEADERBYTES)

enum mboot_pack_chunk_format {
    MBOOT_PACK_CHUNK_META = 0,
    MBOOT_PACK_CHUNK_FULL_SIG = 1,
    MBOOT_PACK_CHUNK_FW_RAW = 2,
    MBOOT_PACK_CHUNK_FW_GZIP = 3,
};

// Each DFU chunk transferred has this header to validate it.

typedef struct _mboot_pack_chunk_buf_t {
    struct {
        uint8_t header_vers;
        uint8_t format;  // enum mboot_pack_chunk_format
        uint8_t _pad[2];
        uint32_t address;
        uint32_t length; // number of bytes in following "data" payload, excluding "signature"
    } header;
    uint8_t data[MBOOT_PACK_DFU_CHUNK_BUF_SIZE];
    uint8_t signature[hydro_sign_BYTES];
} mboot_pack_chunk_buf_t;

// Signing and encryption keys, stored in mboot flash, provided externally.
extern const uint8_t mboot_pack_sign_public_key[hydro_sign_PUBLICKEYBYTES];
extern const uint8_t mboot_pack_secretbox_key[hydro_secretbox_KEYBYTES];

/******************************************************************************/
// Implementation

void mboot_pack_init(void);
int mboot_pack_write(uint32_t addr, const uint8_t *src8, size_t len, bool dry_run);

#endif // MBOOT_ENABLE_PACKING

#endif // MICROPY_INCLUDED_STM32_MBOOT_PACK_H
