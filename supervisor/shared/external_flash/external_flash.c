/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016, 2017 Scott Shawcroft for Adafruit Industries
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
#include "supervisor/shared/external_flash/external_flash.h"

#include <stdint.h>
#include <string.h>
#include "genhdr/devices.h"
#include "supervisor/flash.h"
#include "supervisor/spi_flash_api.h"
#include "supervisor/shared/external_flash/common_commands.h"
#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"
#include "py/misc.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "lib/oofatfs/ff.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "supervisor/memory.h"

#define NO_SECTOR_LOADED 0xFFFFFFFF

// The currently cached sector in the cache, ram or flash based.
static uint32_t current_sector;

STATIC const external_flash_device possible_devices[] = {EXTERNAL_FLASH_DEVICES};
#define EXTERNAL_FLASH_DEVICE_COUNT MP_ARRAY_SIZE(possible_devices)

static const external_flash_device *flash_device = NULL;

// Track which blocks (up to 32) in the current sector currently live in the
// cache.
static uint32_t dirty_mask;

static supervisor_allocation *supervisor_cache = NULL;

// Wait until both the write enable and write in progress bits have cleared.
static bool wait_for_flash_ready(void) {
    bool ok = true;
    // Both the write enable and write in progress bits should be low.
    if (flash_device->no_ready_bit) {
        // For NVM without a ready bit in status register
        return ok;
    }
    uint8_t read_status_response[1] = {0x00};
    do {
        ok = spi_flash_read_command(CMD_READ_STATUS, read_status_response, 1);
    } while (ok && (read_status_response[0] & 0x3) != 0);
    return ok;
}

// Turn on the write enable bit so we can program and erase the flash.
static bool write_enable(void) {
    return spi_flash_command(CMD_ENABLE_WRITE);
}

// Read data_length's worth of bytes starting at address into data.
static bool read_flash(uint32_t address, uint8_t *data, uint32_t data_length) {
    if (flash_device == NULL) {
        return false;
    }
    if (!wait_for_flash_ready()) {
        return false;
    }
    return spi_flash_read_data(address, data, data_length);
}

// Writes data_length's worth of bytes starting at address from data. Assumes
// that the sector that address resides in has already been erased. So make sure
// to run erase_sector.
static bool write_flash(uint32_t address, const uint8_t *data, uint32_t data_length) {
    if (flash_device == NULL) {
        return false;
    }
    // Don't bother writing if the data is all 1s. Thats equivalent to the flash
    // state after an erase.
    if (!flash_device->no_erase_cmd) {
        // Only do this if the device has an erase command
        bool all_ones = true;
        for (uint16_t i = 0; i < data_length; i++) {
            if (data[i] != 0xff) {
                all_ones = false;
                break;
            }
        }
        if (all_ones) {
            return true;
        }
    }

    for (uint32_t bytes_written = 0;
         bytes_written < data_length;
         bytes_written += SPI_FLASH_PAGE_SIZE) {
        if (!wait_for_flash_ready() || !write_enable()) {
            return false;
        }

        if (!spi_flash_write_data(address + bytes_written, (uint8_t *)data + bytes_written,
            SPI_FLASH_PAGE_SIZE)) {
            return false;
        }
    }
    return true;
}

static bool page_erased(uint32_t sector_address) {
    // Check the first few bytes to catch the common case where there is data
    // without using a bunch of memory.
    if (flash_device->no_erase_cmd) {
        // skip this if device doesn't have an erase command.
        return true;
    }
    uint8_t short_buffer[4];
    if (read_flash(sector_address, short_buffer, 4)) {
        for (uint16_t i = 0; i < 4; i++) {
            if (short_buffer[i] != 0xff) {
                return false;
            }
        }
    } else {
        return false;
    }

    // Now check the full length.
    uint8_t full_buffer[FILESYSTEM_BLOCK_SIZE];
    if (read_flash(sector_address, full_buffer, FILESYSTEM_BLOCK_SIZE)) {
        for (uint16_t i = 0; i < FILESYSTEM_BLOCK_SIZE; i++) {
            if (short_buffer[i] != 0xff) {
                return false;
            }
        }
    } else {
        return false;
    }
    return true;
}

// Erases the given sector. Make sure you copied all of the data out of it you
// need! Also note, sector_address is really 24 bits.
static bool erase_sector(uint32_t sector_address) {
    // Before we erase the sector we need to wait for any writes to finish and
    // and then enable the write again.
    if (flash_device->no_erase_cmd) {
        // skip this if device doesn't have an erase command.
        return true;
    }
    if (!wait_for_flash_ready() || !write_enable()) {
        return false;
    }
    if (flash_device->no_erase_cmd) {
        return true;
    }
    spi_flash_sector_command(CMD_SECTOR_ERASE, sector_address);
    return true;
}

