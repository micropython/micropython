/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2017 Damien P. George
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

#include <stdio.h>
#include <string.h>

#include "py/mperrno.h"
#include "py/mphal.h"
#include "extmod/machine_spi.h"
#include "drivers/memory/spiflash.h"

#define CMD_WRITE (0x02)
#define CMD_READ (0x03)
#define CMD_WRDI (0x04)
#define CMD_RDSR (0x05)
#define CMD_WREN (0x06)
#define CMD_SEC_ERASE (0x20)
#define WAIT_SR_TIMEOUT (1000000)

#define PAGE_SIZE (256) // maximum bytes we can write in one SPI transfer
#define SECTOR_SIZE (4096) // size of erase sector

// Note: this code is not reentrant with this shared buffer
STATIC uint8_t buf[SECTOR_SIZE];

void mp_spiflash_init(mp_spiflash_t *self) {
    mp_hal_pin_write(self->cs, 1);
    mp_hal_pin_output(self->cs);
    const mp_machine_spi_p_t *protocol = self->spi->type->protocol;
    protocol->init(self->spi, 0, NULL, (mp_map_t*)&mp_const_empty_map);
}

STATIC void mp_spiflash_acquire_bus(mp_spiflash_t *self) {
    // can be used for actions needed to acquire bus
    (void)self;
}

STATIC void mp_spiflash_release_bus(mp_spiflash_t *self) {
    // can be used for actions needed to release bus
    (void)self;
}

STATIC void mp_spiflash_transfer(mp_spiflash_t *self, size_t len, const uint8_t *src, uint8_t *dest) {
    const mp_machine_spi_p_t *protocol = self->spi->type->protocol;
    protocol->transfer(self->spi, len, src, dest);
}

STATIC int mp_spiflash_wait_sr(mp_spiflash_t *self, uint8_t mask, uint8_t val, uint32_t timeout) {
    uint8_t cmd[1] = {CMD_RDSR};
    mp_hal_pin_write(self->cs, 0);
    mp_spiflash_transfer(self, 1, cmd, NULL);
    for (; timeout; --timeout) {
        mp_spiflash_transfer(self, 1, cmd, cmd);
        if ((cmd[0] & mask) == val) {
            break;
        }
    }
    mp_hal_pin_write(self->cs, 1);
    if ((cmd[0] & mask) == val) {
        return 0; // success
    } else if (timeout == 0) {
        return -MP_ETIMEDOUT;
    } else {
        return -MP_EIO;
    }
}

STATIC int mp_spiflash_wait_wel1(mp_spiflash_t *self) {
    return mp_spiflash_wait_sr(self, 2, 2, WAIT_SR_TIMEOUT);
}

STATIC int mp_spiflash_wait_wip0(mp_spiflash_t *self) {
    return mp_spiflash_wait_sr(self, 1, 0, WAIT_SR_TIMEOUT);
}

STATIC void mp_spiflash_write_cmd(mp_spiflash_t *self, uint8_t cmd) {
    mp_hal_pin_write(self->cs, 0);
    mp_spiflash_transfer(self, 1, &cmd, NULL);
    mp_hal_pin_write(self->cs, 1);
}

STATIC int mp_spiflash_erase_sector(mp_spiflash_t *self, uint32_t addr) {
    // enable writes
    mp_spiflash_write_cmd(self, CMD_WREN);

    // wait WEL=1
    int ret = mp_spiflash_wait_wel1(self);
    if (ret != 0) {
        return ret;
    }

    // erase the sector
    mp_hal_pin_write(self->cs, 0);
    uint8_t cmd[4] = {CMD_SEC_ERASE, addr >> 16, addr >> 8, addr};
    mp_spiflash_transfer(self, 4, cmd, NULL);
    mp_hal_pin_write(self->cs, 1);

    // wait WIP=0
    return mp_spiflash_wait_wip0(self);
}

STATIC int mp_spiflash_write_page(mp_spiflash_t *self, uint32_t addr, const uint8_t *src) {
    // enable writes
    mp_spiflash_write_cmd(self, CMD_WREN);

    // wait WEL=1
    int ret = mp_spiflash_wait_wel1(self);
    if (ret != 0) {
        return ret;
    }

    // write the page
    mp_hal_pin_write(self->cs, 0);
    uint8_t cmd[4] = {CMD_WRITE, addr >> 16, addr >> 8, addr};
    mp_spiflash_transfer(self, 4, cmd, NULL);
    mp_spiflash_transfer(self, PAGE_SIZE, src, NULL);
    mp_hal_pin_write(self->cs, 1);

    // wait WIP=0
    return mp_spiflash_wait_wip0(self);
}

void mp_spiflash_read(mp_spiflash_t *self, uint32_t addr, size_t len, uint8_t *dest) {
    mp_spiflash_acquire_bus(self);
    uint8_t cmd[4] = {CMD_READ, addr >> 16, addr >> 8, addr};
    mp_hal_pin_write(self->cs, 0);
    mp_spiflash_transfer(self, 4, cmd, NULL);
    mp_spiflash_transfer(self, len, dest, dest);
    mp_hal_pin_write(self->cs, 1);
    mp_spiflash_release_bus(self);
}

int mp_spiflash_write(mp_spiflash_t *self, uint32_t addr, size_t len, const uint8_t *src) {
    // TODO optimise so we don't need to erase multiple times for successive writes to a sector

    // align to 4096 sector
    uint32_t offset = addr & 0xfff;
    addr = (addr >> 12) << 12;

    // restriction for now, so we don't need to erase multiple pages
    if (offset + len > sizeof(buf)) {
        printf("mp_spiflash_write: len is too large\n");
        return -MP_EIO;
    }

    mp_spiflash_acquire_bus(self);

    // read sector
    uint8_t cmd[4] = {CMD_READ, addr >> 16, addr >> 8, addr};
    mp_hal_pin_write(self->cs, 0);
    mp_spiflash_transfer(self, 4, cmd, NULL);
    mp_spiflash_transfer(self, SECTOR_SIZE, buf, buf);
    mp_hal_pin_write(self->cs, 1);

    // erase sector
    int ret = mp_spiflash_erase_sector(self, addr);
    if (ret != 0) {
        mp_spiflash_release_bus(self);
        return ret;
    }

    // copy new block into buffer
    memcpy(buf + offset, src, len);

    // write sector in pages of 256 bytes
    for (int i = 0; i < SECTOR_SIZE; i += 256) {
        ret = mp_spiflash_write_page(self, addr + i, buf + i);
        if (ret != 0) {
            mp_spiflash_release_bus(self);
            return ret;
        }
    }

    mp_spiflash_release_bus(self);
    return 0; // success
}
