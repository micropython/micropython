/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2018 Damien P. George
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
#include "drivers/memory/spiflash.h"

#define QSPI_QE_MASK (0x02)
#define USE_WR_DELAY (1)

#define CMD_WRSR        (0x01)
#define CMD_WRITE       (0x02)
#define CMD_READ        (0x03)
#define CMD_RDSR        (0x05)
#define CMD_WREN        (0x06)
#define CMD_SEC_ERASE   (0x20)
#define CMD_RDCR        (0x35)
#define CMD_RD_DEVID    (0x9f)
#define CMD_CHIP_ERASE  (0xc7)
#define CMD_C4READ      (0xeb)

#define WAIT_SR_TIMEOUT (1000000)

#define PAGE_SIZE (256) // maximum bytes we can write in one SPI transfer
#define SECTOR_SIZE (4096) // size of erase sector

// Note: this code is not reentrant with this shared buffer
STATIC uint8_t buf[SECTOR_SIZE] __attribute__((aligned(4)));
STATIC mp_spiflash_t *bufuser; // current user of buf
STATIC uint32_t bufsec; // current sector stored in buf; 0xffffffff if invalid

STATIC void mp_spiflash_acquire_bus(mp_spiflash_t *self) {
    const mp_spiflash_config_t *c = self->config;
    if (c->bus_kind == MP_SPIFLASH_BUS_QSPI) {
        c->bus.u_qspi.proto->ioctl(c->bus.u_qspi.data, MP_QSPI_IOCTL_BUS_ACQUIRE);
    }
}

STATIC void mp_spiflash_release_bus(mp_spiflash_t *self) {
    const mp_spiflash_config_t *c = self->config;
    if (c->bus_kind == MP_SPIFLASH_BUS_QSPI) {
        c->bus.u_qspi.proto->ioctl(c->bus.u_qspi.data, MP_QSPI_IOCTL_BUS_RELEASE);
    }
}

STATIC void mp_spiflash_write_cmd_data(mp_spiflash_t *self, uint8_t cmd, size_t len, uint32_t data) {
    const mp_spiflash_config_t *c = self->config;
    if (c->bus_kind == MP_SPIFLASH_BUS_SPI) {
        // Note: len/data are unused for standard SPI
        mp_hal_pin_write(c->bus.u_spi.cs, 0);
        c->bus.u_spi.proto->transfer(c->bus.u_spi.data, 1, &cmd, NULL);
        mp_hal_pin_write(c->bus.u_spi.cs, 1);
    } else {
        c->bus.u_qspi.proto->write_cmd_data(c->bus.u_qspi.data, cmd, len, data);
    }
}

STATIC void mp_spiflash_write_cmd_addr_data(mp_spiflash_t *self, uint8_t cmd, uint32_t addr, size_t len, const uint8_t *src) {
    const mp_spiflash_config_t *c = self->config;
    if (c->bus_kind == MP_SPIFLASH_BUS_SPI) {
        uint8_t buf[4] = {cmd, addr >> 16, addr >> 8, addr};
        mp_hal_pin_write(c->bus.u_spi.cs, 0);
        c->bus.u_spi.proto->transfer(c->bus.u_spi.data, 4, buf, NULL);
        if (len) {
            c->bus.u_spi.proto->transfer(c->bus.u_spi.data, len, src, NULL);
        }
        mp_hal_pin_write(c->bus.u_spi.cs, 1);
    } else {
        c->bus.u_qspi.proto->write_cmd_addr_data(c->bus.u_qspi.data, cmd, addr, len, src);
    }
}

STATIC uint32_t mp_spiflash_read_cmd(mp_spiflash_t *self, uint8_t cmd, size_t len) {
    const mp_spiflash_config_t *c = self->config;
    if (c->bus_kind == MP_SPIFLASH_BUS_SPI) {
        uint32_t buf;
        mp_hal_pin_write(c->bus.u_spi.cs, 0);
        c->bus.u_spi.proto->transfer(c->bus.u_spi.data, 1, &cmd, NULL);
        c->bus.u_spi.proto->transfer(c->bus.u_spi.data, len, (void*)&buf, (void*)&buf);
        mp_hal_pin_write(c->bus.u_spi.cs, 1);
        return buf;
    } else {
        return c->bus.u_qspi.proto->read_cmd(c->bus.u_qspi.data, cmd, len);
    }
}

STATIC void mp_spiflash_read_data(mp_spiflash_t *self, uint32_t addr, size_t len, uint8_t *dest) {
    const mp_spiflash_config_t *c = self->config;
    if (c->bus_kind == MP_SPIFLASH_BUS_SPI) {
        uint8_t buf[4] = {CMD_READ, addr >> 16, addr >> 8, addr};
        mp_hal_pin_write(c->bus.u_spi.cs, 0);
        c->bus.u_spi.proto->transfer(c->bus.u_spi.data, 4, buf, NULL);
        c->bus.u_spi.proto->transfer(c->bus.u_spi.data, len, dest, dest);
        mp_hal_pin_write(c->bus.u_spi.cs, 1);
    } else {
        c->bus.u_qspi.proto->read_cmd_qaddr_qdata(c->bus.u_qspi.data, CMD_C4READ, addr, len, dest);
    }
}

