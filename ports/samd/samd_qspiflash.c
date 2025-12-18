/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Adafruit Industries
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
 * Port of the Adafruit QSPIflash driver for SAMD devices
 *
 */

#include <stdint.h>
#include <string.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "modmachine.h"
#include "extmod/modmachine.h"
#include "extmod/vfs.h"
#include "pin_af.h"
#include "clock_config.h"
#include "sam.h"

#ifdef MICROPY_HW_QSPIFLASH

#include "drivers/memory/external_flash_device.h"

// QSPI command codes
enum
{
    QSPI_CMD_READ              = 0x03,
    QSPI_CMD_READ_4B           = 0x13,
    QSPI_CMD_QUAD_READ         = 0x6B,// 1 line address, 4 line data

    QSPI_CMD_READ_JEDEC_ID     = 0x9f,

    QSPI_CMD_PAGE_PROGRAM      = 0x02,
    QSPI_CMD_PAGE_PROGRAM_4B   = 0x12,
    QSPI_CMD_QUAD_PAGE_PROGRAM = 0x32, // 1 line address, 4 line data

    QSPI_CMD_READ_STATUS       = 0x05,
    QSPI_CMD_READ_STATUS2      = 0x35,

    QSPI_CMD_WRITE_STATUS      = 0x01,
    QSPI_CMD_WRITE_STATUS2     = 0x31,

    QSPI_CMD_ENABLE_RESET      = 0x66,
    QSPI_CMD_RESET             = 0x99,

    QSPI_CMD_WRITE_ENABLE      = 0x06,
    QSPI_CMD_WRITE_DISABLE     = 0x04,

    QSPI_CMD_ERASE_SECTOR      = 0x20,
    QSPI_CMD_ERASE_SECTOR_4B   = 0x21,
    QSPI_CMD_ERASE_BLOCK       = 0xD8,
    QSPI_CMD_ERASE_CHIP        = 0xC7,

    QSPI_CMD_READ_SFDP_PARAMETER = 0x5A,
};

// QSPI flash pins are: CS=PB11, SCK=PB10, IO0-IO3=PA08, PA09, PA10 and PA11.
#define PIN_CS  (43)
#define PIN_SCK (42)
#define PIN_IO0 (8)
#define PIN_IO1 (9)
#define PIN_IO2 (10)
#define PIN_IO3 (11)

#define PAGE_SIZE (256)
#define SECTOR_SIZE (4096)

typedef struct _samd_qspiflash_obj_t {
    mp_obj_base_t base;
    uint16_t pagesize;
    uint16_t sectorsize;
    uint32_t size;
    uint8_t phase;
    uint8_t polarity;
} samd_qspiflash_obj_t;

/// List of all possible flash devices used by Adafruit boards
static const external_flash_device possible_devices[] = {
    MICROPY_HW_QSPIFLASH
};

#define EXTERNAL_FLASH_DEVICE_COUNT MP_ARRAY_SIZE(possible_devices)
static external_flash_device const *flash_device;
extern const mp_obj_type_t samd_qspiflash_type;

// The QSPIflash object is a singleton
static samd_qspiflash_obj_t qspiflash_obj = { { &samd_qspiflash_type } };

// Turn off cache and invalidate all data in it.
static void samd_peripherals_disable_and_clear_cache(void) {
    CMCC->CTRL.bit.CEN = 0;
    while (CMCC->SR.bit.CSTS) {
    }
    CMCC->MAINT0.bit.INVALL = 1;
}

// Enable cache
static void samd_peripherals_enable_cache(void) {
    CMCC->CTRL.bit.CEN = 1;
}