// Sector is really 24 bits.
static bool copy_block(uint32_t src_address, uint32_t dest_address) {
    // Copy page by page to minimize RAM buffer.
    uint16_t page_size = SPI_FLASH_PAGE_SIZE;
    uint8_t buffer[page_size];
    for (uint32_t i = 0; i < FILESYSTEM_BLOCK_SIZE / page_size; i++) {
        if (!read_flash(src_address + i * page_size, buffer, page_size)) {
            return false;
        }
        if (!write_flash(dest_address + i * page_size, buffer, page_size)) {
            return false;
        }
    }
    return true;
}

void supervisor_flash_init(void) {
    if (flash_device != NULL) {
        return;
    }

    // Delay to give the SPI Flash time to get going.
    // TODO(tannewt): Only do this when we know power was applied vs a reset.
    uint16_t max_start_up_delay_us = 0;
    for (uint8_t i = 0; i < EXTERNAL_FLASH_DEVICE_COUNT; i++) {
        if (possible_devices[i].start_up_time_us > max_start_up_delay_us) {
            max_start_up_delay_us = possible_devices[i].start_up_time_us;
        }
    }
    common_hal_mcu_delay_us(max_start_up_delay_us);

    spi_flash_init();

    #ifdef EXTERNAL_FLASH_NO_JEDEC
    // For NVM that don't have JEDEC response
    spi_flash_command(CMD_WAKE);
    for (uint8_t i = 0; i < EXTERNAL_FLASH_DEVICE_COUNT; i++) {
        const external_flash_device *possible_device = &possible_devices[i];
        flash_device = possible_device;
        break;
    }
    #else
    // The response will be 0xff if the flash needs more time to start up.
    uint8_t jedec_id_response[3] = {0xff, 0xff, 0xff};
    while (jedec_id_response[0] == 0xff) {
        spi_flash_read_command(CMD_READ_JEDEC_ID, jedec_id_response, 3);
    }
    for (uint8_t i = 0; i < EXTERNAL_FLASH_DEVICE_COUNT; i++) {
        const external_flash_device *possible_device = &possible_devices[i];
        if (jedec_id_response[0] == possible_device->manufacturer_id &&
            jedec_id_response[1] == possible_device->memory_type &&
            jedec_id_response[2] == possible_device->capacity) {
            flash_device = possible_device;
            break;
        }
    }
    #endif
    if (flash_device == NULL) {
        return;
    }

    // We don't know what state the flash is in so wait for any remaining writes and then reset.
    uint8_t read_status_response[1] = {0x00};
    // The write in progress bit should be low.
    do {
        spi_flash_read_command(CMD_READ_STATUS, read_status_response, 1);
    } while ((read_status_response[0] & 0x1) != 0);
    if (!flash_device->single_status_byte) {
        // The suspended write/erase bit should be low.
        do {
            spi_flash_read_command(CMD_READ_STATUS2, read_status_response, 1);
        } while ((read_status_response[0] & 0x80) != 0);
    }

    if (!(flash_device->no_reset_cmd)) {
        spi_flash_command(CMD_ENABLE_RESET);
        spi_flash_command(CMD_RESET);
    }

    // Wait 30us for the reset
    common_hal_mcu_delay_us(30);

    spi_flash_init_device(flash_device);

    // Activity LED for flash writes.
    #ifdef MICROPY_HW_LED_MSC
    gpio_set_pin_function(SPI_FLASH_CS_PIN, GPIO_PIN_FUNCTION_OFF);
    gpio_set_pin_direction(MICROPY_HW_LED_MSC, GPIO_DIRECTION_OUT);
    // There's already a pull-up on the board.
    gpio_set_pin_level(MICROPY_HW_LED_MSC, false);
    #endif

    if (flash_device->has_sector_protection) {
        write_enable();

        // Turn off sector protection
        uint8_t data[1] = {0x00};
        spi_flash_write_command(CMD_WRITE_STATUS_BYTE1, data, 1);
    }

    // Turn off writes in case this is a microcontroller only reset.
    spi_flash_command(CMD_DISABLE_WRITE);

    wait_for_flash_ready();

    current_sector = NO_SECTOR_LOADED;
    dirty_mask = 0;
    MP_STATE_VM(flash_ram_cache) = NULL;
}

