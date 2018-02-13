/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017-2018 Damien P. George
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

#include "py/obj.h"
#include "storage.h"

#if defined(MICROPY_HW_SPIFLASH_SIZE_BITS)

#include "drivers/memory/spiflash.h"
#include "genhdr/pins.h"

STATIC const mp_machine_soft_spi_obj_t spiflash_spi_bus = {
    .base = {&mp_machine_soft_spi_type},
    .delay_half = MICROPY_PY_MACHINE_SPI_MIN_DELAY,
    .polarity = 0,
    .phase = 0,
    .sck = &MICROPY_HW_SPIFLASH_SCK,
    .mosi = &MICROPY_HW_SPIFLASH_MOSI,
    .miso = &MICROPY_HW_SPIFLASH_MISO,
};

STATIC const mp_spiflash_t spiflash = {
    .cs = &MICROPY_HW_SPIFLASH_CS,
    .spi = (mp_obj_base_t*)&spiflash_spi_bus.base,
};

void spi_bdev_init(void) {
    mp_spiflash_init((mp_spiflash_t*)&spiflash);
}

bool spi_bdev_readblock(uint8_t *dest, uint32_t block) {
    // we must disable USB irqs to prevent MSC contention with SPI flash
    uint32_t basepri = raise_irq_pri(IRQ_PRI_OTG_FS);

    mp_spiflash_read((mp_spiflash_t*)&spiflash,
        block * FLASH_BLOCK_SIZE, FLASH_BLOCK_SIZE, dest);

    restore_irq_pri(basepri);

    return true;
}

bool spi_bdev_writeblock(const uint8_t *src, uint32_t block) {
    // we must disable USB irqs to prevent MSC contention with SPI flash
    uint32_t basepri = raise_irq_pri(IRQ_PRI_OTG_FS);

    int ret = mp_spiflash_write((mp_spiflash_t*)&spiflash,
        block * FLASH_BLOCK_SIZE, FLASH_BLOCK_SIZE, src);

    restore_irq_pri(basepri);

    return ret == 0;
}

#endif // defined(MICROPY_HW_SPIFLASH_SIZE_BITS)
