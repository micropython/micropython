/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2019 Lucian Copeland for Adafruit Industries
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
#include "supervisor/internal_flash.h"

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"
#include "py/mphal.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "lib/oofatfs/ff.h"

#include "supervisor/flash.h"
#include "supervisor/usb.h"

#include "csr.h"
#include "irq.h"

enum pin {
    PIN_MOSI = 0,
    PIN_CLK = 1,
    PIN_CS = 2,
    PIN_MISO_EN = 3,
    PIN_MISO = 4, // Value is ignored
};

#define NO_CACHE        0xffffffff

static uint8_t _flash_cache[FLASH_PAGE_SIZE] __attribute__((aligned(4)));
static uint32_t _flash_page_addr = NO_CACHE;
static bool _flash_cache_dirty;
// -------------------------------------------------------------------------
// When performing SPI operations, the flash cannot be accessed.  Since we
// normally execute directly from SPI, this can cause problems.
// To work around this, we execute from RAM.  This is accomplished by marking
// functions as being in the section ".ramtext".
// When building under GCC with -O0 or -Od, the `inline` attribute is ignored.
// Therefore, we must re-implement these functions here and explicitly mark
// them as being in `.ramtext`, even though they really ought to be inlined.
__attribute__((section(".ramtext")))
static inline void spi_writel(uint32_t value, uint32_t addr) {
    *((volatile uint32_t *)addr) = value;
}

__attribute__((section(".ramtext")))
static inline uint32_t spi_readl(uint32_t addr) {
    return *(volatile uint32_t *)addr;
}

__attribute__((section(".ramtext")))
static inline void bb_spi_write(unsigned char value) {
    spi_writel(value, CSR_LXSPI_BITBANG_ADDR);
}

__attribute__((section(".ramtext")))
static inline uint32_t bb_read(void) {
    return spi_readl(CSR_LXSPI_MISO_ADDR);
}

__attribute__((section(".ramtext")))
static inline void bb_spi_en(unsigned int en) {
    spi_writel(en, CSR_LXSPI_BITBANG_EN_ADDR);
}

__attribute__((section(".ramtext")))
static inline void bb_spi_irq_setie(unsigned int ie) {
    if (ie) {
        csrs(mstatus,CSR_MSTATUS_MIE);
    } else {
        csrc(mstatus,CSR_MSTATUS_MIE);
    }
}

__attribute__((section(".ramtext")))
static inline void bb_spi_begin(void) {
    bb_spi_write((0 << PIN_CLK) | (0 << PIN_CS));
}

__attribute__((section(".ramtext")))
static inline void bb_spi_end(void) {
    bb_spi_write((0 << PIN_CLK) | (1 << PIN_CS));
}

__attribute__((section(".ramtext")))
static void spi_single_tx(uint8_t out) {
    int bit;

    for (bit = 7; bit >= 0; bit--) {
        if (out & (1 << bit)) {
            bb_spi_write((0 << PIN_CLK) | (1 << PIN_MOSI));
            bb_spi_write((1 << PIN_CLK) | (1 << PIN_MOSI));
            bb_spi_write((0 << PIN_CLK) | (1 << PIN_MOSI));
        } else {
            bb_spi_write((0 << PIN_CLK) | (0 << PIN_MOSI));
            bb_spi_write((1 << PIN_CLK) | (0 << PIN_MOSI));
            bb_spi_write((0 << PIN_CLK) | (0 << PIN_MOSI));
        }
    }
}

__attribute__((section(".ramtext")))
static uint8_t spi_single_rx(void) {
    int bit = 0;
    uint8_t in = 0;

    bb_spi_write((1 << PIN_MISO_EN) | (0 << PIN_CLK));

    while (bit++ < 8) {
        bb_spi_write((1 << PIN_MISO_EN) | (1 << PIN_CLK));
        in = (in << 1) | bb_read();
        bb_spi_write((1 << PIN_MISO_EN) | (0 << PIN_CLK));
    }

    return in;
}

__attribute__((section(".ramtext")))
static int bb_spi_beginErase4(uint32_t erase_addr) {
    // Enable Write-Enable Latch (WEL)
    bb_spi_begin();
    spi_single_tx(0x06);
    bb_spi_end();

    bb_spi_begin();
    spi_single_tx(0x20);
    spi_single_tx(erase_addr >> 16);
    spi_single_tx(erase_addr >> 8);
    spi_single_tx(erase_addr >> 0);
    bb_spi_end();
    return 0;
}

__attribute__((section(".ramtext")))
static int bb_spi_beginWrite(uint32_t addr, const void *v_data, unsigned int count) {
    const uint8_t write_cmd = 0x02;
    const uint8_t *data = v_data;
    unsigned int i;

    #ifdef NDEBUG
    if (v_data < (const void *)_flash_cache) {
        asm ("ebreak");
    }
    if ((v_data + count) > (const void *)&_flash_cache[4096]) {
        asm ("ebreak");
    }
    #endif

    // Enable Write-Enable Latch (WEL)
    bb_spi_begin();
    spi_single_tx(0x06);
    bb_spi_end();

    bb_spi_begin();
    spi_single_tx(write_cmd);
    spi_single_tx(addr >> 16);
    spi_single_tx(addr >> 8);
    spi_single_tx(addr >> 0);
    for (i = 0; (i < count) && (i < 256); i++) {
        spi_single_tx(*data++);
    }
    bb_spi_end();

    return 0;
}