// The size of each individual block.
uint32_t supervisor_flash_get_block_size(void) {
    return FILESYSTEM_BLOCK_SIZE;
}

// The total number of available blocks.
uint32_t supervisor_flash_get_block_count(void) {
    // We subtract one erase sector size because we may use it as a staging area
    // for writes.
    return (flash_device->total_size - SPI_FLASH_ERASE_SIZE) / FILESYSTEM_BLOCK_SIZE;
}

// Flush the cache that was written to the scratch portion of flash. Only used
// when ram is tight.
static bool flush_scratch_flash(void) {
    if (current_sector == NO_SECTOR_LOADED) {
        return true;
    }
    // First, copy out any blocks that we haven't touched from the sector we've
    // cached.
    bool copy_to_scratch_ok = true;
    uint32_t scratch_sector = flash_device->total_size - SPI_FLASH_ERASE_SIZE;
    for (uint8_t i = 0; i < SPI_FLASH_ERASE_SIZE / FILESYSTEM_BLOCK_SIZE; i++) {
        if ((dirty_mask & (1 << i)) == 0) {
            copy_to_scratch_ok = copy_to_scratch_ok &&
                copy_block(current_sector + i * FILESYSTEM_BLOCK_SIZE,
                scratch_sector + i * FILESYSTEM_BLOCK_SIZE);
        }
    }
    if (!copy_to_scratch_ok) {
        // TODO(tannewt): Do more here. We opted to not erase and copy bad data
        // in. We still risk losing the data written to the scratch sector.
        return false;
    }
    // Second, erase the current sector.
    erase_sector(current_sector);
    // Finally, copy the new version into it.
    for (uint8_t i = 0; i < SPI_FLASH_ERASE_SIZE / FILESYSTEM_BLOCK_SIZE; i++) {
        copy_block(scratch_sector + i * FILESYSTEM_BLOCK_SIZE,
            current_sector + i * FILESYSTEM_BLOCK_SIZE);
    }
    return true;
}

// Attempts to allocate a new set of page buffers for caching a full sector in
// ram. Each page is allocated separately so that the GC doesn't need to provide
// one huge block. We can free it as we write if we want to also.
static bool allocate_ram_cache(void) {
    uint8_t blocks_per_sector = SPI_FLASH_ERASE_SIZE / FILESYSTEM_BLOCK_SIZE;
    uint8_t pages_per_block = FILESYSTEM_BLOCK_SIZE / SPI_FLASH_PAGE_SIZE;

    uint32_t table_size = blocks_per_sector * pages_per_block * sizeof(uint32_t);
    // Attempt to allocate outside the heap first.
    supervisor_cache = allocate_memory(table_size + SPI_FLASH_ERASE_SIZE, false, false);
    if (supervisor_cache != NULL) {
        MP_STATE_VM(flash_ram_cache) = (uint8_t **)supervisor_cache->ptr;
        uint8_t *page_start = (uint8_t *)supervisor_cache->ptr + table_size;

        for (uint8_t i = 0; i < blocks_per_sector; i++) {
            for (uint8_t j = 0; j < pages_per_block; j++) {
                uint32_t offset = i * pages_per_block + j;
                MP_STATE_VM(flash_ram_cache)[offset] = page_start + offset * SPI_FLASH_PAGE_SIZE;
            }
        }
        return true;
    }

    if (MP_STATE_MEM(gc_pool_start) == 0) {
        return false;
    }

    MP_STATE_VM(flash_ram_cache) = m_malloc_maybe(blocks_per_sector * pages_per_block * sizeof(uint32_t), false);
    if (MP_STATE_VM(flash_ram_cache) == NULL) {
        return false;
    }
    // Declare i and j outside the loops in case we fail to allocate everything
    // we need. In that case we'll give it back.
    uint8_t i = 0;
    uint8_t j = 0;
    bool success = true;
    for (i = 0; i < blocks_per_sector; i++) {
        for (j = 0; j < pages_per_block; j++) {
            uint8_t *page_cache = m_malloc_maybe(SPI_FLASH_PAGE_SIZE, false);
            if (page_cache == NULL) {
                success = false;
                break;
            }
            MP_STATE_VM(flash_ram_cache)[i * pages_per_block + j] = page_cache;
        }
        if (!success) {
            break;
        }
    }
    // We couldn't allocate enough so give back what we got.
    if (!success) {
        // We add 1 so that we delete 0 when i is 1. Going to zero (i >= 0)
        // would never stop because i is unsigned.
        i++;
        for (; i > 0; i--) {
            for (; j > 0; j--) {
                m_free(MP_STATE_VM(flash_ram_cache)[(i - 1) * pages_per_block + (j - 1)]);
            }
            j = pages_per_block;
        }
        m_free(MP_STATE_VM(flash_ram_cache));
        MP_STATE_VM(flash_ram_cache) = NULL;
    }
    return success;
}

