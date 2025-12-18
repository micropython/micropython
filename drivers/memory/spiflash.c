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

#if defined(CHECK_DEVID)
#error "CHECK_DEVID no longer supported, use MICROPY_HW_SPIFLASH_DETECT_DEVICE instead"
#endif

// The default number of dummy bytes for quad-read is 2.  This can be changed by enabling
// MICROPY_HW_SPIFLASH_CHIP_PARAMS and configuring the value in mp_spiflash_chip_params_t.
#if MICROPY_HW_SPIFLASH_CHIP_PARAMS
#define MICROPY_HW_SPIFLASH_QREAD_NUM_DUMMY(spiflash) (spiflash->chip_params->qread_num_dummy)
#else
#define MICROPY_HW_SPIFLASH_QREAD_NUM_DUMMY(spiflash) (2)
#endif

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
#define CMD_RSTEN       (0x66)
#define CMD_RESET       (0x99)

// 32 bit addressing commands
#define CMD_WRITE_32    (0x12)
#define CMD_READ_32     (0x13)
#define CMD_SEC_ERASE_32 (0x21)
#define CMD_C4READ_32   (0xec)

#define WAIT_SR_TIMEOUT (1000000)

#define PAGE_SIZE (256) // maximum bytes we can write in one SPI transfer
#define SECTOR_SIZE MP_SPIFLASH_ERASE_BLOCK_SIZE

static void mp_spiflash_acquire_bus(mp_spiflash_t *self) {
    const mp_spiflash_config_t *c = self->config;
    if (c->bus_kind == MP_SPIFLASH_BUS_QSPI) {
        c->bus.u_qspi.proto->ioctl(c->bus.u_qspi.data, MP_QSPI_IOCTL_BUS_ACQUIRE, 0);
    }
}

static void mp_spiflash_release_bus(mp_spiflash_t *self) {
    const mp_spiflash_config_t *c = self->config;
    if (c->bus_kind == MP_SPIFLASH_BUS_QSPI) {
        c->bus.u_qspi.proto->ioctl(c->bus.u_qspi.data, MP_QSPI_IOCTL_BUS_RELEASE, 0);
    }
}

static void mp_spiflash_notify_modified(mp_spiflash_t *self, uint32_t addr, uint32_t len) {
    const mp_spiflash_config_t *c = self->config;
    if (c->bus_kind == MP_SPIFLASH_BUS_QSPI) {
        uintptr_t arg[2] = { addr, len };
        c->bus.u_qspi.proto->ioctl(c->bus.u_qspi.data, MP_QSPI_IOCTL_MEMORY_MODIFIED, (uintptr_t)&arg[0]);
    }
}

static int mp_spiflash_write_cmd_data(mp_spiflash_t *self, uint8_t cmd, size_t len, uint32_t data) {
    int ret = 0;
    const mp_spiflash_config_t *c = self->config;
    if (c->bus_kind == MP_SPIFLASH_BUS_SPI) {
        // Note: len/data are unused for standard SPI
        mp_hal_pin_write(c->bus.u_spi.cs, 0);
        c->bus.u_spi.proto->transfer(c->bus.u_spi.data, 1, &cmd, NULL);
        mp_hal_pin_write(c->bus.u_spi.cs, 1);
    } else {
        ret = c->bus.u_qspi.proto->write_cmd_data(c->bus.u_qspi.data, cmd, len, data);
    }
    return ret;
}

static int mp_spiflash_transfer_cmd_addr_data(mp_spiflash_t *self, uint8_t cmd, uint32_t addr, size_t len, const uint8_t *src, uint8_t *dest) {
    int ret = 0;
    const mp_spiflash_config_t *c = self->config;
    if (c->bus_kind == MP_SPIFLASH_BUS_SPI) {
        uint8_t buf[5] = {cmd, 0};
        uint8_t buff_len = 1 + mp_spi_set_addr_buff(&buf[1], addr);
        mp_hal_pin_write(c->bus.u_spi.cs, 0);
        c->bus.u_spi.proto->transfer(c->bus.u_spi.data, buff_len, buf, NULL);
        if (len && (src != NULL)) {
            c->bus.u_spi.proto->transfer(c->bus.u_spi.data, len, src, NULL);
        } else if (len && (dest != NULL)) {
            c->bus.u_spi.proto->transfer(c->bus.u_spi.data, len, dest, dest);
        }

        mp_hal_pin_write(c->bus.u_spi.cs, 1);
    } else {
        if (dest != NULL) {
            uint8_t num_dummy = MICROPY_HW_SPIFLASH_QREAD_NUM_DUMMY(self);
            ret = c->bus.u_qspi.proto->read_cmd_qaddr_qdata(c->bus.u_qspi.data, cmd, addr, num_dummy, len, dest);
        } else {
            ret = c->bus.u_qspi.proto->write_cmd_addr_data(c->bus.u_qspi.data, cmd, addr, len, src);
        }
    }
    return ret;
}

