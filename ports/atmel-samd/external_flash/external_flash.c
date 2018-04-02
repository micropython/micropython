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
#include "external_flash.h"

#include <stdint.h>
#include <string.h>

#include "external_flash/spi_flash_api.h"
#include "external_flash/common_commands.h"
#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"
#include "py/misc.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "lib/oofatfs/ff.h"
#include "peripherals.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "supervisor/shared/rgb_led_status.h"

//#include "shared_dma.h"

#include "hal_gpio.h"
#include "hal_spi_m_sync.h"

#define SPI_FLASH_PART1_START_BLOCK (0x1)

#define NO_SECTOR_LOADED 0xFFFFFFFF

struct spi_m_sync_descriptor spi_flash_desc;

// The currently cached sector in the cache, ram or flash based.
static uint32_t current_sector;

const external_flash_device possible_devices[EXTERNAL_FLASH_DEVICE_COUNT] = {EXTERNAL_FLASH_DEVICES};

static const external_flash_device* flash_device = NULL;

// Track which blocks (up to 32) in the current sector currently live in the
// cache.
static uint32_t dirty_mask;

// Wait until both the write enable and write in progress bits have cleared.
static bool wait_for_flash_ready(void) {
    uint8_t read_status_response[1] = {0x00};
    bool ok = true;
    // Both the write enable and write in progress bits should be low.
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
static bool read_flash(uint32_t address, uint8_t* data, uint32_t data_length) {
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
static bool write_flash(uint32_t address, const uint8_t* data, uint32_t data_length) {
    if (flash_device == NULL) {
        return false;
    }
    // Don't bother writing if the data is all 1s. Thats equivalent to the flash
    // state after an erase.
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

    for (uint32_t bytes_written = 0;
        bytes_written < data_length;
        bytes_written += SPI_FLASH_PAGE_SIZE) {
        if (!wait_for_flash_ready() || !write_enable()) {
            return false;
        }

        if (!spi_flash_write_data(address + bytes_written, (uint8_t*) data + bytes_written,
                                  SPI_FLASH_PAGE_SIZE)) {
            return false;
        }
    }
    return true;
}

static bool page_erased(uint32_t sector_address) {
    // Check the first few bytes to catch the common case where there is data
    // without using a bunch of memory.
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
    if (!wait_for_flash_ready() || !write_enable()) {
        return false;
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

void external_flash_init(void) {
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

    // The response will be 0xff if the flash needs more time to start up.
    uint8_t jedec_id_response[3] = {0xff, 0xff, 0xff};
    while (jedec_id_response[0] == 0xff) {
        spi_flash_read_command(CMD_READ_JEDEC_ID, jedec_id_response, 3);
    }

    for (uint8_t i = 0; i < EXTERNAL_FLASH_DEVICE_COUNT; i++) {
        const external_flash_device* possible_device = &possible_devices[i];
        if (jedec_id_response[0] == possible_device->manufacturer_id &&
            jedec_id_response[1] == possible_device->memory_type &&
            jedec_id_response[2] == possible_device->capacity) {
            flash_device = possible_device;
            break;
        }
    }

    if (flash_device == NULL) {
        return;
    }

    // We don't know what state the flash is in so wait for any remaining writes and then reset.
    uint8_t read_status_response[1] = {0x00};
    // The write in progress bit should be low.
    do {
        spi_flash_read_command(CMD_READ_STATUS, read_status_response, 1);
    } while ((read_status_response[0] & 0x1) != 0);
    // The suspended write/erase bit should be low.
    do {
        spi_flash_read_command(CMD_READ_STATUS2, read_status_response, 1);
    } while ((read_status_response[0] & 0x80) != 0);


    spi_flash_command(CMD_ENABLE_RESET);
    spi_flash_command(CMD_RESET);

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

    if (flash_device->has_sector_protection)  {
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
uint32_t external_flash_get_block_size(void) {
    return FILESYSTEM_BLOCK_SIZE;
}

// The total number of available blocks.
uint32_t external_flash_get_block_count(void) {
    // We subtract one erase sector size because we may use it as a staging area
    // for writes.
    return SPI_FLASH_PART1_START_BLOCK + (flash_device->total_size - SPI_FLASH_ERASE_SIZE) / FILESYSTEM_BLOCK_SIZE;
}

// Flush the cache that was written to the scratch portion of flash. Only used
// when ram is tight.
static bool flush_scratch_flash(void) {
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

// Flush the cached sector from ram onto the flash. We'll free the cache unless
// keep_cache is true.
static bool flush_ram_cache(bool keep_cache) {
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
            if (!keep_cache) {
                m_free(MP_STATE_VM(flash_ram_cache)[i * pages_per_block + j]);
            }
        }
    }
    // We're done with the cache for now so give it back.
    if (!keep_cache) {
        m_free(MP_STATE_VM(flash_ram_cache));
        MP_STATE_VM(flash_ram_cache) = NULL;
    }
    return true;
}

// Delegates to the correct flash flush method depending on the existing cache.
static void spi_flash_flush_keep_cache(bool keep_cache) {
    if (current_sector == NO_SECTOR_LOADED) {
        return;
    }
    #ifdef MICROPY_HW_LED_MSC
        port_pin_set_output_level(MICROPY_HW_LED_MSC, true);
    #endif
    temp_status_color(ACTIVE_WRITE);
    // If we've cached to the flash itself flush from there.
    if (MP_STATE_VM(flash_ram_cache) == NULL) {
        flush_scratch_flash();
    } else {
        flush_ram_cache(keep_cache);
    }
    current_sector = NO_SECTOR_LOADED;
    clear_temp_status();
    #ifdef MICROPY_HW_LED_MSC
        port_pin_set_output_level(MICROPY_HW_LED_MSC, false);
    #endif
}

// External flash function used. If called externally we assume we won't need
// the cache after.
void external_flash_flush(void) {
    spi_flash_flush_keep_cache(false);
}

void flash_flush(void) {
    external_flash_flush();
}

// Builds a partition entry for the MBR.
static void build_partition(uint8_t *buf, int boot, int type,
                            uint32_t start_block, uint32_t num_blocks) {
    buf[0] = boot;

    if (num_blocks == 0) {
        buf[1] = 0;
        buf[2] = 0;
        buf[3] = 0;
    } else {
        buf[1] = 0xff;
        buf[2] = 0xff;
        buf[3] = 0xff;
    }

    buf[4] = type;

    if (num_blocks == 0) {
        buf[5] = 0;
        buf[6] = 0;
        buf[7] = 0;
    } else {
        buf[5] = 0xff;
        buf[6] = 0xff;
        buf[7] = 0xff;
    }

    buf[8] = start_block;
    buf[9] = start_block >> 8;
    buf[10] = start_block >> 16;
    buf[11] = start_block >> 24;

    buf[12] = num_blocks;
    buf[13] = num_blocks >> 8;
    buf[14] = num_blocks >> 16;
    buf[15] = num_blocks >> 24;
}

static int32_t convert_block_to_flash_addr(uint32_t block) {
    if (SPI_FLASH_PART1_START_BLOCK <= block && block < external_flash_get_block_count()) {
        // a block in partition 1
        block -= SPI_FLASH_PART1_START_BLOCK;
        return block * FILESYSTEM_BLOCK_SIZE;
    }
    // bad block
    return -1;
}

bool external_flash_read_block(uint8_t *dest, uint32_t block) {
    if (block == 0) {
        // Fake the MBR so we can decide on our own partition table
        for (int i = 0; i < 446; i++) {
            dest[i] = 0;
        }

        build_partition(dest + 446, 0, 0x01 /* FAT12 */,
                        SPI_FLASH_PART1_START_BLOCK,
                        external_flash_get_block_count() - SPI_FLASH_PART1_START_BLOCK);
        build_partition(dest + 462, 0, 0, 0, 0);
        build_partition(dest + 478, 0, 0, 0, 0);
        build_partition(dest + 494, 0, 0, 0, 0);

        dest[510] = 0x55;
        dest[511] = 0xaa;

        return true;
    } else if (block < SPI_FLASH_PART1_START_BLOCK) {
        memset(dest, 0, FILESYSTEM_BLOCK_SIZE);
        return true;
    } else {
        // Non-MBR block, get data from flash memory.
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
}

bool external_flash_write_block(const uint8_t *data, uint32_t block) {
    if (block < SPI_FLASH_PART1_START_BLOCK) {
        // Fake writing below the flash partition.
        return true;
    } else {
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
                spi_flash_flush_keep_cache(true);
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
}

mp_uint_t external_flash_read_blocks(uint8_t *dest, uint32_t block_num, uint32_t num_blocks) {
    for (size_t i = 0; i < num_blocks; i++) {
        if (!external_flash_read_block(dest + i * FILESYSTEM_BLOCK_SIZE, block_num + i)) {
            return 1; // error
        }
    }
    return 0; // success
}

mp_uint_t external_flash_write_blocks(const uint8_t *src, uint32_t block_num, uint32_t num_blocks) {
    for (size_t i = 0; i < num_blocks; i++) {
        if (!external_flash_write_block(src + i * FILESYSTEM_BLOCK_SIZE, block_num + i)) {
            return 1; // error
        }
    }
    return 0; // success
}

/******************************************************************************/
// MicroPython bindings
//
// Expose the flash as an object with the block protocol.

// there is a singleton Flash object
STATIC const mp_obj_base_t external_flash_obj = {&external_flash_type};

STATIC mp_obj_t external_flash_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    // return singleton object
    return (mp_obj_t)&external_flash_obj;
}

STATIC mp_obj_t external_flash_obj_readblocks(mp_obj_t self, mp_obj_t block_num, mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_WRITE);
    mp_uint_t ret = external_flash_read_blocks(bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / FILESYSTEM_BLOCK_SIZE);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(external_flash_obj_readblocks_obj, external_flash_obj_readblocks);

STATIC mp_obj_t external_flash_obj_writeblocks(mp_obj_t self, mp_obj_t block_num, mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_READ);
    mp_uint_t ret = external_flash_write_blocks(bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / FILESYSTEM_BLOCK_SIZE);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(external_flash_obj_writeblocks_obj, external_flash_obj_writeblocks);

STATIC mp_obj_t external_flash_obj_ioctl(mp_obj_t self, mp_obj_t cmd_in, mp_obj_t arg_in) {
    mp_int_t cmd = mp_obj_get_int(cmd_in);
    switch (cmd) {
        case BP_IOCTL_INIT: external_flash_init(); return MP_OBJ_NEW_SMALL_INT(0);
        case BP_IOCTL_DEINIT: external_flash_flush(); return MP_OBJ_NEW_SMALL_INT(0); // TODO properly
        case BP_IOCTL_SYNC: external_flash_flush(); return MP_OBJ_NEW_SMALL_INT(0);
        case BP_IOCTL_SEC_COUNT: return MP_OBJ_NEW_SMALL_INT(external_flash_get_block_count());
        case BP_IOCTL_SEC_SIZE: return MP_OBJ_NEW_SMALL_INT(external_flash_get_block_size());
        default: return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(external_flash_obj_ioctl_obj, external_flash_obj_ioctl);

STATIC const mp_rom_map_elem_t external_flash_obj_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&external_flash_obj_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&external_flash_obj_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&external_flash_obj_ioctl_obj) },
};

STATIC MP_DEFINE_CONST_DICT(external_flash_obj_locals_dict, external_flash_obj_locals_dict_table);

const mp_obj_type_t external_flash_type = {
    { &mp_type_type },
    .name = MP_QSTR_SPIFlash,
    .make_new = external_flash_obj_make_new,
    .locals_dict = (mp_obj_t)&external_flash_obj_locals_dict,
};

void flash_init_vfs(fs_user_mount_t *vfs) {
    vfs->base.type = &mp_fat_vfs_type;
    vfs->flags |= FSUSER_NATIVE | FSUSER_HAVE_IOCTL;
    vfs->fatfs.drv = vfs;
    vfs->fatfs.part = 1; // flash filesystem lives on first partition
    vfs->readblocks[0] = (mp_obj_t)&external_flash_obj_readblocks_obj;
    vfs->readblocks[1] = (mp_obj_t)&external_flash_obj;
    vfs->readblocks[2] = (mp_obj_t)external_flash_read_blocks; // native version
    vfs->writeblocks[0] = (mp_obj_t)&external_flash_obj_writeblocks_obj;
    vfs->writeblocks[1] = (mp_obj_t)&external_flash_obj;
    vfs->writeblocks[2] = (mp_obj_t)external_flash_write_blocks; // native version
    vfs->u.ioctl[0] = (mp_obj_t)&external_flash_obj_ioctl_obj;
    vfs->u.ioctl[1] = (mp_obj_t)&external_flash_obj;
}
