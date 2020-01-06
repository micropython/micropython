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

#include "nrf_nvmc.h"
#include "sd_mutex.h"

#include "bonding.h"

// Internal flash area reserved for bonding storage.
#define BONDING_PAGES_START_ADDR CIRCUITPY_BLE_CONFIG_START_ADDR
#define BONDING_PAGES_END_ADDR   (CIRCUITPY_BLE_CONFIG_START_ADDR + CIRCUITPY_BLE_CONFIG_SIZE)

// First and last four bytes are magic bytes for id and version. Start data after that.
// 'BD01'
const uint32_t BONDING_START_FLAG = ('1' | '0' << 8 | 'D' << 16 | 'B' << 24);
// 'ED01'
const uint32_t BONDING_END_FLAG = ('1' | '0' << 8 | 'D' << 16 | 'E' << 24);

#define BONDING_DATA_START_ADDR (BONDING_PAGES_START_ADDR + sizeof(BONDING_START_FLAG))
#define BONDING_DATA_END_ADDR (BONDING_PAGES_END_ADDR - sizeof(BONDING_END_FLAG))

#define BONDING_START_FLAG_ADDR BONDING_PAGES_START_ADDR
#define BONDING_END_FLAG_ADDR BONDING_DATA_END_ADDR

// Save both system and user service info.
#define SYS_ATTR_FLAGS      (BLE_GATTS_SYS_ATTR_FLAG_SYS_SRVCS | BLE_GATTS_SYS_ATTR_FLAG_USR_SRVCS)

STATIC bonding_block_t *bonding_unused_block = NULL;
nrf_mutex_t queued_bonding_block_list_mutex;

STATIC inline size_t compute_block_size(uint16_t data_length) {
    // Round data size up to the nearest 32-bit address.
    return sizeof(bonding_block_t) + ((data_length + 3) & 0x3);
}

STATIC void erase_bonding_storage(void) {
    // Erase all pages in the bonding area.
    BONDING_DEBUG_PRINTF("erase_bonding_storage()\n");
    for(uint32_t page_address = BONDING_PAGES_START_ADDR;
        page_address < BONDING_PAGES_END_ADDR;
        page_address += FLASH_PAGE_SIZE) {
        nrf_nvmc_page_erase(page_address);
    }
    // Write marker words at the beginning and the end of the bonding area.
    nrf_nvmc_write_word(BONDING_DATA_START_ADDR, BONDING_START_FLAG_ADDR);
    nrf_nvmc_write_word(BONDING_DATA_END_ADDR, BONDING_END_FLAG_ADDR);
    // First unused block is at the beginning.
    bonding_unused_block = (bonding_block_t *) BONDING_DATA_START_ADDR;
}

// Given NULL to start or block address, return the address of the next valid block.
// The last block returned is the unused block at the end.
// Return NULL if we have run off the end of the bonding space.
STATIC bonding_block_t *next_block(bonding_block_t *block) {
    while (1) {
        // Advance to next block.
        if (block == NULL) {
            // Return first block (which might be unused if block list is empty).
            return (bonding_block_t *) BONDING_DATA_START_ADDR;
        } else if (block->type == BLOCK_UNUSED) {
            // Already at last block (the unused block).
            return NULL;
        }

        // Advance to next block.
        block = (bonding_block_t *) ((uint8_t *) block + compute_block_size(block->data_length));

        if (block >= (bonding_block_t *) BONDING_DATA_END_ADDR) {
            // Went past end of bonding space.
            return NULL;
        }
        if (block->type != BLOCK_INVALID) {
            // Found an empty or a valid block.
            return block;
        }
        // Invalid block (was erased); try again.
    }
}

// Find the block with given type and ediv value.
// If type == BLOCK_UNUSED, ediv is ignored and the the sole unused block at the end is returned.
// If not found, return NULL.
STATIC bonding_block_t *find_block(bool is_central, bonding_block_type_t type, uint16_t ediv) {
    bonding_block_t *block = NULL;
    while (1) {
        block = next_block(block);
        if (block == NULL) {
            return NULL;
        }
        if (block->type == BLOCK_UNUSED) {
            return block;
        }
        if (is_central == block->is_central &&
            type == block->type &&
            ediv == block->ediv) {
            return block;
        }
    }
}

