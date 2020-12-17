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
#ifndef MICROPY_INCLUDED_DRIVERS_MEMORY_SPIFLASH_H
#define MICROPY_INCLUDED_DRIVERS_MEMORY_SPIFLASH_H

#include "drivers/bus/spi.h"
#include "drivers/bus/qspi.h"

#define MP_SPIFLASH_ERASE_BLOCK_SIZE (4096) // must be a power of 2

enum {
    MP_SPIFLASH_BUS_SPI,
    MP_SPIFLASH_BUS_QSPI,
};

struct _mp_spiflash_t;

#if MICROPY_HW_SPIFLASH_ENABLE_CACHE
// A cache must be provided by the user in the config struct.  The same cache
// struct can be shared by multiple SPI flash instances.
typedef struct _mp_spiflash_cache_t {
    uint8_t buf[MP_SPIFLASH_ERASE_BLOCK_SIZE] __attribute__((aligned(4)));
    struct _mp_spiflash_t *user; // current user of buf, for shared use
    uint32_t block; // current block stored in buf; 0xffffffff if invalid
} mp_spiflash_cache_t;
#endif

typedef struct _mp_spiflash_config_t {
    uint32_t bus_kind;
    union {
        struct {
            mp_hal_pin_obj_t cs;
            void *data;
            const mp_spi_proto_t *proto;
        } u_spi;
        struct {
            void *data;
            const mp_qspi_proto_t *proto;
        } u_qspi;
    } bus;
    #if MICROPY_HW_SPIFLASH_ENABLE_CACHE
    mp_spiflash_cache_t *cache; // can be NULL if cache functions not used
    #endif
} mp_spiflash_config_t;

typedef struct _mp_spiflash_t {
    const mp_spiflash_config_t *config;
    volatile uint32_t flags;
} mp_spiflash_t;

void mp_spiflash_init(mp_spiflash_t *self);
void mp_spiflash_deepsleep(mp_spiflash_t *self, int value);

// These functions go direct to the SPI flash device
int mp_spiflash_erase_block(mp_spiflash_t *self, uint32_t addr);
void mp_spiflash_read(mp_spiflash_t *self, uint32_t addr, size_t len, uint8_t *dest);
int mp_spiflash_write(mp_spiflash_t *self, uint32_t addr, size_t len, const uint8_t *src);

#if MICROPY_HW_SPIFLASH_ENABLE_CACHE
// These functions use the cache (which must already be configured)
void mp_spiflash_cache_flush(mp_spiflash_t *self);
void mp_spiflash_cached_read(mp_spiflash_t *self, uint32_t addr, size_t len, uint8_t *dest);
int mp_spiflash_cached_write(mp_spiflash_t *self, uint32_t addr, size_t len, const uint8_t *src);
#endif

#endif // MICROPY_INCLUDED_DRIVERS_MEMORY_SPIFLASH_H
