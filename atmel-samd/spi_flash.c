/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

#include "asf/sam0/drivers/sercom/spi/spi.h"

#include "py/obj.h"
#include "py/runtime.h"
#include "lib/fatfs/ff.h"
#include "extmod/fsusermount.h"

#include "asf/sam0/drivers/nvm/nvm.h"

#include "spi_flash.h"

#define SPI_FLASH_PART1_START_BLOCK (0x100)

#define NO_SECTOR_LOADED 0xFFFFFFFF

#define CMD_READ_JEDEC_ID 0x9f
#define CMD_READ_DATA 0x03
#define CMD_SECTOR_ERASE 0x20
// #define CMD_SECTOR_ERASE CMD_READ_JEDEC_ID
#define CMD_ENABLE_WRITE 0x06
#define CMD_PAGE_PROGRAM 0x02
// #define CMD_PAGE_PROGRAM CMD_READ_JEDEC_ID
#define CMD_READ_STATUS 0x05

static bool spi_flash_is_initialised = false;

struct spi_module spi_flash_instance;

// The total size of the flash.
static uint32_t flash_size;

// The erase sector size.
static uint32_t sector_size;

// The page size. Its the maximum number of bytes that can be written at once.
static uint32_t page_size;

// The currently cached sector in the scratch flash space.
static uint32_t current_sector;

// A sector is made up of 8 blocks. This tracks which of those blocks in the
// current sector current live in the scratch sector.
static uint8_t dirty_mask;

#define SCRATCH_SECTOR (flash_size - sector_size)

static void flash_enable() {
    port_pin_set_output_level(SPI_FLASH_CS, false);
}

static void flash_disable() {
    port_pin_set_output_level(SPI_FLASH_CS, true);
}

static bool wait_for_flash_ready() {
    uint8_t status_request[2] = {CMD_READ_STATUS, 0x00};
    uint8_t response[2] = {0x00, 0x01};
    enum status_code status = STATUS_OK;
    while (status == STATUS_OK && (response[1] & 0x1) == 1) {
        flash_enable();
        status = spi_transceive_buffer_wait(&spi_flash_instance, status_request, response, 2);
        flash_disable();
    }
    return status == STATUS_OK;
}

static bool write_enable() {
    flash_enable();
    uint8_t command = CMD_ENABLE_WRITE;
    enum status_code status = spi_write_buffer_wait(&spi_flash_instance, &command, 1);
    flash_disable();
    return status == STATUS_OK;
}

static void address_to_bytes(uint32_t address, uint8_t* bytes) {
    bytes[0] = (address >> 16) & 0xff;
    bytes[1] = (address >> 8) & 0xff;
    bytes[2] = address & 0xff;
}

static bool read_flash(uint32_t address, uint8_t* data, uint32_t data_length) {
    wait_for_flash_ready();
    enum status_code status;
    // We can read as much as we want sequentially.
    uint8_t read_request[4] = {CMD_READ_DATA, 0x00, 0x00, 0x00};
    address_to_bytes(address, read_request + 1);
    flash_enable();
    status = spi_write_buffer_wait(&spi_flash_instance, read_request, 4);
    if (status == STATUS_OK) {
        status = spi_read_buffer_wait(&spi_flash_instance, data, data_length, 0x00);
    }
    flash_disable();
    return status == STATUS_OK;
}

// Assumes that the sector that address resides in has already been erased.
static bool write_flash(uint32_t address, const uint8_t* data, uint32_t data_length) {
    if (page_size == 0) {
        return false;
    }
    for (uint32_t bytes_written = 0;
        bytes_written < data_length;
        bytes_written += page_size) {
        if (!wait_for_flash_ready() || !write_enable()) {
            return false;
        }
        flash_enable();
        uint8_t command[4] = {CMD_PAGE_PROGRAM, 0x00, 0x00, 0x00};
        address_to_bytes(address + bytes_written, command + 1);
        enum status_code status;
        status = spi_write_buffer_wait(&spi_flash_instance, command, 4);
        if (status == STATUS_OK) {
            status = spi_write_buffer_wait(&spi_flash_instance, data + bytes_written, page_size);
        }
        flash_disable();
        if (status != STATUS_OK) {
            return false;
        }
    }
    return true;
}