// Set the header word to all 0's, to mark the block as invalid.
// We don't change data_length, so we can still skip over this block.
STATIC void invalidate_block(bonding_block_t *block) {
    BONDING_DEBUG_PRINTF("invalidate_block()\n");
    nrf_nvmc_write_word((uint32_t) block, 0x00000000);
}

STATIC void queue_write_block(bool is_central, bonding_block_type_t type, uint16_t ediv, uint16_t conn_handle, uint8_t *data, uint16_t data_length) {
    if (compute_block_size(data_length) > BONDING_DATA_END_ADDR - BONDING_DATA_START_ADDR) {
        // Ridiculous size.
        return;
    }

    queued_bonding_block_list_elt_t* queued_elt =
        m_malloc_maybe(sizeof(queued_bonding_block_list_elt_t) + data_length, false);

    if (!queued_elt) {
        // Failed to allocate. Not much we can do, since this might be during an evt handler.
        return;
    }

    // Add this new element to the front of the list.
    sd_mutex_acquire_wait(&queued_bonding_block_list_mutex);
    queued_elt->next_queued_block = MP_STATE_VM(queued_bonding_block_list);
    MP_STATE_VM(queued_bonding_block_list) = queued_elt;
    sd_mutex_release(&queued_bonding_block_list_mutex);

    //
    queued_elt->bonding_block.is_central = is_central;
    queued_elt->bonding_block.type = type;
    queued_elt->bonding_block.ediv = ediv;
    queued_elt->bonding_block.conn_handle = conn_handle;
    queued_elt->bonding_block.data_length = data_length;
    if (data && data_length != 0) {
        memcpy(&queued_elt->bonding_block.data, data, data_length);
    }
}

// Write bonding block header.
STATIC void write_block_header(bonding_block_t *block) {
    // If no more room, erase all existing blocks and start over.
    if (bonding_unused_block == NULL ||
        (uint8_t *) bonding_unused_block + compute_block_size(block->data_length) >=
        (uint8_t *)BONDING_DATA_END_ADDR) {
        erase_bonding_storage();
    }

    nrf_nvmc_write_words((uint32_t) bonding_unused_block, (uint32_t *) block, sizeof(bonding_block_t) / 4);
}

// Write variable-length data at end of bonding block.
STATIC void write_block_data(uint8_t *data, uint16_t data_length) {
    // Minimize the number of writes. Datasheet says no more than two writes per word before erasing again.

    // Start writing after the current header.
    uint32_t *flash_word_p = (uint32_t *) ((uint8_t *) bonding_unused_block + sizeof(bonding_block_t));
    while (1) {
        uint32_t word = 0xffffffff;
        memcpy(&word, data, data_length >= 4 ? 4 : data_length);
        nrf_nvmc_write_word((uint32_t) flash_word_p, word);
        if (data_length <= 4) {
            break;
        }
        data_length -= 4;
        // Increment by word size.
        flash_word_p++;
    }
    bonding_unused_block = (bonding_block_t *) flash_word_p;
}

STATIC bool write_sys_attr_block(bonding_block_t *block) {
    BONDING_DEBUG_PRINTF("write_sys_attr_block()\n");
    uint16_t length = 0;
    // First find out how big a buffer we need, then fetch the data.
    if(sd_ble_gatts_sys_attr_get(block->conn_handle, NULL, &length, SYS_ATTR_FLAGS) != NRF_SUCCESS) {
        return false;
    }

    uint8_t sys_attr[length];
    if(sd_ble_gatts_sys_attr_get(block->conn_handle, sys_attr, &length, SYS_ATTR_FLAGS) != NRF_SUCCESS) {
        return false;
    }

    // Now we know the data size.
    block->data_length = length;
    write_block_header(block);
    write_block_data(sys_attr, length);
    return true;
}

STATIC bool write_keys_block(bonding_block_t *block) {
    BONDING_DEBUG_PRINTF("write_keys_block()\n");
    if (block->data_length != sizeof(bonding_keys_t)) {
        return false;
    }

    bonding_keys_t *bonding_keys = (bonding_keys_t *) block->data;
    block->ediv = block->is_central
        ? bonding_keys->peer_enc.master_id.ediv
        : bonding_keys->own_enc.master_id.ediv;

    write_block_header(block);
    write_block_data((uint8_t *) bonding_keys, sizeof(bonding_keys_t));
    return true;
}


