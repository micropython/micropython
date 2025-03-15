/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020 Peter Hinch
 * Copyright (c) 2023 Robert Hammelrath
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
 *
 */

#include <stdint.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "extmod/modmachine.h"
#include "extmod/vfs.h"
#include "pin_af.h"

#if MICROPY_HW_SPIFLASH

#define _READ_INDEX (0)
#define _PROGRAM_PAGE_INDEX (1)
#define _SECTOR_ERASE_INDEX (2)

const uint8_t _COMMANDS_24BIT[] = {0x03, 0x02, 0x20};  // READ, PROGRAM_PAGE, ERASE_4K
const uint8_t _COMMANDS_32BIT[] = {0x13, 0x12, 0x21};  // READ, PROGRAM_PAGE, ERASE_4K

#define COMMAND_JEDEC_ID (0x9F)
#define COMMAND_READ_STATUS (0x05)
#define COMMAND_WRITE_ENABLE (0x06)
#define COMMAND_READ_SFDP (0x5A)
#define PAGE_SIZE (256)
#define SECTOR_SIZE (4096)
#ifndef MICROPY_HW_SPIFLASH_BAUDRATE
#define MICROPY_HW_SPIFLASH_BAUDRATE (24000000)
#endif

typedef struct _spiflash_obj_t {
    mp_obj_base_t base;
    mp_obj_base_t *spi;
    mp_hal_pin_obj_t cs;
    bool addr_is_32bit;
    uint16_t pagesize;
    uint16_t sectorsize;
    const uint8_t *commands;
    uint32_t size;
} spiflash_obj_t;

extern const mp_obj_type_t samd_spiflash_type;

// The SPIflash object is a singleton
static spiflash_obj_t spiflash_obj = {
    { &samd_spiflash_type }, NULL, 0, false, PAGE_SIZE, SECTOR_SIZE, NULL, 0
};

static void spi_transfer(mp_obj_base_t *spi, size_t len, const uint8_t *src, uint8_t *dest) {
    mp_machine_spi_p_t *spi_p = (mp_machine_spi_p_t *)MP_OBJ_TYPE_GET_SLOT(spi->type, protocol);
    spi_p->transfer(spi, len, src, dest);
}

static void wait(spiflash_obj_t *self) {
    uint8_t msg[2];
    uint32_t timeout = 100000;

    // each loop takes at least about 5us @ 120Mhz. So a timeout of
    // 100000 wait 500ms max. at 120Mhz. Sector erase lasts about
    // 100ms worst case, page write is < 1ms.
    do {
        msg[0] = COMMAND_READ_STATUS;
        mp_hal_pin_write(self->cs, 0);
        spi_transfer((mp_obj_base_t *)self->spi, 2, msg, msg);
        mp_hal_pin_write(self->cs, 1);
    } while (msg[1] != 0 && timeout-- > 0);
}

static void get_id(spiflash_obj_t *self, uint8_t id[3]) {
    uint8_t msg[1];

    msg[0] = COMMAND_JEDEC_ID;
    mp_hal_pin_write(self->cs, 0);
    spi_transfer(self->spi, 1, msg, NULL);
    spi_transfer(self->spi, 3, id, id);
    mp_hal_pin_write(self->cs, 1);
}
static void write_addr(spiflash_obj_t *self, uint8_t cmd, uint32_t addr) {
    uint8_t msg[5];
    uint8_t index = 1;
    msg[0] = cmd;
    if (self->addr_is_32bit) {
        msg[index++] = addr >> 24;
    }
    msg[index++] = (addr >> 16) & 0xff;
    msg[index++] = (addr >> 8) & 0xff;
    msg[index++] = addr & 0xff;
    mp_hal_pin_write(self->cs, 0);
    spi_transfer(self->spi, self->addr_is_32bit ? 5 : 4, msg, msg);
}

static void write_enable(spiflash_obj_t *self) {
    uint8_t msg[1];

    msg[0] = COMMAND_WRITE_ENABLE;
    mp_hal_pin_write(self->cs, 0);
    spi_transfer(self->spi, 1, msg, NULL);
    mp_hal_pin_write(self->cs, 1);
}

static void get_sfdp(spiflash_obj_t *self, uint32_t addr, uint8_t *buffer, int size) {
    uint8_t dummy[1];
    write_addr(self, COMMAND_READ_SFDP, addr);
    spi_transfer(self->spi, 1, dummy, NULL);
    spi_transfer(self->spi, size, buffer, buffer);
    mp_hal_pin_write(self->cs, 1);
}