static int mp_spiflash_read_cmd(mp_spiflash_t *self, uint8_t cmd, size_t len, uint32_t *dest) {
    const mp_spiflash_config_t *c = self->config;
    if (c->bus_kind == MP_SPIFLASH_BUS_SPI) {
        mp_hal_pin_write(c->bus.u_spi.cs, 0);
        c->bus.u_spi.proto->transfer(c->bus.u_spi.data, 1, &cmd, NULL);
        c->bus.u_spi.proto->transfer(c->bus.u_spi.data, len, (void*)dest, (void*)dest);
        mp_hal_pin_write(c->bus.u_spi.cs, 1);
        return 0;
    } else {
        return c->bus.u_qspi.proto->read_cmd(c->bus.u_qspi.data, cmd, len, dest);
    }
}

static int mp_spiflash_read_data(mp_spiflash_t *self, uint32_t addr, size_t len, uint8_t *dest) {
    const mp_spiflash_config_t *c = self->config;
    uint8_t cmd;
    if (c->bus_kind == MP_SPIFLASH_BUS_SPI) {
        cmd = MICROPY_HW_SPI_ADDR_IS_32BIT(addr) ? CMD_READ_32 : CMD_READ;
    } else {
        cmd = MICROPY_HW_SPI_ADDR_IS_32BIT(addr) ? CMD_C4READ_32 : CMD_C4READ;
    }
    return mp_spiflash_transfer_cmd_addr_data(self, cmd, addr, len, NULL, dest);
}

static int mp_spiflash_write_cmd(mp_spiflash_t *self, uint8_t cmd) {
    return mp_spiflash_write_cmd_data(self, cmd, 0, 0);
}

static int mp_spiflash_wait_sr(mp_spiflash_t *self, uint8_t mask, uint8_t val, uint32_t timeout) {
    do {
        uint32_t sr;
        int ret = mp_spiflash_read_cmd(self, CMD_RDSR, 1, &sr);
        if (ret != 0) {
            return ret;
        }
        if ((sr & mask) == val) {
            return 0; // success
        }
    } while (timeout--);

    return -MP_ETIMEDOUT;
}

static int mp_spiflash_wait_wel1(mp_spiflash_t *self) {
    return mp_spiflash_wait_sr(self, 2, 2, WAIT_SR_TIMEOUT);
}

static int mp_spiflash_wait_wip0(mp_spiflash_t *self) {
    return mp_spiflash_wait_sr(self, 1, 0, WAIT_SR_TIMEOUT);
}

static inline void mp_spiflash_deepsleep_internal(mp_spiflash_t *self, int value) {
    mp_spiflash_write_cmd(self, value ? 0xb9 : 0xab); // sleep/wake
}

