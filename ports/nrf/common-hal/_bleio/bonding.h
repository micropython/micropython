/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Dan Halbert for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_NRF_COMMON_HAL_BLEIO_BONDING_H
#define MICROPY_INCLUDED_NRF_COMMON_HAL_BLEIO_BONDING_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ble.h"
#include "ble_drv.h"
#include "common-hal/_bleio/__init__.h"

#define EDIV_INVALID (0xffff)

#define BONDING_DEBUG (1)
#if BONDING_DEBUG
  #define BONDING_DEBUG_PRINTF(...) printf(__VA_ARGS__)
  #define BONDING_DEBUG_PRINT_BLOCK(block) bonding_print_block(block)
  #define BONDING_DEBUG_PRINT_KEYS(keys) bonding_print_keys(keys)
#else
  #define BONDING_DEBUG_PRINTF(...)
  #define BONDING_DEBUG_PRINT_BLOCK(block)
  #define BONDING_DEBUG_PRINT_KEYS(keys)
#endif

// Bonding data is stored in variable-length blocks consecutively in
// erased flash (all 1's). The blocks are 32-bit aligned, though the
// data may be any number of bytes.  We hop through the blocks using
// the size field to find the next block.  When we hit a word that is
// all 1's, we have reached the end of the blocks.  We can write a new
// block there.

typedef enum {
    BLOCK_INVALID = 0,      // Ignore this block
    BLOCK_KEYS = 1,         // Block contains bonding keys.
    BLOCK_SYS_ATTR = 2,     // Block contains sys_attr values (CCCD settings, etc.).
    BLOCK_UNUSED = 0xff,    // Initial erased value.
} bonding_block_type_t;

typedef struct {
    bool is_central : 1;            // 1 if data is for a central role.
    uint16_t reserved : 7;          // Not currently used
    bonding_block_type_t type : 8;  // What kind of data is stored in.
    uint16_t ediv;                 // ediv value; used as a lookup key.
    uint16_t conn_handle;          // Connection handle: used when a BLOCK_SYS_ATTR is queued to write.
                                   // Not used as a key, etc.
    uint16_t data_length;          // Length of data in bytes, including ediv, not including padding.
    // End of block header. 32-bit boundary here.
    uint8_t data[];                // Rest of data in the block. Needs to be 32-bit aligned.
    // Block is padded to 32-bit alignment.
} bonding_block_t;

void bonding_background(void);
void bonding_erase_storage(void);
void bonding_reset(void);
void bonding_clear_keys(bonding_keys_t *bonding_keys);
bool bonding_load_cccd_info(bool is_central, uint16_t conn_handle, uint16_t ediv);
bool bonding_load_keys(bool is_central, uint16_t ediv, bonding_keys_t *bonding_keys);
const ble_gap_enc_key_t *bonding_load_peer_encryption_key(bool is_central, const ble_gap_addr_t *peer);
size_t bonding_load_identities(bool is_central, const ble_gap_id_key_t **keys, size_t max_length);
size_t bonding_peripheral_bond_count(void);

#endif // MICROPY_INCLUDED_NRF_COMMON_HAL_BLEIO_BONDING_H
