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

#if BONDING_DEBUG
void bonding_print_block(bonding_block_t *block) {
    printf("at 0x%08lx: is_central: %1d, type: 0x%x, ediv: 0x%04x, data_length: %d\n",
        (uint32_t)block, block->is_central, block->type, block->ediv, block->data_length);
}

void bonding_print_keys(bonding_keys_t *keys) {
    for (size_t i = 0; i < sizeof(bonding_keys_t); i++) {
        printf("%x", ((uint8_t *)keys)[i]);
    }
    printf("\n");
}
#endif

STATIC size_t compute_block_size(uint16_t data_length) {
    // Round data size up to the nearest 32-bit address.
    return sizeof(bonding_block_t) + ((data_length + 3) & ~0x3);
}

void bonding_erase_storage(void) {
    // Erase all pages in the bonding area.
    for (uint32_t page_address = BONDING_PAGES_START_ADDR;
         page_address < BONDING_PAGES_END_ADDR;
         page_address += FLASH_PAGE_SIZE) {
        // Argument is page number, not address.
        sd_flash_page_erase_sync(page_address / FLASH_PAGE_SIZE);
    }
    // Write marker words at the beginning and the end of the bonding area.
    uint32_t flag = BONDING_FLAG;
    sd_flash_write_sync((uint32_t *)BONDING_START_FLAG_ADDR, &flag, 1);
    sd_flash_write_sync((uint32_t *)BONDING_END_FLAG_ADDR, &flag, 1);
}

// Given NULL to start or block address, return the address of the next valid block.
// The last block returned is the unused block at the end.
// Return NULL if we have run off the end of the bonding space.