void mp_spiflash_init(mp_spiflash_t *self) {
    self->flags = 0;

    if (self->config->bus_kind == MP_SPIFLASH_BUS_SPI) {
        mp_hal_pin_write(self->config->bus.u_spi.cs, 1);
        mp_hal_pin_output(self->config->bus.u_spi.cs);
        self->config->bus.u_spi.proto->ioctl(self->config->bus.u_spi.data, MP_SPI_IOCTL_INIT);
    } else {
        uint8_t num_dummy = MICROPY_HW_SPIFLASH_QREAD_NUM_DUMMY(self);
        self->config->bus.u_qspi.proto->ioctl(self->config->bus.u_qspi.data, MP_QSPI_IOCTL_INIT, num_dummy);
        if (self->config->bus.u_qspi.proto->direct_read != NULL) {
            // A bus with a custom read function should not have any further initialisation done.
            return;
        }
    }

    mp_spiflash_acquire_bus(self);

    // Ensure SPI flash is out of sleep mode
    mp_spiflash_deepsleep_internal(self, 0);

    // Software reset.
    #if MICROPY_HW_SPIFLASH_SOFT_RESET
    mp_spiflash_write_cmd(self, CMD_RSTEN);
    mp_spiflash_write_cmd(self, CMD_RESET);
    mp_spiflash_wait_wip0(self);
    mp_hal_delay_ms(1);
    #endif

    #if MICROPY_HW_SPIFLASH_DETECT_DEVICE
    // Attempt to detect SPI flash based on its JEDEC id.
    uint32_t devid;
    int ret = mp_spiflash_read_cmd(self, CMD_RD_DEVID, 3, &devid);
    ret = mp_spiflash_detect(self, ret, devid);
    if (ret != 0) {
        // Could not read device id.
        mp_spiflash_release_bus(self);
        return;
    }
    #endif

    if (self->config->bus_kind == MP_SPIFLASH_BUS_QSPI) {
        // Set QE bit
        uint32_t sr = 0, cr = 0;
        int ret = mp_spiflash_read_cmd(self, CMD_RDSR, 1, &sr);
        if (ret == 0) {
            ret = mp_spiflash_read_cmd(self, CMD_RDCR, 1, &cr);
        }
        uint32_t data = (sr & 0xff) | (cr & 0xff) << 8;
        if (ret == 0 && !(data & (QSPI_QE_MASK << 8))) {
            data |= QSPI_QE_MASK << 8;
            mp_spiflash_write_cmd(self, CMD_WREN);
            mp_spiflash_write_cmd_data(self, CMD_WRSR, 2, data);
            mp_spiflash_wait_wip0(self);
        }
    }

    mp_spiflash_release_bus(self);
}

void mp_spiflash_deepsleep(mp_spiflash_t *self, int value) {
    if (value) {
        mp_spiflash_acquire_bus(self);
    }
    mp_spiflash_deepsleep_internal(self, value);
    if (!value) {
        mp_spiflash_release_bus(self);
    }
}

static int mp_spiflash_erase_block_internal(mp_spiflash_t *self, uint32_t addr) {
    int ret = 0;
    // enable writes
    ret = mp_spiflash_write_cmd(self, CMD_WREN);
    if (ret != 0) {
        return ret;
    }

    // wait WEL=1
    ret = mp_spiflash_wait_wel1(self);
    if (ret != 0) {
        return ret;
    }

    // erase the sector
    uint8_t cmd = MICROPY_HW_SPI_ADDR_IS_32BIT(addr) ? CMD_SEC_ERASE_32 : CMD_SEC_ERASE;
    ret = mp_spiflash_transfer_cmd_addr_data(self, cmd, addr, 0, NULL, NULL);
    if (ret != 0) {
        return ret;
    }

    // wait WIP=0
    return mp_spiflash_wait_wip0(self);
}

static int mp_spiflash_write_page(mp_spiflash_t *self, uint32_t addr, size_t len, const uint8_t *src) {
    int ret = 0;
    // enable writes
    ret = mp_spiflash_write_cmd(self, CMD_WREN);
    if (ret != 0) {
        return ret;
    }

    // wait WEL=1
    ret = mp_spiflash_wait_wel1(self);
    if (ret != 0) {
        return ret;
    }

    // write the page
    uint8_t cmd = MICROPY_HW_SPI_ADDR_IS_32BIT(addr) ? CMD_WRITE_32 : CMD_WRITE;
    ret = mp_spiflash_transfer_cmd_addr_data(self, cmd, addr, len, src, NULL);
    if (ret != 0) {
        return ret;
    }

    // wait WIP=0
    return mp_spiflash_wait_wip0(self);
}

/******************************************************************************/
// Interface functions that go direct to the SPI flash device

int mp_spiflash_erase_block(mp_spiflash_t *self, uint32_t addr) {
    mp_spiflash_acquire_bus(self);
    int ret = mp_spiflash_erase_block_internal(self, addr);
    mp_spiflash_notify_modified(self, addr, SECTOR_SIZE);
    mp_spiflash_release_bus(self);
    return ret;
}