// Run a single QSPI instruction.
// Parameters are:
// - command instruction code
// - iframe iframe register value (configured by caller according to command code)
// - addr the address to read or write from. If the instruction doesn't require an address, this parameter is meaningless.
// - buffer pointer to the data to be written or stored depending on the type is Read or Write
// - size the number of bytes to read or write.
bool run_instruction(uint8_t command, uint32_t iframe, uint32_t addr, uint8_t *buffer, uint32_t size) {

    samd_peripherals_disable_and_clear_cache();

    uint8_t *qspi_mem = (uint8_t *)QSPI_AHB;
    if (addr) {
        qspi_mem += addr;
    }

    QSPI->INSTRCTRL.bit.INSTR = command;
    QSPI->INSTRADDR.reg = addr;
    QSPI->INSTRFRAME.reg = iframe;

    // Dummy read of INSTRFRAME needed to synchronize.
    // See Instruction Transmission Flow Diagram, figure 37.9, page 995
    // and Example 4, page 998, section 37.6.8.5.
    (volatile uint32_t)QSPI->INSTRFRAME.reg;

    if (buffer && size) {
        uint32_t const tfr_type = iframe & QSPI_INSTRFRAME_TFRTYPE_Msk;
        if ((tfr_type == QSPI_INSTRFRAME_TFRTYPE_READ) || (tfr_type == QSPI_INSTRFRAME_TFRTYPE_READMEMORY)) {
            memcpy(buffer, qspi_mem, size);
        } else {
            memcpy(qspi_mem, buffer, size);
        }
    }

    __asm volatile ("dsb");
    __asm volatile ("isb");

    QSPI->CTRLA.reg = QSPI_CTRLA_ENABLE | QSPI_CTRLA_LASTXFER;
    while (!QSPI->INTFLAG.bit.INSTREND) {
    }
    QSPI->INTFLAG.reg = QSPI_INTFLAG_INSTREND;

    samd_peripherals_enable_cache();
    return true;
}

bool run_command(uint8_t command) {
    uint32_t iframe = QSPI_INSTRFRAME_WIDTH_SINGLE_BIT_SPI | QSPI_INSTRFRAME_ADDRLEN_24BITS |
        QSPI_INSTRFRAME_TFRTYPE_READ | QSPI_INSTRFRAME_INSTREN;
    return run_instruction(command, iframe, 0, NULL, 0);
}

bool read_command(uint8_t command, uint8_t *response, uint32_t len) {
    uint32_t iframe = QSPI_INSTRFRAME_WIDTH_SINGLE_BIT_SPI | QSPI_INSTRFRAME_ADDRLEN_24BITS |
        QSPI_INSTRFRAME_TFRTYPE_READ | QSPI_INSTRFRAME_INSTREN | QSPI_INSTRFRAME_DATAEN;
    return run_instruction(command, iframe, 0, response, len);
}

bool read_memory_single(uint8_t command, uint32_t addr, uint8_t *response, uint32_t len) {
    uint32_t iframe = QSPI_INSTRFRAME_WIDTH_SINGLE_BIT_SPI | QSPI_INSTRFRAME_ADDRLEN_24BITS |
        QSPI_INSTRFRAME_TFRTYPE_READ | QSPI_INSTRFRAME_INSTREN | QSPI_INSTRFRAME_ADDREN |
        QSPI_INSTRFRAME_DATAEN | QSPI_INSTRFRAME_DUMMYLEN(8);
    return run_instruction(command, iframe, addr, response, len);
}

bool write_command(uint8_t command, uint8_t const *data, uint32_t len) {
    uint32_t iframe = QSPI_INSTRFRAME_WIDTH_SINGLE_BIT_SPI | QSPI_INSTRFRAME_ADDRLEN_24BITS |
        QSPI_INSTRFRAME_TFRTYPE_WRITE | QSPI_INSTRFRAME_INSTREN | (data != NULL ? QSPI_INSTRFRAME_DATAEN : 0);
    return run_instruction(command, iframe, 0, (uint8_t *)data, len);
}

bool erase_command(uint8_t command, uint32_t address) {
    // Sector Erase
    uint32_t iframe = QSPI_INSTRFRAME_WIDTH_SINGLE_BIT_SPI | QSPI_INSTRFRAME_ADDRLEN_24BITS |
        QSPI_INSTRFRAME_TFRTYPE_WRITE | QSPI_INSTRFRAME_INSTREN | QSPI_INSTRFRAME_ADDREN;
    return run_instruction(command, iframe, address, NULL, 0);
}

bool read_memory_quad(uint8_t command, uint32_t addr, uint8_t *data, uint32_t len) {
    uint32_t iframe = QSPI_INSTRFRAME_WIDTH_QUAD_OUTPUT | QSPI_INSTRFRAME_ADDRLEN_24BITS |
        QSPI_INSTRFRAME_TFRTYPE_READMEMORY | QSPI_INSTRFRAME_INSTREN | QSPI_INSTRFRAME_ADDREN | QSPI_INSTRFRAME_DATAEN |
        /*QSPI_INSTRFRAME_CRMODE |*/ QSPI_INSTRFRAME_DUMMYLEN(8);
    return run_instruction(command, iframe, addr, data, len);
}

