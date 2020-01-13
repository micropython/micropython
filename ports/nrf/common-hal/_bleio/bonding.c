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
#include "supervisor/shared/tick.h"

#include "nrf_soc.h"
#include "sd_mutex.h"

#include "bonding.h"

// Internal flash area reserved for bonding storage.
#define BONDING_PAGES_START_ADDR CIRCUITPY_BLE_CONFIG_START_ADDR
#define BONDING_PAGES_END_ADDR   (CIRCUITPY_BLE_CONFIG_START_ADDR + CIRCUITPY_BLE_CONFIG_SIZE)

// First and last four bytes are magic bytes for id and version. Data is in between.
// 'BD01'
const uint32_t BONDING_FLAG = ('1' | '0' << 8 | 'D' << 16 | 'B' << 24);

#define BONDING_DATA_START_ADDR (BONDING_PAGES_START_ADDR + sizeof(BONDING_FLAG))
#define BONDING_DATA_END_ADDR (BONDING_PAGES_END_ADDR - sizeof(BONDING_FLAG))

#define BONDING_START_FLAG_ADDR BONDING_PAGES_START_ADDR
#define BONDING_END_FLAG_ADDR BONDING_DATA_END_ADDR

// Save both system and user service info.
#define SYS_ATTR_FLAGS      (BLE_GATTS_SYS_ATTR_FLAG_SYS_SRVCS | BLE_GATTS_SYS_ATTR_FLAG_USR_SRVCS)

STATIC nrf_mutex_t queued_bonding_block_entries_mutex;
STATIC uint64_t block_queued_at_ticks_ms = 0;

#if BONDING_DEBUG
void bonding_print_block(bonding_block_t *block) {
    printf("at 0x%08lx: is_central: %1d, type: 0x%x, ediv: 0x%04x, data_length: %d\n",
               (uint32_t) block, block->is_central, block->type, block->ediv, block->data_length);
}

void bonding_print_keys(bonding_keys_t *keys) {
    for (size_t i = 0; i < sizeof(bonding_keys_t); i ++) {
        printf("%x", ((uint8_t*) keys)[i]);
    }
    printf("\n");
}
#endif

STATIC size_t compute_block_size(uint16_t data_length) {
    // Round data size up to the nearest 32-bit address.
    return sizeof(bonding_block_t) + ((data_length + 3) & ~0x3);
}

void bonding_erase_storage(void) {
    BONDING_DEBUG_PRINTF("bonding_erase_storage()\n");
    // Erase all pages in the bonding area.
    for(uint32_t page_address = BONDING_PAGES_START_ADDR;
        page_address < BONDING_PAGES_END_ADDR;
        page_address += FLASH_PAGE_SIZE) {
        // Argument is page number, not address.
        sd_flash_page_erase_sync(page_address / FLASH_PAGE_SIZE);
    }
    // Write marker words at the beginning and the end of the bonding area.
    uint32_t flag = BONDING_FLAG;
    sd_flash_write_sync((uint32_t *) BONDING_START_FLAG_ADDR, &flag, 1);
    sd_flash_write_sync((uint32_t *) BONDING_END_FLAG_ADDR, &flag, 1);
}

