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
#ifndef MICROPY_INCLUDED_STM32_MBOOT_ENCRYPTION_H
#define MICROPY_INCLUDED_STM32_MBOOT_ENCRYPTION_H

#if MBOOT_ENCRYPTED

#include <stdint.h>
#include "py/mphal.h"
#include "dfu.h"
#include "extmod/uzlib/uzlib.h"
#include "lib/libhydrogen/hydrogen.h"

// Encrypted & signed bootloader support

#define MBOOT_FW_SECTIONS 4
#define FOOTER_VERSION 1

// Footer stored at the end of flash; 208 bytes long
typedef struct __attribute__ ((__packed__)) {
    uint32_t address;
    uint32_t length;
    uint8_t sha256[32];
} fw_footer_section_v1_t;

typedef uint8_t public_key_t[hydro_sign_PUBLICKEYBYTES];

typedef struct __attribute__ ((__packed__)) _fw_footer_v1 {
    uint8_t footer_vers;
    char version[15];
    public_key_t public_key;
    fw_footer_section_v1_t sections[MBOOT_FW_SECTIONS];
} fw_footer_v1_t;

enum chunk_format {
    RAW=0,
    GZIP=1
};

// Each DFU chunk transfered has this header to validate it.
typedef struct __attribute__ ((__packed__)) _chunk_header_v1 {
    uint8_t header_vers;
    uint8_t format;  // chunk_format
    uint32_t address;
    uint32_t length;
    uint8_t signature[64];  // hydro_sign_BYTES
} chunk_header_v1_t;

#define MBOOT_PUBLIC_KEY_LOCATION (APPLICATION_ADDR - sizeof(public_key_t))
#define FW_FOOTER_LOCATION (FLASH_END + 1 - sizeof(fw_footer_v1_t))

// Used by liibhydrogen
#define ENC_CONTEXT "mbootenc"

// pyb_usbdd_StrDescriptor
#define  MBOOT_ERROR_STR_UNENCRYPTED_IDX 0x12
#define  MBOOT_ERROR_STR_UNENCRYPTED "Encryption key not yet flashed"

#define  MBOOT_ERROR_STR_INVALID_SIG_IDX 0x13
#define  MBOOT_ERROR_STR_INVALID_SIG "Invalid signature in file"

#define  MBOOT_ERROR_STR_INVALID_READ_IDX 0x14
#define  MBOOT_ERROR_STR_INVALID_READ "Read support disabled on encrypted bootloader"

#endif // MBOOT_ENCRYPTED
#endif // MICROPY_INCLUDED_STM32_MBOOT_ENCRYPTION_H