void bonding_clear_keys(bonding_keys_t *bonding_keys) {
    memset((uint8_t*) bonding_keys, 0, sizeof(bonding_keys_t));
}

void bonding_reset(void) {
    BONDING_DEBUG_PRINTF("bonding_reset()\n");
    sd_mutex_new(&queued_bonding_block_list_mutex);
    if (BONDING_START_FLAG != *((uint32_t *) BONDING_START_FLAG_ADDR) ||
        BONDING_END_FLAG != *((uint32_t *) BONDING_END_FLAG_ADDR)) {
        erase_bonding_storage();
    } else {
        bonding_unused_block = find_block(true, BLOCK_UNUSED, EDIV_INVALID);
    }
}

// Write bonding blocks to flash. These have been queued during event handlers.
// We do one at a time, on each background call.
void bonding_background(void) {

    // Get block at front of list.
    sd_mutex_acquire_wait(&queued_bonding_block_list_mutex);
    bonding_block_t *block = &(MP_STATE_VM(queued_bonding_block_list)->bonding_block);
    if (block) {
        // Remove written block from front of list.
        MP_STATE_VM(queued_bonding_block_list) = MP_STATE_VM(queued_bonding_block_list)->next_queued_block;
    }
    sd_mutex_release(&queued_bonding_block_list_mutex);

    if (!block) {
        // No blocks in queue.
        return;
    }

    // Is there an existing block whose keys match?
    bonding_block_t *matching_block = find_block(block->is_central, block->type, block->ediv);
    if (matching_block) {
        if (block->data_length == matching_block->data_length &&
            memcmp(block->data, matching_block->data, block->data_length) == 0) {
            // Identical block found. No need to store again.
            BONDING_DEBUG_PRINTF("bonding_background(): identical block found\n");
            return;
        }
        // Block keys match but data doesn't. Invalidate block and store a new one.
        BONDING_DEBUG_PRINTF("bonding_background(): invalidating block\n");
        invalidate_block(matching_block);
    }

    switch (block->type) {
        case BLOCK_SYS_ATTR:
            write_sys_attr_block(block);
            break;

        case BLOCK_KEYS:
            write_keys_block(block);
            break;

        default:
            break;
    }
}

bool bonding_load_cccd_info(bool is_central, uint16_t conn_handle, uint16_t ediv) {
    bonding_block_t *block = find_block(is_central, BLOCK_SYS_ATTR, ediv);
    if (block == NULL) {
        BONDING_DEBUG_PRINTF("bonding_load_cccd_info(): block not found\n");
        return false;
    }

    BONDING_DEBUG_PRINTF("bonding_load_cccd_info(): block found\n");
    return NRF_SUCCESS ==
        sd_ble_gatts_sys_attr_set(conn_handle, block->data, block->data_length, SYS_ATTR_FLAGS);
}

bool bonding_load_keys(bool is_central, uint16_t ediv, bonding_keys_t *bonding_keys) {
    bonding_block_t *block = find_block(is_central, BLOCK_SYS_ATTR, ediv);
    if (block == NULL) {
        BONDING_DEBUG_PRINTF("bonding_load_keys(): block not found\n");
        return false;
    }
    if (sizeof(bonding_keys_t) != block->data_length) {
        // bonding_keys_t is a fixed length, so lengths should match.
        return false;
    }

    BONDING_DEBUG_PRINTF("bonding_load_keys(): block found\n");
    memcpy(bonding_keys, block->data, block->data_length);
    return true;
}

void bonding_save_cccd_info(bool is_central, uint16_t conn_handle, uint16_t ediv) {
    queue_write_block(is_central, ediv, conn_handle, BLOCK_SYS_ATTR, NULL, 0);
}

void bonding_save_keys(bool is_central, uint16_t conn_handle, bonding_keys_t *bonding_keys) {
    uint16_t const ediv = is_central
        ? bonding_keys->peer_enc.master_id.ediv
        : bonding_keys->own_enc.master_id.ediv;
    queue_write_block(is_central, BLOCK_KEYS, ediv, conn_handle, (uint8_t *) bonding_keys, sizeof(bonding_keys_t));
}