static mp_obj_t spiflash_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    // Set up the object
    spiflash_obj_t *self = &spiflash_obj;

    mp_obj_t spi_args[] = {
        MP_OBJ_NEW_SMALL_INT(MICROPY_HW_SPIFLASH_ID),
        MP_OBJ_NEW_SMALL_INT(MICROPY_HW_SPIFLASH_BAUDRATE),
        MP_OBJ_NEW_QSTR(MP_QSTR_mosi), MP_OBJ_NEW_QSTR(MP_QSTR_FLASH_MOSI),
        MP_OBJ_NEW_QSTR(MP_QSTR_miso), MP_OBJ_NEW_QSTR(MP_QSTR_FLASH_MISO),
        MP_OBJ_NEW_QSTR(MP_QSTR_sck), MP_OBJ_NEW_QSTR(MP_QSTR_FLASH_SCK),
    };
    self->spi = MP_OBJ_TYPE_GET_SLOT(&machine_spi_type, make_new)((mp_obj_t)&machine_spi_type, 2, 3, spi_args);

    mp_obj_t pin_args[] = {
        MP_OBJ_NEW_QSTR(MP_QSTR_FLASH_CS),
        MP_ROM_INT(1),
    };
    machine_pin_obj_t *cs = MP_OBJ_TYPE_GET_SLOT(&machine_pin_type, make_new)((mp_obj_t)&machine_pin_type, 2, 0, pin_args);
    self->cs = cs->pin_id;
    mp_hal_pin_write(self->cs, 1);

    wait(self);

    // Get the flash size from the device ID (default)
    uint8_t id[3];
    get_id(self, id);
    if (id[1] == 0x84 && id[2] == 1) {  // Adesto
        self->size = 512 * 1024;
    } else if (id[1] == 0x1f && id[2] == 1) {  // Atmel / Renesas
        self->size = 1024 * 1024;
    } else {
        self->size = 1 << id[2];
    }

    // Get the addr_is_32bit flag and the sector size
    uint8_t buffer[128];
    get_sfdp(self, 0, buffer, 16);  // get the header
    int len = MIN(buffer[11] * 4, sizeof(buffer));
    if (len >= 29) {
        int addr = buffer[12] + (buffer[13] << 8) + (buffer[14] << 16);
        get_sfdp(self, addr, buffer, len);  // Get the JEDEC mandatory table
        self->sectorsize = 1 << buffer[28];
        self->addr_is_32bit = ((buffer[2] >> 1) & 0x03) != 0;
    }
    self->commands = self->addr_is_32bit ? _COMMANDS_32BIT : _COMMANDS_24BIT;

    return self;
}

static mp_obj_t spiflash_read(spiflash_obj_t *self, uint32_t addr, uint8_t *dest, uint32_t len) {
    if (len > 0) {
        write_addr(self, self->commands[_READ_INDEX], addr);
        spi_transfer(self->spi, len, dest, dest);
        mp_hal_pin_write(self->cs, 1);
    }

    return mp_const_none;
}

static mp_obj_t spiflash_write(spiflash_obj_t *self, uint32_t addr, uint8_t *src, uint32_t len) {
    uint32_t length = len;
    uint32_t pos = 0;
    uint8_t *buf = src;

    while (pos < length) {
        uint16_t maxsize = self->pagesize - pos % self->pagesize;
        uint16_t size = (length - pos) > maxsize ? maxsize : length - pos;

        write_enable(self);
        write_addr(self, self->commands[_PROGRAM_PAGE_INDEX], addr);
        spi_transfer(self->spi, size, buf + pos, NULL);
        mp_hal_pin_write(self->cs, 1);
        wait(self);

        addr += size;
        pos += size;
    }

    return mp_const_none;
}

static mp_obj_t spiflash_erase(spiflash_obj_t *self, uint32_t addr) {
    write_enable(self);
    write_addr(self, self->commands[_SECTOR_ERASE_INDEX], addr);
    mp_hal_pin_write(self->cs, 1);
    wait(self);

    return mp_const_none;
}

static mp_obj_t spiflash_readblocks(size_t n_args, const mp_obj_t *args) {
    spiflash_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t offset = (mp_obj_get_int(args[1]) * self->sectorsize);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_WRITE);
    if (n_args == 4) {
        offset += mp_obj_get_int(args[3]);
    }

    // Read data to flash (adf4 API)
    spiflash_read(self, offset, bufinfo.buf, bufinfo.len);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(spiflash_readblocks_obj, 3, 4, spiflash_readblocks);

static mp_obj_t spiflash_writeblocks(size_t n_args, const mp_obj_t *args) {
    spiflash_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t offset = (mp_obj_get_int(args[1]) * self->sectorsize);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_READ);
    if (n_args == 3) {
        spiflash_erase(self, offset);
        // TODO check return value
    } else {
        offset += mp_obj_get_int(args[3]);
    }
    // Write data to flash (adf4 API)
    spiflash_write(self,  offset, bufinfo.buf, bufinfo.len);
    // TODO check return value
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(spiflash_writeblocks_obj, 3, 4, spiflash_writeblocks);

static mp_obj_t spiflash_ioctl(mp_obj_t self_in, mp_obj_t cmd_in, mp_obj_t arg_in) {
    spiflash_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t cmd = mp_obj_get_int(cmd_in);

    switch (cmd) {
        case MP_BLOCKDEV_IOCTL_INIT:
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_DEINIT:
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_SYNC:
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_BLOCK_COUNT:
            return MP_OBJ_NEW_SMALL_INT(self->size / self->sectorsize);
        case MP_BLOCKDEV_IOCTL_BLOCK_SIZE:
            return MP_OBJ_NEW_SMALL_INT(self->sectorsize);
        case MP_BLOCKDEV_IOCTL_BLOCK_ERASE: {
            spiflash_erase(self, mp_obj_get_int(arg_in) * self->sectorsize);
            // TODO check return value
            return MP_OBJ_NEW_SMALL_INT(0);
        }
        default:
            return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_3(spiflash_ioctl_obj, spiflash_ioctl);

static const mp_rom_map_elem_t spiflash_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&spiflash_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&spiflash_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&spiflash_ioctl_obj) },
};
static MP_DEFINE_CONST_DICT(spiflash_locals_dict, spiflash_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    samd_spiflash_type,
    MP_QSTR_Flash,
    MP_TYPE_FLAG_NONE,
    make_new, spiflash_make_new,
    locals_dict, &spiflash_locals_dict
    );

#endif // #if MICROPY_HW_SPIFLASH
