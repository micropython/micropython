/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
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

#include "shared-bindings/busio/SPI.h"

#include "lib/utils/interrupt_char.h"
#include "py/mperrno.h"
#include "py/runtime.h"

#include "supervisor/board.h"
#include "common-hal/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/Pin.h"

#include "src/rp2_common/hardware_dma/include/hardware/dma.h"
#include "src/rp2_common/hardware_gpio/include/hardware/gpio.h"

#define NO_INSTANCE 0xff

STATIC bool never_reset_spi[2];
STATIC spi_inst_t *spi[2] = {spi0, spi1};

void reset_spi(void) {
    for (size_t i = 0; i < 2; i++) {
        if (never_reset_spi[i]) {
            continue;
        }

        spi_deinit(spi[i]);
    }
}

void common_hal_busio_spi_construct(busio_spi_obj_t *self,
    const mcu_pin_obj_t *clock, const mcu_pin_obj_t *mosi,
    const mcu_pin_obj_t *miso) {
    size_t instance_index = NO_INSTANCE;
    if (clock->number % 4 == 2) {
        instance_index = (clock->number / 8) % 2;
    }
    if (mosi != NULL) {
        // Make sure the set MOSI matches the clock settings.
        if (mosi->number % 4 != 3 ||
            (mosi->number / 8) % 2 != instance_index) {
            instance_index = NO_INSTANCE;
        }
    }
    if (miso != NULL) {
        // Make sure the set MOSI matches the clock settings.
        if (miso->number % 4 != 0 ||
            (miso->number / 8) % 2 != instance_index) {
            instance_index = NO_INSTANCE;
        }
    }

    // TODO: Check to see if we're sharing the SPI with a native APA102.

    if (instance_index > 1) {
        mp_raise_ValueError(translate("Invalid pins"));
    }

    if (instance_index == 0) {
        self->peripheral = spi0;
    } else if (instance_index == 1) {
        self->peripheral = spi1;
    }

    if ((spi_get_hw(self->peripheral)->cr1 & SPI_SSPCR1_SSE_BITS) != 0) {
        mp_raise_ValueError(translate("SPI peripheral in use"));
    }

    spi_init(self->peripheral, 250000);

    gpio_set_function(clock->number, GPIO_FUNC_SPI);
    claim_pin(clock);
    self->clock = clock;

    self->MOSI = mosi;
    if (mosi != NULL) {
        gpio_set_function(mosi->number, GPIO_FUNC_SPI);
        claim_pin(mosi);
    }

    self->MISO = miso;
    if (miso != NULL) {
        gpio_set_function(miso->number, GPIO_FUNC_SPI);
        claim_pin(miso);
    }
}

void common_hal_busio_spi_never_reset(busio_spi_obj_t *self) {
    never_reset_spi[spi_get_index(self->peripheral)] = true;

    common_hal_never_reset_pin(self->clock);
    common_hal_never_reset_pin(self->MOSI);
    common_hal_never_reset_pin(self->MISO);
}

bool common_hal_busio_spi_deinited(busio_spi_obj_t *self) {
    return self->clock == NULL;
}

void common_hal_busio_spi_deinit(busio_spi_obj_t *self) {
    if (common_hal_busio_spi_deinited(self)) {
        return;
    }
    never_reset_spi[spi_get_index(self->peripheral)] = false;
    spi_deinit(self->peripheral);

    common_hal_reset_pin(self->clock);
    common_hal_reset_pin(self->MOSI);
    common_hal_reset_pin(self->MISO);
    self->clock = NULL;
}

bool common_hal_busio_spi_configure(busio_spi_obj_t *self,
    uint32_t baudrate, uint8_t polarity, uint8_t phase, uint8_t bits) {
    if (baudrate == self->target_frequency &&
        polarity == self->polarity &&
        phase == self->phase &&
        bits == self->bits) {
        return true;
    }

    spi_set_format(self->peripheral, bits, polarity, phase, SPI_MSB_FIRST);

    self->polarity = polarity;
    self->phase = phase;
    self->bits = bits;
    self->target_frequency = baudrate;
    self->real_frequency = spi_set_baudrate(self->peripheral, baudrate);

    return true;
}

bool common_hal_busio_spi_try_lock(busio_spi_obj_t *self) {
    bool grabbed_lock = false;
    if (!self->has_lock) {
        grabbed_lock = true;
        self->has_lock = true;
    }
    return grabbed_lock;
}

bool common_hal_busio_spi_has_lock(busio_spi_obj_t *self) {
    return self->has_lock;
}