STATIC void mp_spiflash_write_cmd(mp_spiflash_t *self, uint8_t cmd) {
    mp_spiflash_write_cmd_data(self, cmd, 0, 0);
}

STATIC void mp_spiflash_write_cmd_addr(mp_spiflash_t *self, uint8_t cmd, uint32_t addr) {
    mp_spiflash_write_cmd_addr_data(self, cmd, addr, 0, NULL);
}

STATIC int mp_spiflash_wait_sr(mp_spiflash_t *self, uint8_t mask, uint8_t val, uint32_t timeout) {
    uint8_t sr;
    for (; timeout; --timeout) {
        sr = mp_spiflash_read_cmd(self, CMD_RDSR, 1);
        if ((sr & mask) == val) {
            break;
        }
    }
    if ((sr & mask) == val) {
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

void mp_spiflash_init(mp_spiflash_t *self) {
    self->flags = 0;

    if (self->config->bus_kind == MP_SPIFLASH_BUS_SPI) {
        mp_hal_pin_write(self->config->bus.u_spi.cs, 1);
        mp_hal_pin_output(self->config->bus.u_spi.cs);
        self->config->bus.u_spi.proto->ioctl(self->config->bus.u_spi.data, MP_SPI_IOCTL_INIT);
    } else {
        self->config->bus.u_qspi.proto->ioctl(self->config->bus.u_qspi.data, MP_QSPI_IOCTL_INIT);
    }

    mp_spiflash_acquire_bus(self);

    #if defined(CHECK_DEVID)
    // Validate device id
    uint32_t devid = mp_spiflash_read_cmd(self, CMD_RD_DEVID, 3);
    if (devid != CHECK_DEVID) {
        return 0;
    }
    #endif

    if (self->config->bus_kind == MP_SPIFLASH_BUS_QSPI) {
        // Set QE bit
        uint32_t data = (mp_spiflash_read_cmd(self, CMD_RDSR, 1) & 0xff)
            | (mp_spiflash_read_cmd(self, CMD_RDCR, 1) & 0xff) << 8;
        if (!(data & (QSPI_QE_MASK << 8))) {
            data |= QSPI_QE_MASK << 8;
            mp_spiflash_write_cmd(self, CMD_WREN);
            mp_spiflash_write_cmd_data(self, CMD_WRSR, 2, data);
            mp_spiflash_wait_wip0(self);
        }
    }

    mp_spiflash_release_bus(self);
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
    mp_spiflash_write_cmd_addr(self, CMD_SEC_ERASE, addr);

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
    mp_spiflash_write_cmd_addr_data(self, CMD_WRITE, addr, PAGE_SIZE, src);

    // wait WIP=0
    return mp_spiflash_wait_wip0(self);
}

void mp_spiflash_read(mp_spiflash_t *self, uint32_t addr, size_t len, uint8_t *dest) {
    if (len == 0) {
        return;
    }
    mp_spiflash_acquire_bus(self);
    if (bufuser == self && bufsec != 0xffffffff) {
        uint32_t bis = addr / SECTOR_SIZE;
        int rest = 0;
        if (bis < bufsec) {
            rest = bufsec * SECTOR_SIZE - addr;
            if (rest > len) {
                rest = len;
            }
            mp_spiflash_read_data(self, addr, rest, dest);
            len -= rest;
            if (len <= 0) {
                mp_spiflash_release_bus(self);
                return;
            } else {
                // Something from buffer...
                addr = bufsec * SECTOR_SIZE;
                dest += rest;
                if (len > SECTOR_SIZE) {
                    rest = SECTOR_SIZE;
                } else {
                    rest = len;
                }
                memcpy(dest, buf, rest);
                len -= rest;
                if (len <= 0) {
                    mp_spiflash_release_bus(self);
                    return;
                }
                dest += rest;
                addr += rest;
            }
        } else if (bis == bufsec) {
            uint32_t offset = addr & (SECTOR_SIZE-1);
            rest = SECTOR_SIZE - offset;
            if (rest > len) {
                rest = len;
            }
            memcpy(dest, &buf[offset], rest);
            len -= rest;
            if (len <= 0) {
                mp_spiflash_release_bus(self);
                return;
            }
        }
        dest += rest;
        addr += rest;
    }
    // Read rest direct from flash
    mp_spiflash_read_data(self, addr, len, dest);
    mp_spiflash_release_bus(self);
}

STATIC void mp_spiflash_flush_internal(mp_spiflash_t *self) {
    #if USE_WR_DELAY
    if (!(self->flags & 1)) {
        return;
    }

    self->flags &= ~1;

    // Erase sector
    int ret = mp_spiflash_erase_sector(self, bufsec * SECTOR_SIZE);
    if (ret != 0) {
        return;
    }

    // Write
    for (int i = 0; i < 16; i += 1) {
        int ret = mp_spiflash_write_page(self, bufsec * SECTOR_SIZE + i * PAGE_SIZE, buf + i * PAGE_SIZE);
        if (ret != 0) {
            return;
        }
    }
    #endif
}

void mp_spiflash_flush(mp_spiflash_t *self) {
    mp_spiflash_acquire_bus(self);
    mp_spiflash_flush_internal(self);
    mp_spiflash_release_bus(self);
}

STATIC int mp_spiflash_write_part(mp_spiflash_t *self, uint32_t addr, size_t len, const uint8_t *src) {
    // Align to 4096 sector
    uint32_t offset = addr & 0xfff;
    uint32_t sec = addr >> 12;
    addr = sec << 12;

    // Restriction for now, so we don't need to erase multiple pages
    if (offset + len > sizeof(buf)) {
        printf("mp_spiflash_write_part: len is too large\n");
        return -MP_EIO;
    }

    // Acquire the sector buffer
    if (bufuser != self) {
        if (bufuser != NULL) {
            mp_spiflash_flush(bufuser);
        }
        bufuser = self;
        bufsec = 0xffffffff;
    }

    if (bufsec != sec) {
        // Read sector
        #if USE_WR_DELAY
        if (bufsec != 0xffffffff) {
            mp_spiflash_flush_internal(self);
        }
        #endif
        mp_spiflash_read_data(self, addr, SECTOR_SIZE, buf);
    }

    #if USE_WR_DELAY

    bufsec = sec;
    // Just copy to buffer
    memcpy(buf + offset, src, len);
    // And mark dirty
    self->flags |= 1;

    #else

    uint32_t dirty = 0;
    for (size_t i = 0; i < len; ++i) {
        if (buf[offset + i] != src[i]) {
            if (buf[offset + i] != 0xff) {
                // Erase sector
                int ret = mp_spiflash_erase_sector(self, addr);
                if (ret != 0) {
                    return ret;
                }
                dirty = 0xffff;
                break;
            } else {
                dirty |= (1 << ((offset + i) >> 8));
            }
        }
    }

    bufsec = sec;
    // Copy new block into buffer
    memcpy(buf + offset, src, len);

    // Write sector in pages of 256 bytes
    for (size_t i = 0; i < 16; ++i) {
        if (dirty & (1 << i)) {
            int ret = mp_spiflash_write_page(self, addr + i * PAGE_SIZE, buf + i * PAGE_SIZE);
            if (ret != 0) {
                return ret;
            }
        }
    }

    #endif

    return 0; // success
}

int mp_spiflash_write(mp_spiflash_t *self, uint32_t addr, size_t len, const uint8_t *src) {
    uint32_t bis = addr / SECTOR_SIZE;
    uint32_t bie = (addr + len - 1) / SECTOR_SIZE;

    mp_spiflash_acquire_bus(self);
    if (bufuser == self && bis <= bufsec && bie >= bufsec) {
        // Current buffer affected, handle this part first
        uint32_t taddr = (bufsec + 1) * SECTOR_SIZE;
        int32_t offset = addr - bufsec * SECTOR_SIZE;
        int32_t pre = bufsec * SECTOR_SIZE - addr;
        if (offset < 0) {
            offset = 0;
        } else {
            pre = 0;
        }
        int32_t rest = len - pre;
        int32_t trail = 0;
        if (rest > SECTOR_SIZE - offset) {
            trail = rest - (SECTOR_SIZE - offset);
            rest = SECTOR_SIZE - offset;
        }
        memcpy(&buf[offset], &src[pre], rest);
        self->flags |= 1; // Mark dirty
        if ((pre | trail) == 0) {
            mp_spiflash_release_bus(self);
            return 0;
        }
        const uint8_t *p = src;
        while (pre) {
            int rest = pre & (SECTOR_SIZE - 1);
            if (rest == 0) {
                rest = SECTOR_SIZE;
            }
            mp_spiflash_write_part(self, addr, rest, p);
            p += rest;
            addr += rest;
            pre -= rest;
        }
        while (trail) {
            int rest = trail;
            if (rest > SECTOR_SIZE) {
                rest = SECTOR_SIZE;
            }
            mp_spiflash_write_part(self, taddr, rest, src);
            src += rest;
            taddr += rest;
            trail -= rest;
        }
    } else {
        // Current buffer not affected, business as usual
        uint32_t offset = addr & (SECTOR_SIZE - 1);
        while (len) {
            int rest = SECTOR_SIZE - offset;
            if (rest > len) {
                rest = len;
            }
            mp_spiflash_write_part(self, addr, rest, src);
            len -= rest;
            addr += rest;
            src += rest;
            offset = 0;
        }
    }
    mp_spiflash_release_bus(self);
    return 0;
}