// Sector is really 24 bits.
static bool erase_sector(uint32_t sector_address) {
    // Before we erase the sector we need to wait for any writes to finish and
    // and then enable the write again. For good measure we check that the flash
    // is ready after enabling the write too.
    if (!wait_for_flash_ready() || !write_enable() || !wait_for_flash_ready()) {
        return false;
    }

    uint8_t erase_request[4] = {CMD_SECTOR_ERASE, 0x00, 0x00, 0x00};
    address_to_bytes(sector_address, erase_request + 1);

    flash_enable();
    enum status_code status = spi_write_buffer_wait(&spi_flash_instance, erase_request, 4);
    flash_disable();
    return status == STATUS_OK;
}

// Sector is really 24 bits.
static bool copy_block(uint32_t src_address, uint32_t dest_address) {
    // Copy page by page to minimize RAM buffer.
    uint8_t buffer[page_size];
    for (int i = 0; i < FLASH_BLOCK_SIZE / page_size; i++) {
        if (!read_flash(src_address + i * page_size, buffer, page_size)) {
            return false;
        }
        if (!write_flash(dest_address + i * page_size, buffer, page_size)) {
            return false;
        }
    }
    return true;
}

void spi_flash_init(void) {
    if (!spi_flash_is_initialised) {

        struct spi_config config_spi_master;
        spi_get_config_defaults(&config_spi_master);
        config_spi_master.mux_setting = SPI_FLASH_MUX_SETTING;
        config_spi_master.pinmux_pad0 = SPI_FLASH_PAD0_PINMUX;
        config_spi_master.pinmux_pad1 = SPI_FLASH_PAD1_PINMUX;
        config_spi_master.pinmux_pad2 = SPI_FLASH_PAD2_PINMUX;
        config_spi_master.pinmux_pad3 = SPI_FLASH_PAD3_PINMUX;
        config_spi_master.mode_specific.master.baudrate = SPI_FLASH_BAUDRATE;
        spi_init(&spi_flash_instance, SPI_FLASH_SERCOM, &config_spi_master);
        spi_enable(&spi_flash_instance);

        // Manage chip select ourselves.
        struct port_config pin_conf;
        port_get_config_defaults(&pin_conf);

        pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
        port_pin_set_config(SPI_FLASH_CS, &pin_conf);
        flash_disable();

        uint8_t jedec_id_request[4] = {CMD_READ_JEDEC_ID, 0x00, 0x00, 0x00};
        uint8_t response[4] = {0x00, 0x00, 0x00, 0x00};
        flash_enable();
        volatile enum status_code status = spi_transceive_buffer_wait(&spi_flash_instance, jedec_id_request, response, 4);
        flash_disable();
        (void) status;
        if (response[1] == 0x01 && response[2] == 0x40 && response[3] == 0x15) {
            flash_size = 1 << 21; // 2 MiB
            sector_size = 1 << 12; // 4 KiB
            page_size = 256; // 256 bytes
        } else {
            // Unknown flash chip!
            flash_size = 0;
        }

        current_sector = NO_SECTOR_LOADED;
        dirty_mask = 0;

        spi_flash_is_initialised = true;
    }
}

uint32_t spi_flash_get_block_size(void) {
    return FLASH_BLOCK_SIZE;
}

uint32_t spi_flash_get_block_count(void) {
    // We subtract on erase sector size because we're going to use it as a
    // staging area for writes.
    return SPI_FLASH_PART1_START_BLOCK + (flash_size - sector_size) / FLASH_BLOCK_SIZE;
}