static void release_ram_cache(void) {
    if (supervisor_cache != NULL) {
        free_memory(supervisor_cache);
        supervisor_cache = NULL;
    } else if (MP_STATE_MEM(gc_pool_start)) {
        m_free(MP_STATE_VM(flash_ram_cache));
    }
    MP_STATE_VM(flash_ram_cache) = NULL;
}

// Flush the cached sector from ram onto the flash. We'll free the cache unless
// keep_cache is true.
static bool flush_ram_cache(bool keep_cache) {
    if (current_sector == NO_SECTOR_LOADED) {
        if (!keep_cache) {
            release_ram_cache();
        }
        return true;
    }
    // First, copy out any blocks that we haven't touched from the sector
    // we've cached. If we don't do this we'll erase the data during the sector
    // erase below.
    bool copy_to_ram_ok = true;
    uint8_t pages_per_block = FILESYSTEM_BLOCK_SIZE / SPI_FLASH_PAGE_SIZE;
    for (uint8_t i = 0; i < SPI_FLASH_ERASE_SIZE / FILESYSTEM_BLOCK_SIZE; i++) {
        if ((dirty_mask & (1 << i)) == 0) {
            for (uint8_t j = 0; j < pages_per_block; j++) {
                copy_to_ram_ok = read_flash(
                    current_sector + (i * pages_per_block + j) * SPI_FLASH_PAGE_SIZE,
                    MP_STATE_VM(flash_ram_cache)[i * pages_per_block + j],
                    SPI_FLASH_PAGE_SIZE);
                if (!copy_to_ram_ok) {
                    break;
                }
            }
        }
        if (!copy_to_ram_ok) {
            break;
        }
    }

    if (!copy_to_ram_ok) {
        return false;
    }
    // Second, erase the current sector.
    erase_sector(current_sector);
    // Lastly, write all the data in ram that we've cached.
    for (uint8_t i = 0; i < SPI_FLASH_ERASE_SIZE / FILESYSTEM_BLOCK_SIZE; i++) {
        for (uint8_t j = 0; j < pages_per_block; j++) {
            write_flash(current_sector + (i * pages_per_block + j) * SPI_FLASH_PAGE_SIZE,
                MP_STATE_VM(flash_ram_cache)[i * pages_per_block + j],
                SPI_FLASH_PAGE_SIZE);
            if (!keep_cache && supervisor_cache == NULL && MP_STATE_MEM(gc_pool_start)) {
                m_free(MP_STATE_VM(flash_ram_cache)[i * pages_per_block + j]);
            }
        }
    }
    // We're done with the cache for now so give it back.
    if (!keep_cache) {
        release_ram_cache();
    }
    return true;
}

// Delegates to the correct flash flush method depending on the existing cache.
// TODO Don't blink the status indicator if we don't actually do any writing (hard to tell right now).
static void spi_flash_flush_keep_cache(bool keep_cache) {
    #ifdef MICROPY_HW_LED_MSC
    port_pin_set_output_level(MICROPY_HW_LED_MSC, true);
    #endif
    // If we've cached to the flash itself flush from there.
    if (MP_STATE_VM(flash_ram_cache) == NULL) {
        flush_scratch_flash();
    } else {
        flush_ram_cache(keep_cache);
    }
    current_sector = NO_SECTOR_LOADED;
    #ifdef MICROPY_HW_LED_MSC
    port_pin_set_output_level(MICROPY_HW_LED_MSC, false);
    #endif
}

void supervisor_external_flash_flush(void) {
    spi_flash_flush_keep_cache(true);
}

void supervisor_flash_release_cache(void) {
    spi_flash_flush_keep_cache(false);
}

static int32_t convert_block_to_flash_addr(uint32_t block) {
    if (0 <= block && block < supervisor_flash_get_block_count()) {
        // a block in partition 1
        return block * FILESYSTEM_BLOCK_SIZE;
    }
    // bad block
    return -1;
}