int mp_spiflash_read(mp_spiflash_t *self, uint32_t addr, size_t len, uint8_t *dest) {
    if (len == 0) {
        return 0;
    }
    const mp_spiflash_config_t *c = self->config;
    if (c->bus_kind == MP_SPIFLASH_BUS_QSPI && c->bus.u_qspi.proto->direct_read != NULL) {
        return c->bus.u_qspi.proto->direct_read(c->bus.u_qspi.data, addr, len, dest);
    }
    mp_spiflash_acquire_bus(self);
    int ret = mp_spiflash_read_data(self, addr, len, dest);
    mp_spiflash_release_bus(self);
    return ret;
}

int mp_spiflash_write(mp_spiflash_t *self, uint32_t addr, size_t len, const uint8_t *src) {
    uint32_t orig_addr = addr;
    uint32_t orig_len = len;
    mp_spiflash_acquire_bus(self);
    int ret = 0;
    uint32_t offset = addr & (PAGE_SIZE - 1);
    while (len) {
        size_t rest = PAGE_SIZE - offset;
        if (rest > len) {
            rest = len;
        }
        ret = mp_spiflash_write_page(self, addr, rest, src);
        if (ret != 0) {
            break;
        }
        len -= rest;
        addr += rest;
        src += rest;
        offset = 0;
    }
    mp_spiflash_notify_modified(self, orig_addr, orig_len);
    mp_spiflash_release_bus(self);
    return ret;
}

/******************************************************************************/
// Interface functions that use the cache
//
// These functions do not call mp_spiflash_notify_modified(), so shouldn't be
// used for memory-mapped flash (for example).

#if MICROPY_HW_SPIFLASH_ENABLE_CACHE

int mp_spiflash_cached_read(mp_spiflash_t *self, uint32_t addr, size_t len, uint8_t *dest) {
    if (len == 0) {
        return 0;
    }
    mp_spiflash_acquire_bus(self);
    mp_spiflash_cache_t *cache = self->config->cache;
    if (cache->user == self && cache->block != 0xffffffff) {
        uint32_t bis = addr / SECTOR_SIZE;
        uint32_t bie = (addr + len - 1) / SECTOR_SIZE;
        if (bis <= cache->block && cache->block <= bie) {
            // Read straddles current buffer
            size_t rest = 0;
            if (bis < cache->block) {
                // Read direct from flash for first part
                rest = cache->block * SECTOR_SIZE - addr;
                int ret = mp_spiflash_read_data(self, addr, rest, dest);
                if (ret != 0) {
                    mp_spiflash_release_bus(self);
                    return ret;
                }
                len -= rest;
                dest += rest;
                addr += rest;
            }
            uint32_t offset = addr & (SECTOR_SIZE - 1);
            rest = SECTOR_SIZE - offset;
            if (rest > len) {
                rest = len;
            }
            memcpy(dest, &cache->buf[offset], rest);
            len -= rest;
            if (len == 0) {
                mp_spiflash_release_bus(self);
                return 0;
            }
            dest += rest;
            addr += rest;
        }
    }
    // Read rest direct from flash
    int ret = mp_spiflash_read_data(self, addr, len, dest);
    mp_spiflash_release_bus(self);
    return ret;
}

static int mp_spiflash_cache_flush_internal(mp_spiflash_t *self) {
    #if USE_WR_DELAY
    if (!(self->flags & 1)) {
        return 0;
    }

    self->flags &= ~1;

    mp_spiflash_cache_t *cache = self->config->cache;

    // Erase sector
    int ret = mp_spiflash_erase_block_internal(self, cache->block * SECTOR_SIZE);
    if (ret != 0) {
        return ret;
    }

    // Write
    for (int i = 0; i < 16; i += 1) {
        uint32_t addr = cache->block * SECTOR_SIZE + i * PAGE_SIZE;
        int ret = mp_spiflash_write_page(self, addr, PAGE_SIZE, cache->buf + i * PAGE_SIZE);
        if (ret != 0) {
            return ret;
        }
    }
    #endif
    return 0;
}

int mp_spiflash_cache_flush(mp_spiflash_t *self) {
    mp_spiflash_acquire_bus(self);
    int ret = mp_spiflash_cache_flush_internal(self);
    mp_spiflash_release_bus(self);
    return ret;
}