STATIC bonding_block_t *next_block(bonding_block_t *block) {
    while (1) {
        // Advance to next block.
        if (block == NULL) {
            return (bonding_block_t *)BONDING_DATA_START_ADDR;
        } else if (block->type == BLOCK_UNUSED) {
            // Already at last block (the unused block).
            return NULL;
        }

        // Advance to next block.
        block = (bonding_block_t *)((uint8_t *)block + compute_block_size(block->data_length));

        if (block >= (bonding_block_t *)BONDING_DATA_END_ADDR) {
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
STATIC bonding_block_t *find_existing_block(bool is_central, bonding_block_type_t type, uint16_t ediv) {
    bonding_block_t *block = NULL;
    while (1) {
        block = next_block(block);
        if (block == NULL) {
            return NULL;
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

size_t bonding_peripheral_bond_count(void) {
    bonding_block_t *block = NULL;
    size_t count = 0;
    while (1) {
        block = next_block(block);
        if (block == NULL) {
            return count;
        }
        if (block->type != BLOCK_UNUSED && block->type != BLOCK_INVALID && !block->is_central) {
            count++;
        }
    }
}

// Get an empty block large enough to store data_length data.
STATIC bonding_block_t *find_unused_block(uint16_t data_length) {
    bonding_block_t *unused_block = find_existing_block(true, BLOCK_UNUSED, EDIV_INVALID);
    // If no more room, erase all existing blocks and start over.
    if (!unused_block ||
        (uint8_t *)unused_block + compute_block_size(data_length) >= (uint8_t *)BONDING_DATA_END_ADDR) {
        bonding_erase_storage();
        unused_block = (bonding_block_t *)BONDING_DATA_START_ADDR;
    }
    return unused_block;
}

// Set the header word to all 0's, to mark the block as invalid.
// We don't change data_length, so we can still skip over this block.
STATIC void invalidate_block(bonding_block_t *block) {
    uint32_t zero = 0;
    sd_flash_write_sync((uint32_t *)block, &zero, 1);
}

// Write bonding block header.
STATIC void write_block_header(bonding_block_t *dest_block, bonding_block_t *source_block_header) {
    sd_flash_write_sync((uint32_t *)dest_block, (uint32_t *)source_block_header, sizeof(bonding_block_t) / 4);
}

// Write variable-length data at end of bonding block.
STATIC void write_block_data(bonding_block_t *dest_block, uint8_t *data, uint16_t data_length) {
    // Minimize the number of writes. Datasheet says no more than two writes per word before erasing again.

    // Start writing after the current header.
    uint32_t *flash_word_p = (uint32_t *)((uint8_t *)dest_block + sizeof(bonding_block_t));
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

STATIC void write_sys_attr_block(bleio_connection_internal_t *connection) {
    uint16_t length = 0;
    // First find out how big a buffer we need, then fetch the data.
    if (sd_ble_gatts_sys_attr_get(connection->conn_handle, NULL, &length, SYS_ATTR_FLAGS) != NRF_SUCCESS) {
        return;
    }
    uint8_t sys_attr[length];
    if (sd_ble_gatts_sys_attr_get(connection->conn_handle, sys_attr, &length, SYS_ATTR_FLAGS) != NRF_SUCCESS) {
        return;
    }

    // Is there an existing sys_attr block that matches the current sys_attr data?
    bonding_block_t *existing_block =
        find_existing_block(connection->is_central, BLOCK_SYS_ATTR, connection->ediv);
    if (existing_block) {
        if (length == existing_block->data_length &&
            memcmp(sys_attr, existing_block->data, length) == 0) {
            // Identical block found. No need to store again.
            return;
        }
        // Data doesn't match. Invalidate block and store a new one.
        invalidate_block(existing_block);
    }

    bonding_block_t block_header = {
        .is_central = connection->is_central,
        .type = BLOCK_SYS_ATTR,
        .ediv = connection->ediv,
        .conn_handle = connection->conn_handle,
        .data_length = length,
    };
    bonding_block_t *new_block = find_unused_block(length);
    write_block_header(new_block, &block_header);
    write_block_data(new_block, sys_attr, length);
    return;
}

STATIC void write_keys_block(bleio_connection_internal_t *connection) {
    uint16_t const ediv = connection->is_central
        ? connection->bonding_keys.peer_enc.master_id.ediv
        : connection->bonding_keys.own_enc.master_id.ediv;

    // Is there an existing keys block that matches the ediv?
    bonding_block_t *existing_block = find_existing_block(connection->is_central, BLOCK_KEYS, ediv);
    if (existing_block) {
        if (existing_block->data_length == sizeof(bonding_keys_t) &&
            memcmp(existing_block->data, &connection->bonding_keys, sizeof(bonding_keys_t)) == 0) {
            // Identical block found. No need to store again.
            return;
        }
        // Data doesn't match. Invalidate block and store a new one.
        invalidate_block(existing_block);
    }
    // Invalidate any existing blocks that match the peer address.
    existing_block = next_block(NULL);
    while (existing_block != NULL) {
        if (existing_block->type == BLOCK_KEYS && connection->is_central == existing_block->is_central &&
            existing_block->data_length == sizeof(bonding_keys_t)) {
            const ble_gap_addr_t *existing_peer = &((const bonding_keys_t *)existing_block->data)->peer_id.id_addr_info;
            const ble_gap_addr_t *connecting_peer = &connection->bonding_keys.peer_id.id_addr_info;
            if (memcmp(existing_peer->addr, connecting_peer->addr, 6) == 0 &&
                memcmp(existing_block->data, &connection->bonding_keys, sizeof(bonding_keys_t)) != 0) {
                // Mismatched block found. Invalidate it.
                invalidate_block(existing_block);
            }
        }
        existing_block = next_block(existing_block);
    }

    bonding_block_t block_header = {
        .is_central = connection->is_central,
        .type = BLOCK_KEYS,
        .ediv = ediv,
        .conn_handle = connection->conn_handle,
        .data_length = sizeof(bonding_keys_t),
    };
    bonding_block_t *new_block = find_unused_block(sizeof(bonding_keys_t));
    write_block_header(new_block, &block_header);
    write_block_data(new_block, (uint8_t *)&connection->bonding_keys, sizeof(bonding_keys_t));
}

void bonding_clear_keys(bonding_keys_t *bonding_keys) {
    memset((uint8_t *)bonding_keys, 0, sizeof(bonding_keys_t));
}

void bonding_reset(void) {
    if (BONDING_FLAG != *((uint32_t *)BONDING_START_FLAG_ADDR) ||
        BONDING_FLAG != *((uint32_t *)BONDING_END_FLAG_ADDR)) {
        bonding_erase_storage();
    }
}

// Write bonding blocks to flash. Requests have been queued during evt handlers.
void bonding_background(void) {
    // A paired connection will request that its keys and CCCD values be stored.
    // The CCCD store  whenever a CCCD value is written.
    for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
        bleio_connection_internal_t *connection = &bleio_connections[i];

        // Wait at least one second before saving CCCD, to consolidate
        // writes that involve multiple CCCDs. For instance, for HID,
        // three CCCD's are set in short succession by the HID client.
        if (connection->do_bond_cccds) {
            uint64_t current_ticks_ms = supervisor_ticks_ms64();
            if (current_ticks_ms - connection->do_bond_cccds_request_time >= 1000) {
                write_sys_attr_block(connection);
                connection->do_bond_cccds = false;
            }
        }

        if (connection->do_bond_keys) {
            write_keys_block(connection);
            connection->do_bond_keys = false;
        }
    }
}

bool bonding_load_cccd_info(bool is_central, uint16_t conn_handle, uint16_t ediv) {
    bonding_block_t *block = find_existing_block(is_central, BLOCK_SYS_ATTR, ediv);
    if (block == NULL) {
        return false;
    }

    return NRF_SUCCESS ==
           sd_ble_gatts_sys_attr_set(conn_handle, block->data, block->data_length, SYS_ATTR_FLAGS);
}

bool bonding_load_keys(bool is_central, uint16_t ediv, bonding_keys_t *bonding_keys) {
    bonding_block_t *block = find_existing_block(is_central, BLOCK_KEYS, ediv);
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

size_t bonding_load_identities(bool is_central, const ble_gap_id_key_t **keys, size_t max_length) {
    bonding_block_t *block = NULL;
    size_t len = 0;
    while (len < max_length) {
        block = next_block(block);
        if (block == NULL) {
            return len;
        }
        if (block->type != BLOCK_UNUSED &&
            block->type != BLOCK_INVALID &&
            block->is_central == is_central) {
            if (sizeof(bonding_keys_t) != block->data_length) {
                // bonding_keys_t is a fixed length, so lengths should match.
                return len;
            }
            const bonding_keys_t *key_set = (const bonding_keys_t *)block->data;
            keys[len] = &key_set->peer_id;
            len++;
        }
    }
    return len;
}

const ble_gap_enc_key_t *bonding_load_peer_encryption_key(bool is_central, const ble_gap_addr_t *peer) {
    bonding_block_t *block = next_block(NULL);
    while (block != NULL) {
        if (block->type == BLOCK_KEYS && block->is_central == is_central) {
            const bonding_keys_t *key_set = (const bonding_keys_t *)block->data;
            if (memcmp(key_set->peer_id.id_addr_info.addr, peer->addr, 6) == 0) {
                return &key_set->peer_enc;
            }
        }
        block = next_block(block);
    }
    return NULL;
}