void spi_flash_flush(void) {
    if (current_sector == NO_SECTOR_LOADED) {
        return;
    }
    // First, copy out any blocks that we haven't touched from the sector we've
    // cached.
    bool copy_to_scratch_ok = true;
    for (int i = 0; i < sector_size / FLASH_BLOCK_SIZE; i++) {
        if ((dirty_mask & (1 << i)) == 0) {
            copy_to_scratch_ok = copy_to_scratch_ok &&
                copy_block(current_sector + i * FLASH_BLOCK_SIZE,
                           SCRATCH_SECTOR + i * FLASH_BLOCK_SIZE);
        }
    }
    if (!copy_to_scratch_ok) {
        // TODO(tannewt): Do more here. We opted to not erase and copy bad data
        // in. We still risk losing the data written to the scratch sector.
        return;
    }
    // Second, erase the current sector.
    erase_sector(current_sector);
    // Finally, copy the new version into it.
    for (int i = 0; i < sector_size / FLASH_BLOCK_SIZE; i++) {
        copy_block(SCRATCH_SECTOR + i * FLASH_BLOCK_SIZE,
                   current_sector + i * FLASH_BLOCK_SIZE);
    }
    current_sector = NO_SECTOR_LOADED;
}

static void build_partition(uint8_t *buf, int boot, int type, uint32_t start_block, uint32_t num_blocks) {
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

static uint32_t convert_block_to_flash_addr(uint32_t block) {
    if (SPI_FLASH_PART1_START_BLOCK <= block && block < spi_flash_get_block_count()) {
        // a block in partition 1
        block -= SPI_FLASH_PART1_START_BLOCK;
        return block * FLASH_BLOCK_SIZE;
    }
    // bad block
    return -1;
}

bool spi_flash_read_block(uint8_t *dest, uint32_t block) {
    //printf("RD %u\n", block);
    if (block == 0) {
        // fake the MBR so we can decide on our own partition table

        for (int i = 0; i < 446; i++) {
            dest[i] = 0;
        }

        build_partition(dest + 446, 0, 0x01 /* FAT12 */, SPI_FLASH_PART1_START_BLOCK, spi_flash_get_block_count() - SPI_FLASH_PART1_START_BLOCK);
        build_partition(dest + 462, 0, 0, 0, 0);
        build_partition(dest + 478, 0, 0, 0, 0);
        build_partition(dest + 494, 0, 0, 0, 0);

        dest[510] = 0x55;
        dest[511] = 0xaa;

        return true;

    } else {
        // non-MBR block, get data from flash memory
        uint32_t src = convert_block_to_flash_addr(block);
        if (src == -1) {
            // bad block number
            return false;
        }
        return read_flash(src, dest, FLASH_BLOCK_SIZE);
    }
}

bool spi_flash_write_block(const uint8_t *data, uint32_t block) {
    if (block == 0) {
        // can't write MBR, but pretend we did
        return true;

    } else {
        // non-MBR block, copy to cache
        volatile uint32_t address = convert_block_to_flash_addr(block);
        if (address == -1) {
            // bad block number
            return false;
        }
        // Wait for any previous writes to finish.
        wait_for_flash_ready();
        uint32_t this_sector = address & (~(sector_size - 1));
        uint8_t block_index = block % (sector_size / FLASH_BLOCK_SIZE);
        uint8_t mask = 1 << (block_index);
        if (current_sector != this_sector || (mask & dirty_mask) > 0) {
            if (current_sector != NO_SECTOR_LOADED) {
                spi_flash_flush();
            }
            erase_sector(SCRATCH_SECTOR);
            current_sector = this_sector;
            dirty_mask = 0;
            wait_for_flash_ready();
        }
        uint32_t scratch_address = SCRATCH_SECTOR + block_index * FLASH_BLOCK_SIZE;
        dirty_mask |= mask;
        return write_flash(scratch_address, data, FLASH_BLOCK_SIZE);
    }
}

mp_uint_t spi_flash_read_blocks(uint8_t *dest, uint32_t block_num, uint32_t num_blocks) {
    for (size_t i = 0; i < num_blocks; i++) {
        if (!spi_flash_read_block(dest + i * FLASH_BLOCK_SIZE, block_num + i)) {
            return 1; // error
        }
    }
    return 0; // success
}

mp_uint_t spi_flash_write_blocks(const uint8_t *src, uint32_t block_num, uint32_t num_blocks) {
    for (size_t i = 0; i < num_blocks; i++) {
        if (!spi_flash_write_block(src + i * FLASH_BLOCK_SIZE, block_num + i)) {
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
STATIC const mp_obj_base_t spi_flash_obj = {&spi_flash_type};

STATIC mp_obj_t spi_flash_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    // return singleton object
    return (mp_obj_t)&spi_flash_obj;
}

STATIC mp_obj_t spi_flash_obj_readblocks(mp_obj_t self, mp_obj_t block_num, mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_WRITE);
    mp_uint_t ret = spi_flash_read_blocks(bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / FLASH_BLOCK_SIZE);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(spi_flash_obj_readblocks_obj, spi_flash_obj_readblocks);

STATIC mp_obj_t spi_flash_obj_writeblocks(mp_obj_t self, mp_obj_t block_num, mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_READ);
    mp_uint_t ret = spi_flash_write_blocks(bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / FLASH_BLOCK_SIZE);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(spi_flash_obj_writeblocks_obj, spi_flash_obj_writeblocks);

STATIC mp_obj_t spi_flash_obj_ioctl(mp_obj_t self, mp_obj_t cmd_in, mp_obj_t arg_in) {
    mp_int_t cmd = mp_obj_get_int(cmd_in);
    switch (cmd) {
        case BP_IOCTL_INIT: spi_flash_init(); return MP_OBJ_NEW_SMALL_INT(0);
        case BP_IOCTL_DEINIT: spi_flash_flush(); return MP_OBJ_NEW_SMALL_INT(0); // TODO properly
        case BP_IOCTL_SYNC: spi_flash_flush(); return MP_OBJ_NEW_SMALL_INT(0);
        case BP_IOCTL_SEC_COUNT: return MP_OBJ_NEW_SMALL_INT(spi_flash_get_block_count());
        case BP_IOCTL_SEC_SIZE: return MP_OBJ_NEW_SMALL_INT(spi_flash_get_block_size());
        default: return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(spi_flash_obj_ioctl_obj, spi_flash_obj_ioctl);

STATIC const mp_map_elem_t spi_flash_obj_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_readblocks), (mp_obj_t)&spi_flash_obj_readblocks_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_writeblocks), (mp_obj_t)&spi_flash_obj_writeblocks_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ioctl), (mp_obj_t)&spi_flash_obj_ioctl_obj },
};