static int mp_spiflash_cached_write_part(mp_spiflash_t *self, uint32_t addr, size_t len, const uint8_t *src) {
    // Align to 4096 sector
    uint32_t offset = addr & 0xfff;
    uint32_t sec = addr >> 12;
    addr = sec << 12;

    // Restriction for now, so we don't need to erase multiple pages
    if (offset + len > SECTOR_SIZE) {
        printf("mp_spiflash_cached_write_part: len is too large\n");
        return -MP_EIO;
    }

    mp_spiflash_cache_t *cache = self->config->cache;

    // Acquire the sector buffer
    if (cache->user != self) {
        if (cache->user != NULL) {
            mp_spiflash_cache_flush(cache->user);
        }
        cache->user = self;
        cache->block = 0xffffffff;
    }

    if (cache->block != sec) {
        // Read sector
        #if USE_WR_DELAY
        if (cache->block != 0xffffffff) {
            int ret = mp_spiflash_cache_flush_internal(self);
            if (ret != 0) {
                return ret;
            }
        }
        #endif
        int ret = mp_spiflash_read_data(self, addr, SECTOR_SIZE, cache->buf);
        if (ret != 0) {
            return ret;
        }
    }

    #if USE_WR_DELAY

    cache->block = sec;
    // Just copy to buffer
    memcpy(cache->buf + offset, src, len);
    // And mark dirty
    self->flags |= 1;

    #else

    uint32_t dirty = 0;
    for (size_t i = 0; i < len; ++i) {
        if (cache->buf[offset + i] != src[i]) {
            if (cache->buf[offset + i] != 0xff) {
                // Erase sector
                int ret = mp_spiflash_erase_block_internal(self, addr);
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

    cache->block = sec;
    // Copy new block into buffer
    memcpy(cache->buf + offset, src, len);

    // Write sector in pages of 256 bytes
    for (size_t i = 0; i < 16; ++i) {
        if (dirty & (1 << i)) {
            int ret = mp_spiflash_write_page(self, addr + i * PAGE_SIZE, PAGE_SIZE, cache->buf + i * PAGE_SIZE);
            if (ret != 0) {
                return ret;
            }
        }
    }

    #endif

    return 0; // success
}

int mp_spiflash_cached_write(mp_spiflash_t *self, uint32_t addr, size_t len, const uint8_t *src) {
    uint32_t bis = addr / SECTOR_SIZE;
    uint32_t bie = (addr + len - 1) / SECTOR_SIZE;

    mp_spiflash_acquire_bus(self);

    mp_spiflash_cache_t *cache = self->config->cache;
    if (cache->user == self && bis <= cache->block && bie >= cache->block) {
        // Write straddles current buffer
        uint32_t pre;
        uint32_t offset;
        if (cache->block * SECTOR_SIZE >= addr) {
            pre = cache->block * SECTOR_SIZE - addr;
            offset = 0;
        } else {
            pre = 0;
            offset = addr - cache->block * SECTOR_SIZE;
        }

        // Write buffered part first
        uint32_t len_in_buf = len - pre;
        len = 0;
        if (len_in_buf > SECTOR_SIZE - offset) {
            len = len_in_buf - (SECTOR_SIZE - offset);
            len_in_buf = SECTOR_SIZE - offset;
        }
        memcpy(&cache->buf[offset], &src[pre], len_in_buf);
        self->flags |= 1; // Mark dirty

        // Write part before buffer sector
        while (pre) {
            int rest = pre & (SECTOR_SIZE - 1);
            if (rest == 0) {
                rest = SECTOR_SIZE;
            }
            int ret = mp_spiflash_cached_write_part(self, addr, rest, src);
            if (ret != 0) {
                mp_spiflash_release_bus(self);
                return ret;
            }
            src += rest;
            addr += rest;
            pre -= rest;
        }
        src += len_in_buf;
        addr += len_in_buf;

        // Fall through to write remaining part
    }

    uint32_t offset = addr & (SECTOR_SIZE - 1);
    while (len) {
        int rest = SECTOR_SIZE - offset;
        if (rest > len) {
            rest = len;
        }
        int ret = mp_spiflash_cached_write_part(self, addr, rest, src);
        if (ret != 0) {
            mp_spiflash_release_bus(self);
            return ret;
        }
        len -= rest;
        addr += rest;
        src += rest;
        offset = 0;
    }

    mp_spiflash_release_bus(self);
    return 0;
}

#endif // MICROPY_HW_SPIFLASH_ENABLE_CACHE