bool write_memory_quad(uint8_t command, uint32_t addr, uint8_t *data, uint32_t len) {
    uint32_t iframe = QSPI_INSTRFRAME_WIDTH_QUAD_OUTPUT | QSPI_INSTRFRAME_ADDRLEN_24BITS |
        QSPI_INSTRFRAME_TFRTYPE_WRITEMEMORY | QSPI_INSTRFRAME_INSTREN | QSPI_INSTRFRAME_ADDREN | QSPI_INSTRFRAME_DATAEN;
    return run_instruction(command, iframe, addr, data, len);
}

static uint8_t read_status(void) {
    uint8_t r;
    read_command(QSPI_CMD_READ_STATUS, &r, 1);
    return r;
}

static uint8_t read_status2(void) {
    uint8_t r;
    read_command(QSPI_CMD_READ_STATUS2, &r, 1);
    return r;
}

static bool write_enable(void) {
    return run_command(QSPI_CMD_WRITE_ENABLE);
}

static void wait_for_flash_ready(void) {
    // both WIP and WREN bit should be clear
    while (read_status() & 0x03) {
    }
}

static uint8_t get_baud(int32_t freq_mhz) {
    int baud = get_cpu_freq() / (freq_mhz * 1000000) - 1;
    if (baud < 1) {
        baud = 1;
    }
    if (baud > 255) {
        baud = 255;
    }
    return baud;
}

static mp_obj_t samd_qspiflash_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    // The QSPI is a singleton
    samd_qspiflash_obj_t *self = &qspiflash_obj;
    self->phase = 0;
    self->polarity = 0;
    self->pagesize = PAGE_SIZE;
    self->sectorsize = SECTOR_SIZE;

    // Enable the device clock
    MCLK->AHBMASK.reg |= MCLK_AHBMASK_QSPI;
    MCLK->AHBMASK.reg |= MCLK_AHBMASK_QSPI_2X;
    MCLK->APBCMASK.reg |= MCLK_APBCMASK_QSPI;

    // Configure the pins.
    mp_hal_set_pin_mux(PIN_CS, ALT_FCT_QSPI);
    mp_hal_set_pin_mux(PIN_SCK, ALT_FCT_QSPI);
    mp_hal_set_pin_mux(PIN_IO0, ALT_FCT_QSPI);
    mp_hal_set_pin_mux(PIN_IO1, ALT_FCT_QSPI);
    mp_hal_set_pin_mux(PIN_IO2, ALT_FCT_QSPI);
    mp_hal_set_pin_mux(PIN_IO3, ALT_FCT_QSPI);

    // Configure the QSPI interface
    QSPI->CTRLA.bit.SWRST = 1;
    mp_hal_delay_us(1000);  // Maybe not required.

    QSPI->CTRLB.reg = QSPI_CTRLB_MODE_MEMORY |
        QSPI_CTRLB_CSMODE_NORELOAD |
        QSPI_CTRLB_DATALEN_8BITS |
        QSPI_CTRLB_CSMODE_LASTXFER;
    // start with low 4Mhz, Mode 0
    QSPI->BAUD.reg = QSPI_BAUD_BAUD(get_baud(4)) |
        (self->phase << QSPI_BAUD_CPHA_Pos) |
        (self->polarity << QSPI_BAUD_CPOL_Pos);
    QSPI->CTRLA.bit.ENABLE = 1;

    uint8_t jedec_ids[3];
    read_command(QSPI_CMD_READ_JEDEC_ID, jedec_ids, sizeof(jedec_ids));

    // Check, if the flash device is known and get it's properties.
    flash_device = NULL;
    for (uint8_t i = 0; i < EXTERNAL_FLASH_DEVICE_COUNT; i++) {
        const external_flash_device *possible_device = &possible_devices[i];
        if (jedec_ids[0] == possible_device->manufacturer_id &&
            jedec_ids[1] == possible_device->memory_type &&
            jedec_ids[2] == possible_device->capacity) {
            flash_device = possible_device;
            break;
        }
    }
    if (flash_device == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("QSPI device not supported"));
    }

    self->size = flash_device->total_size;

    // The write in progress bit should be low.
    while (read_status() & 0x01) {
    }

    if (!flash_device->single_status_byte) {
        // The suspended write/erase bit should be low.
        while (read_status2() & 0x80) {
        }
    }

    run_command(QSPI_CMD_ENABLE_RESET);
    run_command(QSPI_CMD_RESET);
    // Wait 30us for the reset
    mp_hal_delay_us(30);
    // Speed up the frequency
    QSPI->BAUD.bit.BAUD = get_baud(flash_device->max_clock_speed_mhz);

    // Enable Quad Mode if available
    uint8_t status = 0;
    if (flash_device->quad_enable_bit_mask) {
        // Verify that QSPI mode is enabled.
        status = flash_device->single_status_byte ? read_status() : read_status2();
    }

    // Check the quad enable bit.
    if ((status & flash_device->quad_enable_bit_mask) == 0) {
        write_enable();
        uint8_t full_status[2] = {0x00, flash_device->quad_enable_bit_mask};

        if (flash_device->write_status_register_split) {
            write_command(QSPI_CMD_WRITE_STATUS2, full_status + 1, 1);
        } else if (flash_device->single_status_byte) {
            write_command(QSPI_CMD_WRITE_STATUS, full_status + 1, 1);
        } else {
            write_command(QSPI_CMD_WRITE_STATUS, full_status, 2);
        }
    }
    // Turn off writes in case this is a microcontroller only reset.
    run_command(QSPI_CMD_WRITE_DISABLE);
    wait_for_flash_ready();

    return self;
}