void common_hal_busio_spi_unlock(busio_spi_obj_t *self) {
    self->has_lock = false;
}

static bool _transfer(busio_spi_obj_t *self,
    const uint8_t *data_out, size_t out_len,
    uint8_t *data_in, size_t in_len) {
    // Use DMA for large transfers if channels are available
    const size_t dma_min_size_threshold = 32;
    int chan_tx = -1;
    int chan_rx = -1;
    size_t len = MAX(out_len, in_len);
    if (len >= dma_min_size_threshold) {
        // Use two DMA channels to service the two FIFOs
        chan_tx = dma_claim_unused_channel(false);
        chan_rx = dma_claim_unused_channel(false);
    }
    bool use_dma = chan_rx >= 0 && chan_tx >= 0;
    if (use_dma) {
        dma_channel_config c = dma_channel_get_default_config(chan_tx);
        channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
        channel_config_set_dreq(&c, spi_get_index(self->peripheral) ? DREQ_SPI1_TX : DREQ_SPI0_TX);
        channel_config_set_read_increment(&c, out_len == len);
        channel_config_set_write_increment(&c, false);
        dma_channel_configure(chan_tx, &c,
            &spi_get_hw(self->peripheral)->dr,
            data_out,
            len,
            false);

        c = dma_channel_get_default_config(chan_rx);
        channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
        channel_config_set_dreq(&c, spi_get_index(self->peripheral) ? DREQ_SPI1_RX : DREQ_SPI0_RX);
        channel_config_set_read_increment(&c, false);
        channel_config_set_write_increment(&c, in_len == len);
        dma_channel_configure(chan_rx, &c,
            data_in,
            &spi_get_hw(self->peripheral)->dr,
            len,
            false);

        dma_start_channel_mask((1u << chan_rx) | (1u << chan_tx));
        while (dma_channel_is_busy(chan_rx) || dma_channel_is_busy(chan_tx)) {
            // TODO: We should idle here until we get a DMA interrupt or something else.
            RUN_BACKGROUND_TASKS;
        }
    }

    // If we have claimed only one channel successfully, we should release immediately. This also
    // releases the DMA after use_dma has been done.
    if (chan_rx >= 0) {
        dma_channel_unclaim(chan_rx);
    }
    if (chan_tx >= 0) {
        dma_channel_unclaim(chan_tx);
    }

    if (!use_dma) {
        // Use software for small transfers, or if couldn't claim two DMA channels
        // Never have more transfers in flight than will fit into the RX FIFO,
        // else FIFO will overflow if this code is heavily interrupted.
        const size_t fifo_depth = 8;
        size_t rx_remaining = len;
        size_t tx_remaining = len;

        while (rx_remaining || tx_remaining) {
            if (tx_remaining && spi_is_writable(self->peripheral) && rx_remaining - tx_remaining < fifo_depth) {
                spi_get_hw(self->peripheral)->dr = (uint32_t)*data_out;
                // Increment only if the buffer is the transfer length. It's 1 otherwise.
                if (out_len == len) {
                    data_out++;
                }
                --tx_remaining;
            }
            if (rx_remaining && spi_is_readable(self->peripheral)) {
                *data_in = (uint8_t)spi_get_hw(self->peripheral)->dr;
                // Increment only if the buffer is the transfer length. It's 1 otherwise.
                if (in_len == len) {
                    data_in++;
                }
                --rx_remaining;
            }
            RUN_BACKGROUND_TASKS;
        }
    }
    return true;
}

bool common_hal_busio_spi_write(busio_spi_obj_t *self,
    const uint8_t *data, size_t len) {
    uint32_t data_in;
    return _transfer(self, data, len, (uint8_t *)&data_in, MIN(len, 4));
}

bool common_hal_busio_spi_read(busio_spi_obj_t *self,
    uint8_t *data, size_t len, uint8_t write_value) {
    uint32_t data_out = write_value << 24 | write_value << 16 | write_value << 8 | write_value;
    return _transfer(self, (const uint8_t *)&data_out, MIN(4, len), data, len);
}

bool common_hal_busio_spi_transfer(busio_spi_obj_t *self, const uint8_t *data_out, uint8_t *data_in, size_t len) {
    return _transfer(self, data_out, len, data_in, len);
}

uint32_t common_hal_busio_spi_get_frequency(busio_spi_obj_t *self) {
    return self->real_frequency;
}

uint8_t common_hal_busio_spi_get_phase(busio_spi_obj_t *self) {
    return self->phase;
}

uint8_t common_hal_busio_spi_get_polarity(busio_spi_obj_t *self) {
    return self->polarity;
}