STATIC MP_DEFINE_CONST_DICT(spi_flash_obj_locals_dict, spi_flash_obj_locals_dict_table);

const mp_obj_type_t spi_flash_type = {
    { &mp_type_type },
    .name = MP_QSTR_SPIFlash,
    .make_new = spi_flash_obj_make_new,
    .locals_dict = (mp_obj_t)&spi_flash_obj_locals_dict,
};

void flash_init_vfs(fs_user_mount_t *vfs) {
    vfs->flags |= FSUSER_NATIVE | FSUSER_HAVE_IOCTL;
    vfs->readblocks[0] = (mp_obj_t)&spi_flash_obj_readblocks_obj;
    vfs->readblocks[1] = (mp_obj_t)&spi_flash_obj;
    vfs->readblocks[2] = (mp_obj_t)spi_flash_read_blocks; // native version
    vfs->writeblocks[0] = (mp_obj_t)&spi_flash_obj_writeblocks_obj;
    vfs->writeblocks[1] = (mp_obj_t)&spi_flash_obj;
    vfs->writeblocks[2] = (mp_obj_t)spi_flash_write_blocks; // native version
    vfs->u.ioctl[0] = (mp_obj_t)&spi_flash_obj_ioctl_obj;
    vfs->u.ioctl[1] = (mp_obj_t)&spi_flash_obj;
}