__attribute__((section(".ramtext")))
static uint8_t spi_read_status(void) {
    uint8_t val;

    bb_spi_begin();
    spi_single_tx(0x05);
    val = spi_single_rx();
    bb_spi_end();
    return val;
}

__attribute__((section(".ramtext")))
static int bb_spi_is_busy(void) {
    return spi_read_status() & (1 << 0);
}

__attribute__((used))
uint32_t page_write_log[128];
__attribute__((used))
uint32_t page_write_log_offset;

__attribute__((section(".ramtext")))
static void bb_spi_write_page(uint32_t flash_address, const uint8_t *data) {
    const uint32_t flash_address_end = flash_address + FLASH_PAGE_SIZE;

    // Ensure we're within the target flash address range.
    if ((flash_address - FLASH_PARTITION_OFFSET_BYTES) > FLASH_SIZE) {
        asm ("ebreak");
        return;
    }
    if (flash_address < FLASH_PARTITION_OFFSET_BYTES) {
        asm ("ebreak");
        return;
    }

    if ((flash_address_end - FLASH_PARTITION_OFFSET_BYTES) > FLASH_SIZE) {
        asm ("ebreak");
        return;
    }
    if (flash_address_end < FLASH_PARTITION_OFFSET_BYTES) {
        asm ("ebreak");
        return;
    }

    // Ensure we're not erasing the middle of a flash bank
    if ((flash_address & 0xfff) != 0) {
        asm ("ebreak");
        return;
    }

    page_write_log[page_write_log_offset++] = flash_address;
    if (page_write_log_offset > sizeof(page_write_log) / sizeof(*page_write_log)) {
        page_write_log_offset = 0;
    }

    while (bb_spi_is_busy()) {
        ; // relax
    }
    bb_spi_beginErase4(flash_address);
    while (bb_spi_is_busy()) {
        ; // relax
    }
    while (flash_address < flash_address_end) {
        bb_spi_beginWrite(flash_address, data, 256);
        while (bb_spi_is_busy()) {
            ; // relax
        }
        flash_address += 256;
        data += 256;
    }
}

static inline uint32_t lba2addr(uint32_t block) {
    return (0x20000000 + FLASH_PARTITION_OFFSET_BYTES) + (block * FILESYSTEM_BLOCK_SIZE);
}

void supervisor_flash_init(void) {
}

uint32_t supervisor_flash_get_block_size(void) {
    return FILESYSTEM_BLOCK_SIZE;
}

uint32_t supervisor_flash_get_block_count(void) {
    return FLASH_SIZE / FILESYSTEM_BLOCK_SIZE;
}

__attribute__((section(".ramtext")))
void port_internal_flash_flush(void) {
    // Skip if data is the same, or if there is no data in the cache
    if (_flash_page_addr == NO_CACHE) {
        return;
    }
    if (!_flash_cache_dirty) {
        return;
    }

    // Disable interrupts and enable bit-bang mode on the SPI flash.
    // This function is running from RAM -- otherwise enabling bitbang mode
    // would crash the CPU as the program suddenly became an endless stream
    // of `0xffffffff`.
    bb_spi_irq_setie(0);
    bb_spi_write((0 << PIN_CLK) | (1 << PIN_CS));
    bb_spi_en(1);

    bb_spi_write_page(_flash_page_addr & 0x00ffffff, (const uint8_t *)_flash_cache);

    bb_spi_en(0);
    bb_spi_irq_setie(1);

    _flash_cache_dirty = false;
}

mp_uint_t supervisor_flash_read_blocks(uint8_t *dest, uint32_t block, uint32_t num_blocks) {
    // Must write out anything in cache before trying to read.
    supervisor_flash_flush();

    uint32_t src = lba2addr(block);
    memcpy(dest, (uint8_t *)src, FILESYSTEM_BLOCK_SIZE * num_blocks);

    #if CIRCUITPY_USB
    usb_background();
    #endif

    return 0;
}

mp_uint_t supervisor_flash_write_blocks(const uint8_t *src, uint32_t lba, uint32_t num_blocks) {
    while (num_blocks) {
        uint32_t const addr = lba2addr(lba);
        uint32_t const page_addr = addr & ~(FLASH_PAGE_SIZE - 1);

        uint32_t count = 8 - (lba % 8); // up to page boundary
        count = MIN(num_blocks, count);

        if (page_addr != _flash_page_addr) {
            // Write out anything in cache before overwriting it.
            supervisor_flash_flush();

            _flash_page_addr = page_addr;
            _flash_cache_dirty = false;

            // Copy the current contents of the entire page into the cache.
            memcpy(_flash_cache, (void *)page_addr, FLASH_PAGE_SIZE);
        }

        // Overwrite part or all of the page cache with the src data, but only if it's changed.
        if (_flash_cache_dirty || memcmp(_flash_cache + (addr & (FLASH_PAGE_SIZE - 1)), src, count * FILESYSTEM_BLOCK_SIZE)) {
            memcpy(_flash_cache + (addr & (FLASH_PAGE_SIZE - 1)), src, count * FILESYSTEM_BLOCK_SIZE);
            _flash_cache_dirty = true;
        }

        // adjust for next run
        lba += count;
        src += count * FILESYSTEM_BLOCK_SIZE;
        num_blocks -= count;

        #if CIRCUITPY_USB
        usb_background();
        #endif
    }

    return 0; // success
}

void supervisor_flash_release_cache(void) {
}