static mp_obj_t samd_qspiflash_read(samd_qspiflash_obj_t *self, uint32_t addr, uint8_t *dest, uint32_t len) {
    if (len > 0) {
        wait_for_flash_ready();
        // Command 0x6B 1 line address, 4 line Data
        // with Continuous Read Mode and Quad output mode, read memory type
        read_memory_quad(QSPI_CMD_QUAD_READ, addr, dest, len);
    }

    return mp_const_none;
}

static mp_obj_t samd_qspiflash_write(samd_qspiflash_obj_t *self, uint32_t addr, uint8_t *src, uint32_t len) {
    uint32_t length = len;
    uint32_t pos = 0;
    uint8_t *buf = src;

    while (pos < length) {
        uint16_t maxsize = self->pagesize - pos % self->pagesize;
        uint16_t size = (length - pos) > maxsize ? maxsize : length - pos;

        wait_for_flash_ready();
        write_enable();
        write_memory_quad(QSPI_CMD_QUAD_PAGE_PROGRAM, addr, buf + pos, size);

        addr += size;
        pos += size;
    }

    return mp_const_none;
}

static mp_obj_t samd_qspiflash_erase(uint32_t addr) {
    wait_for_flash_ready();
    write_enable();
    erase_command(QSPI_CMD_ERASE_SECTOR, addr);

    return mp_const_none;
}

static mp_obj_t samd_qspiflash_readblocks(size_t n_args, const mp_obj_t *args) {
    samd_qspiflash_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t offset = (mp_obj_get_int(args[1]) * self->sectorsize);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_WRITE);
    if (n_args == 4) {
        offset += mp_obj_get_int(args[3]);
    }

    // Read data to flash (adf4 API)
    samd_qspiflash_read(self, offset, bufinfo.buf, bufinfo.len);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(samd_qspiflash_readblocks_obj, 3, 4, samd_qspiflash_readblocks);

static mp_obj_t samd_qspiflash_writeblocks(size_t n_args, const mp_obj_t *args) {
    samd_qspiflash_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t offset = (mp_obj_get_int(args[1]) * self->sectorsize);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_READ);
    if (n_args == 3) {
        samd_qspiflash_erase(offset);
        // TODO check return value
    } else {
        offset += mp_obj_get_int(args[3]);
    }
    // Write data to flash (adf4 API)
    samd_qspiflash_write(self,  offset, bufinfo.buf, bufinfo.len);
    // TODO check return value
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(samd_qspiflash_writeblocks_obj, 3, 4, samd_qspiflash_writeblocks);

static mp_obj_t samd_qspiflash_ioctl(mp_obj_t self_in, mp_obj_t cmd_in, mp_obj_t arg_in) {
    samd_qspiflash_obj_t *self = MP_OBJ_TO_PTR(self_in);
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
            samd_qspiflash_erase(mp_obj_get_int(arg_in) * self->sectorsize);
            // TODO check return value
            return MP_OBJ_NEW_SMALL_INT(0);
        }
        default:
            return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_3(samd_qspiflash_ioctl_obj, samd_qspiflash_ioctl);

static const mp_rom_map_elem_t samd_qspiflash_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&samd_qspiflash_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&samd_qspiflash_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&samd_qspiflash_ioctl_obj) },
};
static MP_DEFINE_CONST_DICT(samd_qspiflash_locals_dict, samd_qspiflash_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    samd_qspiflash_type,
    MP_QSTR_Flash,
    MP_TYPE_FLAG_NONE,
    make_new, samd_qspiflash_make_new,
    locals_dict, &samd_qspiflash_locals_dict
    );

#endif // MICROPY_HW_QSPI_FLASH