bool external_flash_read_block(uint8_t *dest, uint32_t block) {
    int32_t address = convert_block_to_flash_addr(block);
    if (address == -1) {
        // bad block number
        return false;
    }

    // Mask out the lower bits that designate the address within the sector.
    uint32_t this_sector = address & (~(SPI_FLASH_ERASE_SIZE - 1));
    uint8_t block_index = (address / FILESYSTEM_BLOCK_SIZE) % (SPI_FLASH_ERASE_SIZE / FILESYSTEM_BLOCK_SIZE);
    uint8_t mask = 1 << (block_index);
    // We're reading from the currently cached sector.
    if (current_sector == this_sector && (mask & dirty_mask) > 0) {
        if (MP_STATE_VM(flash_ram_cache) != NULL) {
            uint8_t pages_per_block = FILESYSTEM_BLOCK_SIZE / SPI_FLASH_PAGE_SIZE;
            for (int i = 0; i < pages_per_block; i++) {
                memcpy(dest + i * SPI_FLASH_PAGE_SIZE,
                    MP_STATE_VM(flash_ram_cache)[block_index * pages_per_block + i],
                    SPI_FLASH_PAGE_SIZE);
            }
            return true;
        } else {
            uint32_t scratch_address = flash_device->total_size - SPI_FLASH_ERASE_SIZE + block_index * FILESYSTEM_BLOCK_SIZE;
            return read_flash(scratch_address, dest, FILESYSTEM_BLOCK_SIZE);
        }
    }
    return read_flash(address, dest, FILESYSTEM_BLOCK_SIZE);
}

bool external_flash_write_block(const uint8_t *data, uint32_t block) {
    // Non-MBR block, copy to cache
    int32_t address = convert_block_to_flash_addr(block);
    if (address == -1) {
        // bad block number
        return false;
    }
    // Wait for any previous writes to finish.
    wait_for_flash_ready();
    // Mask out the lower bits that designate the address within the sector.
    uint32_t this_sector = address & (~(SPI_FLASH_ERASE_SIZE - 1));
    uint8_t block_index = (address / FILESYSTEM_BLOCK_SIZE) % (SPI_FLASH_ERASE_SIZE / FILESYSTEM_BLOCK_SIZE);
    uint8_t mask = 1 << (block_index);
    // Flush the cache if we're moving onto a sector or we're writing the
    // same block again.
    if (current_sector != this_sector || (mask & dirty_mask) > 0) {
        // Check to see if we'd write to an erased page. In that case we
        // can write directly.
        if (page_erased(address)) {
            return write_flash(address, data, FILESYSTEM_BLOCK_SIZE);
        }
        if (current_sector != NO_SECTOR_LOADED) {
            supervisor_flash_flush();
        }
        if (MP_STATE_VM(flash_ram_cache) == NULL && !allocate_ram_cache()) {
            erase_sector(flash_device->total_size - SPI_FLASH_ERASE_SIZE);
            wait_for_flash_ready();
        }
        current_sector = this_sector;
        dirty_mask = 0;
    }
    dirty_mask |= mask;
    // Copy the block to the appropriate cache.
    if (MP_STATE_VM(flash_ram_cache) != NULL) {
        uint8_t pages_per_block = FILESYSTEM_BLOCK_SIZE / SPI_FLASH_PAGE_SIZE;
        for (int i = 0; i < pages_per_block; i++) {
            memcpy(MP_STATE_VM(flash_ram_cache)[block_index * pages_per_block + i],
                data + i * SPI_FLASH_PAGE_SIZE,
                SPI_FLASH_PAGE_SIZE);
        }
        return true;
    } else {
        uint32_t scratch_address = flash_device->total_size - SPI_FLASH_ERASE_SIZE + block_index * FILESYSTEM_BLOCK_SIZE;
        return write_flash(scratch_address, data, FILESYSTEM_BLOCK_SIZE);
    }
}

mp_uint_t supervisor_flash_read_blocks(uint8_t *dest, uint32_t block_num, uint32_t num_blocks) {
    for (size_t i = 0; i < num_blocks; i++) {
        if (!external_flash_read_block(dest + i * FILESYSTEM_BLOCK_SIZE, block_num + i)) {
            return 1; // error
        }
    }
    return 0; // success
}

mp_uint_t supervisor_flash_write_blocks(const uint8_t *src, uint32_t block_num, uint32_t num_blocks) {
    for (size_t i = 0; i < num_blocks; i++) {
        if (!external_flash_write_block(src + i * FILESYSTEM_BLOCK_SIZE, block_num + i)) {
            return 1; // error
        }
    }
    return 0; // success
}
