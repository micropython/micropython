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

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ble.h"
#include "ble_drv.h"
#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Adapter.h"
#include "shared-bindings/nvm/ByteArray.h"

#include "bonding.h"

// Internal flash area reserved for bonding storage.
#define BONDING_PAGES_START_ADDR CIRCUITPY_BLE_CONFIG_START_ADDR
#define BONDING_PAGES_END_ADDR   (CIRCUITPY_BLE_CONFIG_START_ADDR + CIRCUITPY_BLE_CONFIG_SIZE)

// First and last four bytes are magic bytes for id and version. Start data after that.
// 'BD01'
const uint32_t BONDING_START_FLAG ('1' | '0' << 8 | 'D' << 16 | 'B' << 24);
// 'ED01'
const uint32_t BONDING_END_FLAG ('1' | '0' << 8 | 'D' << 16 | 'E' << 24);

#define BONDING_DATA_START_ADDR (BONDING_DATA_START_ADDR + sizeof(BONDING_START_BYTES))
#define BONDING_DATA_END_ADDR (BONDING_PAGES_END_ADDR - sizeof(BONDING_END_BYTES))

#define BONDING_START_FLAG_ADDR BONDING_DATA_START_ADDR
#define BONDING_END_FLAG_ADDR BONDING_DATA_END_ADDR

// Bonding data is stored in variable-length blocks consecutively in erased flash.
// The blocks are 32-bit aligned, though the data may be any number of bytes.
// You can hop through the blocks using the size field to find the next block.
// When you hit a word that is all one's, you have reached the end of the blocks.
// You can write a new block there.

typdef enum {
    BLOCK_INVALID = 0,      // Ignore this block
    BLOCK_KEYS = 1,         // Block contains bonding keys.
    BLOCK_SYS_ATTR = 2,     // Block contains sys_attr values (CCCD settings, etc.).
    BLOCK_UNUSED = 0xff,    // Initial erased value.
} bonding_block_type_t;

typedef struct {
    uint16_t central:1;            // 1 if data is for a central role.
    uint16_t reserved: 7;          // Not currently used
    bonding_block_type_t type: 8;  // What kind of data is stored in.
    uint16_t ediv;                 // ediv value; used as a lookup key.
    uint32_t data_length;          // Length of data in bytes, including ediv, not including padding.
                                   // data_length only needs to be 16 bits, but easier to write a word at a time.
    uint8_t data[];                // Rest of data in the block. Needs to be 32-bit aligned.
    // Block is padded to 32-bit alignment.
} bonding_block_t;

STATIC inline size_t bonding_block_size(uint16_t data_length) {
    // Round data size up to the nearest 32-bit address.
    return sizeof(bonding_block_t) + ((data_length + 3) & 0x3);


void bonding_clear_keys(bonding_keys_t *bonding_keys) {
    memset(bonding_keys, 0, sizeof(bonding_keys));
}

STATIC void bonding_erase_storage(void) {
    // Erase all pages in the bonding area.
    for(uint32_t page_address = BONDING_PAGES_START_ADDR;
        page_address < BONDING_PAGES_END_ADDR;
        page_address += FLASH_PAGE) {
        nrf_nvmc_page_erase(page_address);
    }
    // Write marker words at the beginning and the end of the bonding area.
    nrf_nvmc_write_word(BONDING_DATA_START_ADDR, BONDING_START_FLAG_ADDR);
    nrf_nvmc_write_word(BONDING_DATA_END_ADDR, BONDING_END_FLAG_ADDR);
    // First unused block is at the beginning.
    bonding_unused_block = BONDING_DATA_START_ADDR;
}

STATIC bonding_block_t *bonding_unused_block = NULL;

void bonding_init(void) {
    if (BONDING_START_BYTES != *((uint32_t *) BONDING_START_FLAG_ADDR) ||
        BONDING_END_BYTES != *((uint32_t *) BONDING_END_FLAG_ADDR)) {
        bonding_erase_storage();
    } else {
        bonding_unused_block = bonding_find_block(BLOCK_UNUSED, EDIV_INVALID);
    }
}

// Given NULL to start or block address, return the address of the next valid block.
// The last block returned is the unused block at the end.
// Return NULL if we have run off the end of the bonding space.
STATIC bonding_block_t *bonding_next_block(bonding_block_t *block) {
    while (1) {
        // Advance to next block.
        if (block == NULL) {
            // Return first block (which might be unused if block list is empty).
            return BONDING_DATA_START_ADDR;
        } else if (block->type == BLOCK_UNUSED) {
            // Already at last block (the unused block).
            return NULL;
        }

        // Advance to next block.
        block += (bonding_block_t *) ((uint8_t *) block + bonding_block_word_size(block->data_length));

        }
        if (block >= (bonding_block_t *) BONDING_DATA_END_ADDR) {
            // Went past end of bonding space.
            return NULL;
        }
        if (block.valid) {
            // Found an empty or a valid block.
            return block;
        }
        // Invalid block (was erased); try again.
    }
}