// Given NULL to start or block address, return the address of the next valid block.
// The last block returned is the unused block at the end.
// Return NULL if we have run off the end of the bonding space.
STATIC bonding_block_t *next_block(bonding_block_t *block) {
    while (1) {
        // Advance to next block.
        if (block == NULL) {
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

// Find the block with given is_central, type and ediv value.
// If type == BLOCK_UNUSED, ediv is ignored and the the sole unused block at the end is returned.
// If not found, return NULL.
STATIC bonding_block_t *find_candidate_block(bool is_central, bonding_block_type_t type, uint16_t ediv) {
    bonding_block_t *block = NULL;
    while (1) {
        block = next_block(block);
        if (block == NULL) {
            return NULL;
        }
        if (block->type == BLOCK_INVALID) {
            // Skip discarded blocks.
            continue;
        }
        // If types match, and block is unused, just return it.
        // Otherwise check that is_central and ediv match.
        if (type == block->type) {
            if (type == BLOCK_UNUSED ||
                (is_central == block->is_central && ediv == block->ediv)) {
            return block;
            }
        }
    }
}

// Get an empty block large enough to store data_length data.
STATIC bonding_block_t* find_unused_block(uint16_t data_length) {
    bonding_block_t *unused_block = find_candidate_block(true, BLOCK_UNUSED, EDIV_INVALID);
    // If no more room, erase all existing blocks and start over.
    if (!unused_block ||
        (uint8_t *) unused_block + compute_block_size(data_length) >= (uint8_t *) BONDING_DATA_END_ADDR) {
        bonding_erase_storage();
        unused_block = (bonding_block_t *) BONDING_DATA_START_ADDR;
    }
    return unused_block;
}

// Set the header word to all 0's, to mark the block as invalid.
// We don't change data_length, so we can still skip over this block.
STATIC void invalidate_block(bonding_block_t *block) {
    BONDING_DEBUG_PRINTF("invalidate_block()\n");
    uint32_t zero = 0;
    sd_flash_write_sync((uint32_t *) block, &zero, 1);
}

STATIC void queue_write_block(bool is_central, bonding_block_type_t type, uint16_t ediv, uint16_t conn_handle, uint8_t *data, uint16_t data_length) {
    if (compute_block_size(data_length) > BONDING_DATA_END_ADDR - BONDING_DATA_START_ADDR) {
        // Ridiculous size.
        return;
    }

    // No heap available, so never mind. This might be called between VM instantiations.
    if (!gc_alloc_possible()) {
        return;
    }
    queued_bonding_block_entry_t* queued_entry =
        m_malloc_maybe(sizeof(queued_bonding_block_entry_t) + data_length, false);

    if (!queued_entry) {
        // Failed to allocate. Not much we can do, since this might be during an evt handler.
        return;
    }

    queued_entry->block.is_central = is_central;
    queued_entry->block.type = type;
    queued_entry->block.ediv = ediv;
    queued_entry->block.conn_handle = conn_handle;
    queued_entry->block.data_length = data_length;
    if (data && data_length != 0) {
        memcpy(&queued_entry->block.data, data, data_length);
    }

    // Note: blocks are added in LIFO order, for simplicity and speed.
    // The assumption is that there won't be stale blocks on the
    // list. The sys_attr blocks don't contain sys_attr data, just a
    // request to store the latest value. The key blocks are assumed
    // not to be superseded quickly.  If this assumption becomes
    // invalid, the queue should be changed to FIFO.

    // Add this new element to the front of the list.
    sd_mutex_acquire_wait(&queued_bonding_block_entries_mutex);
    queued_entry->next = MP_STATE_VM(queued_bonding_block_entries);
    MP_STATE_VM(queued_bonding_block_entries) = queued_entry;
    sd_mutex_release(&queued_bonding_block_entries_mutex);

    // Remember when we last queued a block, so we avoid excesive
    // sys_attr writes.
    block_queued_at_ticks_ms = supervisor_ticks_ms64();
}

// Write bonding block header.
STATIC void write_block_header(bonding_block_t *dest_block, bonding_block_t *source_block_header) {
    sd_flash_write_sync((uint32_t *) dest_block, (uint32_t *) source_block_header, sizeof(bonding_block_t) / 4);
}

// Write variable-length data at end of bonding block.
STATIC void write_block_data(bonding_block_t *dest_block, uint8_t *data, uint16_t data_length) {
    // Minimize the number of writes. Datasheet says no more than two writes per word before erasing again.

    // Start writing after the current header.
    uint32_t *flash_word_p = (uint32_t *) ((uint8_t *) dest_block + sizeof(bonding_block_t));
    while (1) {
        uint32_t word = 0xffffffff;
        memcpy(&word, data, data_length >= 4 ? 4 : data_length);
        sd_flash_write_sync(flash_word_p, &word, 1);
        if (data_length <= 4) {
            break;
        }
        data_length -= 4;
        data += 4;
        // Increment by word size.
        flash_word_p++;
    }
}

STATIC void write_sys_attr_block(bonding_block_t *block) {
    uint16_t length = 0;
    // First find out how big a buffer we need, then fetch the data.
    if(sd_ble_gatts_sys_attr_get(block->conn_handle, NULL, &length, SYS_ATTR_FLAGS) != NRF_SUCCESS) {
        return;
    }

    uint8_t sys_attr[length];
    if(sd_ble_gatts_sys_attr_get(block->conn_handle, sys_attr, &length, SYS_ATTR_FLAGS) != NRF_SUCCESS) {
        return;
    }

    // Now we know the data size.
    block->data_length = length;

    // Is there an existing sys_attr block that matches the current sys_attr data?
    bonding_block_t *candidate_block = find_candidate_block(block->is_central, block->type, block->ediv);
    if (candidate_block) {
        if (length == candidate_block->data_length &&
            memcmp(sys_attr, candidate_block->data, block->data_length) == 0) {
            BONDING_DEBUG_PRINTF("Identical sys_attr block already stored.\n");
            // Identical block found. No need to store again.
            return;
        }
        // Data doesn't match. Invalidate block and store a new one.
        invalidate_block(candidate_block);
    }

    bonding_block_t *new_block = find_unused_block(length);
    write_block_header(new_block, block);
    write_block_data(new_block, sys_attr, length);
    return;
}

STATIC void write_keys_block(bonding_block_t *block) {
    if (block->data_length != sizeof(bonding_keys_t)) {
        // Bad length.
        return;
    }

    // Is there an existing keys block that matches?
    bonding_block_t *candidate_block = find_candidate_block(block->is_central, block->type, block->ediv);
    if (candidate_block) {
        if (block->data_length == candidate_block->data_length &&
            memcmp(block->data, candidate_block->data, block->data_length) == 0) {
            BONDING_DEBUG_PRINTF("Identical keys block already stored.\n");
            // Identical block found. No need to store again.
            return;
        }
        // Data doesn't match. Invalidate block and store a new one.
        invalidate_block(candidate_block);
    }

    bonding_keys_t *bonding_keys = (bonding_keys_t *) block->data;
    block->ediv = block->is_central
        ? bonding_keys->peer_enc.master_id.ediv
        : bonding_keys->own_enc.master_id.ediv;
    bonding_block_t *new_block = find_unused_block(sizeof(bonding_keys_t));
    write_block_header(new_block, block);
    write_block_data(new_block, (uint8_t *) bonding_keys, sizeof(bonding_keys_t));
}


void bonding_clear_keys(bonding_keys_t *bonding_keys) {
    memset((uint8_t*) bonding_keys, 0, sizeof(bonding_keys_t));
}

// Call only when SD is enabled.
void bonding_reset(void) {
    MP_STATE_VM(queued_bonding_block_entries) = NULL;
    sd_mutex_new(&queued_bonding_block_entries_mutex);
    if (BONDING_FLAG != *((uint32_t *) BONDING_START_FLAG_ADDR) ||
        BONDING_FLAG != *((uint32_t *) BONDING_END_FLAG_ADDR)) {
        bonding_erase_storage();
    }
}

// Write bonding blocks to flash. These have been queued during event handlers.
// We do one at a time, on each background call.
void bonding_background(void) {
    uint8_t sd_en = 0;
    (void) sd_softdevice_is_enabled(&sd_en);
    if (!sd_en) {
        return;
    }

    if (block_queued_at_ticks_ms == 0) {
        // No writes have been queued yet.
        return;
    }

    // Wait at least one second before writing a block, to consolidate writes
    // that will be duplicates.
    uint64_t current_ticks_ms = supervisor_ticks_ms64();
    if (current_ticks_ms - block_queued_at_ticks_ms < 1000) {
        return;
    }

    // Get block at front of list.
    bonding_block_t *block = NULL;
    sd_mutex_acquire_wait(&queued_bonding_block_entries_mutex);
    if (MP_STATE_VM(queued_bonding_block_entries)) {
        block = &(MP_STATE_VM(queued_bonding_block_entries)->block);
        // Remove entry from list.
        MP_STATE_VM(queued_bonding_block_entries) = MP_STATE_VM(queued_bonding_block_entries)->next;
    }
    sd_mutex_release(&queued_bonding_block_entries_mutex);
    if (!block) {
        // List is empty.
        return;
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
    bonding_block_t *block = find_candidate_block(is_central, BLOCK_SYS_ATTR, ediv);
    if (block == NULL) {
        return false;
    }

    return NRF_SUCCESS ==
        sd_ble_gatts_sys_attr_set(conn_handle, block->data, block->data_length, SYS_ATTR_FLAGS);
}

bool bonding_load_keys(bool is_central, uint16_t ediv, bonding_keys_t *bonding_keys) {
    bonding_block_t *block = find_candidate_block(is_central, BLOCK_KEYS, ediv);
    if (block == NULL) {
        return false;
    }
    if (sizeof(bonding_keys_t) != block->data_length) {
        // bonding_keys_t is a fixed length, so lengths should match.
        return false;
    }

    memcpy(bonding_keys, block->data, block->data_length);
    return true;
}

void bonding_save_cccd_info(bool is_central, uint16_t conn_handle, uint16_t ediv) {
    BONDING_DEBUG_PRINTF("bonding_save_cccd_info()\n");
    queue_write_block(is_central, BLOCK_SYS_ATTR, ediv, conn_handle, NULL, 0);
}

void bonding_save_keys(bool is_central, uint16_t conn_handle, bonding_keys_t *bonding_keys) {
    uint16_t const ediv = is_central
        ? bonding_keys->peer_enc.master_id.ediv
        : bonding_keys->own_enc.master_id.ediv;
    queue_write_block(is_central, BLOCK_KEYS, ediv, conn_handle, (uint8_t *) bonding_keys, sizeof(bonding_keys_t));
}
