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
#include "systick.h"
#include "led.h"
#include "storage.h"

#if defined(MICROPY_HW_SPIFLASH_SIZE_BITS)

#include "drivers/memory/spiflash.h"
#include "genhdr/pins.h"

static uint32_t flash_tick_counter_last_write;

STATIC const mp_machine_soft_spi_obj_t spiflash_spi_bus = {
    .base = {&mp_machine_soft_spi_type},
    .delay_half = MICROPY_PY_MACHINE_SPI_MIN_DELAY,
    .polarity = 0,
    .phase = 0,
    .sck = &MICROPY_HW_SPIFLASH_SCK,
    .mosi = &MICROPY_HW_SPIFLASH_MOSI,
    .miso = &MICROPY_HW_SPIFLASH_MISO,
};

STATIC const mp_spiflash_config_t spiflash_config = {
    .bus_kind = MP_SPIFLASH_BUS_SPI,
    .bus.u_spi.cs = &MICROPY_HW_SPIFLASH_CS,
    .bus.u_spi.data = (void*)&spiflash_spi_bus,
    .bus.u_spi.proto = &mp_machine_soft_spi_p,
};

STATIC mp_spiflash_t spiflash;

void spi_bdev_init(void) {
    spiflash.config = &spiflash_config;
    mp_spiflash_init(&spiflash);
    flash_tick_counter_last_write = 0;
}

void spi_bdev_irq_handler(void) {
    if ((spiflash.flags & 1) && sys_tick_has_passed(flash_tick_counter_last_write, 1000)) {
        mp_spiflash_flush(&spiflash);
        led_state(PYB_LED_RED, 0); // indicate a clean cache with LED off
    }
}

void spi_bdev_flush(void) {
    if (spiflash.flags & 1) {
        // we must disable USB irqs to prevent MSC contention with SPI flash
        uint32_t basepri = raise_irq_pri(IRQ_PRI_OTG_FS);
        mp_spiflash_flush(&spiflash);
        led_state(PYB_LED_RED, 0); // indicate a clean cache with LED off
        restore_irq_pri(basepri);
    }
}

bool spi_bdev_readblock(uint8_t *dest, uint32_t block) {
    // we must disable USB irqs to prevent MSC contention with SPI flash
    uint32_t basepri = raise_irq_pri(IRQ_PRI_OTG_FS);
    mp_spiflash_read(&spiflash, block * FLASH_BLOCK_SIZE, FLASH_BLOCK_SIZE, dest);
    restore_irq_pri(basepri);

    return true;
}

bool spi_bdev_writeblock(const uint8_t *src, uint32_t block) {
    // we must disable USB irqs to prevent MSC contention with SPI flash
    uint32_t basepri = raise_irq_pri(IRQ_PRI_OTG_FS);
    int ret = mp_spiflash_write(&spiflash, block * FLASH_BLOCK_SIZE, FLASH_BLOCK_SIZE, src);
    if (spiflash.flags & 1) {
        led_state(PYB_LED_RED, 1); // indicate a dirty cache with LED on
        flash_tick_counter_last_write = HAL_GetTick();
    }
    restore_irq_pri(basepri);

    return ret == 0;
}

#endif // defined(MICROPY_HW_SPIFLASH_SIZE_BITS)