// Find the block with given type and ediv value.
// If type == BLOCK_UNUSED, ediv is ignored and the the sole unused block at the end is returned.
// If not found, return NULL.
STATIC bonding_block_t *bonding_find_block(bonding_block_type_t type, uint16_t ediv) {
    bonding_block_t *block = NULL;
    while (1) {
        block = bonding_next_block(block);
        if (block == NULL) {
            return NULL;
        }
        if (block.type == BLOCK_UNUSED) {
            return block;
        }
        if (type == block.type && ediv = block.ediv) {
            return block;
        }
    }
}

// Set the header word to all 0's, to mark the block as invalid.
// We don't change data_length, so we can still skip over this block.
STATIC void bonding_invalidate_block(bonding_block_t *block) {
    nrf_nvmc_write_word((uint32_t) bonding_unused_block, 0x00000000);
}

// Try to write a new block. If no room, erase everything and start again.
// TODO: could do garbage collection instead.
STATIC bool bonding_write_block(bonding_block_type_t type, uint16_t ediv, uint8_t *data, uint16_t data_length) {
    size_t block_size = bonding_block_word_size(data_length);
    if (block_size > BONDING_DATA_END_ADDR - BONDING_DATA_START_ADDR) {
        // Ridiculous size.
        return false;
    }

    // No more room. Erase all existing bonding info and start over.
    if (bonding_unused_block == NULL || bonding_unused_block + block_size >= BONDING_DATA_END_ADDR) {
        bonding_erase_storage();
    }

    bonding_block_t block_without_data;
    block_without_data.valid = 1;
    block_without_data.type = type;
    block_without_data.ediv = ediv;
    block_without_data.data_length = data_length;

    // Write header data.
    nrf_nvmc_write_words((uint32_t) bonding_unused_block, (uint32_t *) &block_without_data,
                         sizeof(block_without_data) / 4);

    // Write variable-length data.
    // Minimize the number of writes. Datasheet says no more than two writes per word before erasing again.
    uint32_t *word_p = (uint32_t) bonding_unused_block + sizeof(block_without_data);
    while (1) {
        uint32_t word = 0xffffffff;
        memcpy(&word, data, data_length >= 4 ? 4 : data_length);
        nrf_nvmc_write_word(word_p, word);
        if (data_length <= 4) {
            break;
        }
        data_length -= 4;
        word_p++;
    }
    return true;
}



bool bonding_load_cccd_info(bool is_central, uint16_t conn_handle, uint16_t ediv,
                            uint8_t **sys_attr, uint16_t *sys_attr_len) {
    bonding_block_t *block = bonding_find_matching_block(BLOCK_SYS_ATTR, ediv);
    if (block) {
        *sys_attr = block.data;
        *sys_attr_len = block.data_length;
        return true;
    } else {
        return false;
    }
}

bool bonding_load_keys(bool is_central, uint16_t ediv, bonding_keys_t *bonding_keys) {
    bonding_block_t *block = bonding_find_matching_block(BLOCK_SYS_ATTR, ediv);
    if (block) {
        memcpy(bonding_keys, block.data, block.data_length);
        return true;
    } else {
        return false;
    }
}

bool bonding_save_cccd_info_later(bool is_central, uint16_t conn_handle, uint16_t ediv, uint8_t *sys_attr, uint16_t sys_attr_len) {
    // save in id role/ediv
    // sys_attr

bool bonding_save_keys(bool is_central, uint16_t conn_handle, bonding_keys_t *bonding_keys) {
    // save in id role/ediv:
    // bonding keys
    // peer name, or if no name, then human-readable address
}
