// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/async_spi/SPI.h"

#include "py/runtime.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "supervisor/background_callback.h"

#include "hardware/dma.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"

#define NO_INSTANCE 0xff

// Done flag of the async transfer on each kept RX channel, set when that channel finishes.
static circuitpy_async_flag_t *async_flags[NUM_DMA_CHANNELS];
// RX channels with the interrupt enabled.
static uint32_t async_irq_mask;

// Shared DMA_IRQ_0 handler: acknowledges and services only our channels, like audio_dma and rp2pio.
static void __not_in_flash_func(spi_dma_irq_handler)(void) {
    uint32_t pending = dma_hw->ints0 & async_irq_mask;
    dma_hw->ints0 = pending;
    for (uint chan = 0; pending != 0; chan++, pending >>= 1) {
        if ((pending & 1) && async_flags[chan] != NULL) {
            CIRCUITPY_ASYNC_FLAG_SET(async_flags[chan]);
        }
    }
}

static void async_irq_enable(uint chan) {
    dma_hw->ints0 = 1u << chan;
    if (async_irq_mask == 0) {
        irq_add_shared_handler(DMA_IRQ_0, spi_dma_irq_handler, PICO_SHARED_IRQ_HANDLER_DEFAULT_ORDER_PRIORITY);
    }
    async_irq_mask |= 1u << chan;
    dma_irqn_set_channel_enabled(0, chan, true);
    irq_set_enabled(DMA_IRQ_0, true);
}

static void async_irq_disable(uint chan) {
    dma_irqn_set_channel_enabled(0, chan, false);
    async_irq_mask &= ~(1u << chan);
    if (async_irq_mask == 0) {
        irq_remove_handler(DMA_IRQ_0, spi_dma_irq_handler);
    }
    if (dma_hw->inte0 == 0) {
        irq_set_enabled(DMA_IRQ_0, false);
    }
}

void common_hal_async_spi_spi_construct(async_spi_spi_obj_t *self, const mcu_pin_obj_t *clock,
    const mcu_pin_obj_t *mosi, const mcu_pin_obj_t *miso) {
    size_t instance_index = NO_INSTANCE;

    // Ensure the object starts in its deinit state.
    self->clock = NULL;

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
        raise_ValueError_invalid_pins();
    }

    if (instance_index == 0) {
        self->peripheral = spi0;
    } else if (instance_index == 1) {
        self->peripheral = spi1;
    }

    if ((spi_get_hw(self->peripheral)->cr1 & SPI_SSPCR1_SSE_BITS) != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("SPI peripheral in use"));
    }

    circuitpy_async_lock_init(&self->lock);
    self->async_active = false;
    self->dma_kept = false;
    self->target_frequency = 250000;
    self->real_frequency = spi_init(self->peripheral, self->target_frequency);   // mode 0, 8 bits
    self->polarity = 0;
    self->phase = 0;
    self->bits = 8;

    gpio_set_function(clock->number, GPIO_FUNC_SPI);
    claim_pin(clock);
    self->clock = clock;

    self->mosi = mosi;
    if (mosi != NULL) {
        gpio_set_function(mosi->number, GPIO_FUNC_SPI);
        claim_pin(mosi);
    }

    self->miso = miso;
    if (miso != NULL) {
        gpio_set_function(miso->number, GPIO_FUNC_SPI);
        claim_pin(miso);
    }
}

bool common_hal_async_spi_spi_deinited(async_spi_spi_obj_t *self) {
    return self->clock == NULL;
}

void common_hal_async_spi_spi_never_reset(async_spi_spi_obj_t *self) {
    common_hal_never_reset_pin(self->clock);
    common_hal_never_reset_pin(self->mosi);
    common_hal_never_reset_pin(self->miso);
}

void common_hal_async_spi_spi_mark_deinit(async_spi_spi_obj_t *self) {
    self->clock = NULL;
}

static void _abort(async_spi_spi_obj_t *self);

void common_hal_async_spi_spi_deinit(async_spi_spi_obj_t *self) {
    if (common_hal_async_spi_spi_deinited(self)) {
        return;
    }
    circuitpy_async_lock_deinit(&self->lock);
    if (self->async_active) {
        _abort(self);
        CIRCUITPY_ASYNC_FLAG_SET(self->async_done);
    }
    if (self->dma_kept) {
        async_irq_disable(self->dma_rx);
        dma_channel_unclaim(self->dma_tx);
        dma_channel_unclaim(self->dma_rx);
        self->dma_kept = false;
    }
    spi_deinit(self->peripheral);

    common_hal_reset_pin(self->clock);
    common_hal_reset_pin(self->mosi);
    common_hal_reset_pin(self->miso);

    self->clock = NULL;
}

static void _apply(async_spi_spi_obj_t *self, uint32_t baudrate, uint8_t polarity, uint8_t phase,
    uint8_t bits) {
    if (baudrate == self->target_frequency &&
        polarity == self->polarity &&
        phase == self->phase &&
        bits == self->bits) {
        return;
    }

    spi_set_format(self->peripheral, bits, polarity, phase, SPI_MSB_FIRST);

    // Workaround to start with clock line high if polarity=1. The hw SPI peripheral does not do this
    // automatically. See https://github.com/raspberrypi/pico-sdk/issues/868 and
    // https://forums.raspberrypi.com/viewtopic.php?t=336142
    // TODO: scheduled to be be fixed in pico-sdk 1.5.0.
    if (polarity) {
        hw_clear_bits(&spi_get_hw(self->peripheral)->cr1, SPI_SSPCR1_SSE_BITS); // disable the SPI
        hw_set_bits(&spi_get_hw(self->peripheral)->cr1, SPI_SSPCR1_SSE_BITS); // re-enable the SPI
    }

    self->polarity = polarity;
    self->phase = phase;
    self->bits = bits;
    self->target_frequency = baudrate;
    self->real_frequency = spi_set_baudrate(self->peripheral, baudrate);
}

// The caller holds the lock, so no transfer is running.
void common_hal_async_spi_spi_configure(async_spi_spi_obj_t *self, uint32_t baudrate,
    uint8_t polarity, uint8_t phase, uint8_t bits) {
    _apply(self, baudrate, polarity, phase, bits);
}

circuitpy_async_lock_obj_t *common_hal_async_spi_spi_get_lock(async_spi_spi_obj_t *self) {
    return &self->lock;
}

uint8_t common_hal_async_spi_spi_get_polarity(async_spi_spi_obj_t *self) {
    return self->polarity;
}

uint8_t common_hal_async_spi_spi_get_phase(async_spi_spi_obj_t *self) {
    return self->phase;
}

uint32_t common_hal_async_spi_spi_get_frequency(async_spi_spi_obj_t *self) {
    return self->real_frequency;
}

// Start a transfer. With DMA it runs in the background and done is set from the interrupt;
// otherwise it is done in software before this returns. An out or in buffer shorter than the
// transfer is one byte repeated or dropped. The DMA channels are kept until deinit, for buses
// that send often. Returns whether DMA is running.
static bool _start(async_spi_spi_obj_t *self,
    const uint8_t *data_out, size_t out_len,
    uint8_t *data_in, size_t in_len, circuitpy_async_flag_t *done) {
    size_t len = MAX(out_len, in_len);
    // Only use DMA if both data buffers are in SRAM. Otherwise, we'll stall the DMA with PSRAM or flash cache misses.
    bool use_dma = len >= 32 && data_in >= (uint8_t *)SRAM_BASE && data_out >= (uint8_t *)SRAM_BASE;
    if (use_dma && !self->dma_kept) {
        int chan_tx = dma_claim_unused_channel(false);
        int chan_rx = dma_claim_unused_channel(false);
        if (chan_tx >= 0 && chan_rx >= 0) {
            self->dma_tx = chan_tx;
            self->dma_rx = chan_rx;
            self->dma_kept = true;
            async_irq_enable(chan_rx);
        } else {
            // If we have claimed only one channel successfully, release it.
            if (chan_tx >= 0) {
                dma_channel_unclaim(chan_tx);
            }
            if (chan_rx >= 0) {
                dma_channel_unclaim(chan_rx);
            }
            use_dma = false;
        }
    }
    if (use_dma) {
        dma_channel_config c = dma_channel_get_default_config(self->dma_tx);
        channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
        channel_config_set_dreq(&c, spi_get_index(self->peripheral) ? DREQ_SPI1_TX : DREQ_SPI0_TX);
        channel_config_set_read_increment(&c, out_len == len);
        channel_config_set_write_increment(&c, false);
        dma_channel_configure(self->dma_tx, &c,
            &spi_get_hw(self->peripheral)->dr,
            data_out,
            len,
            false);

        c = dma_channel_get_default_config(self->dma_rx);
        channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
        channel_config_set_dreq(&c, spi_get_index(self->peripheral) ? DREQ_SPI1_RX : DREQ_SPI0_RX);
        channel_config_set_read_increment(&c, false);
        channel_config_set_write_increment(&c, in_len == len);
        dma_channel_configure(self->dma_rx, &c,
            data_in,
            &spi_get_hw(self->peripheral)->dr,
            len,
            false);

        // Before the start, so that a short transfer cannot finish unseen.
        async_flags[self->dma_rx] = done;
        dma_start_channel_mask((1u << self->dma_rx) | (1u << self->dma_tx));
        return true;
    }

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
    return false;
}

// Waits for the running async transfer, if any, and sets its done flag. No background tasks:
// the caller holds the bus, and one of them may want it.
static void _end(async_spi_spi_obj_t *self) {
    if (!self->async_active) {
        return;
    }
    // The RX channel finishes last.
    while (dma_channel_is_busy(self->dma_rx)) {
    }
    self->async_active = false;
    async_flags[self->dma_rx] = NULL;
    CIRCUITPY_ASYNC_FLAG_SET(self->async_done);
}

// Start an async transfer after the previous one; done is set when it has finished, here if
// it ran in software. The buffers must stay valid until then.
static __attribute__((noinline)) void _async_start(async_spi_spi_obj_t *self, const uint8_t *data_out,
    size_t out_len, uint8_t *data_in, size_t in_len, circuitpy_async_flag_t *done) {
    _end(self);
    CIRCUITPY_ASYNC_FLAG_INIT(done);
    self->async_done = done;
    // An empty data side means there is nothing to send, whatever the one-byte side holds.
    self->async_active = out_len != 0 && in_len != 0 &&
        _start(self, data_out, out_len, data_in, in_len, done);
    if (!self->async_active) {
        CIRCUITPY_ASYNC_FLAG_SET(done);
    }
}

void *common_hal_async_spi_spi_write_start(async_spi_spi_obj_t *self, const uint8_t *data,
    size_t len, circuitpy_async_flag_t *done) {
    _async_start(self, data, len, &self->one_byte, 1, done);
    return NULL;
}

void *common_hal_async_spi_spi_readinto_start(async_spi_spi_obj_t *self, uint8_t *data,
    size_t len, uint8_t write_value, circuitpy_async_flag_t *done) {
    self->one_byte = write_value;
    _async_start(self, &self->one_byte, 1, data, len, done);
    return NULL;
}

void *common_hal_async_spi_spi_write_readinto_start(async_spi_spi_obj_t *self,
    const uint8_t *data_out, uint8_t *data_in, size_t len, circuitpy_async_flag_t *done) {
    _async_start(self, data_out, len, data_in, len, done);
    return NULL;
}

// Stop the running DMA transfer without setting its done flag.
static void _abort(async_spi_spi_obj_t *self) {
    // A completion interrupt may still come (RP2040-E13); the handler skips a NULL flag.
    async_flags[self->dma_rx] = NULL;
    #if PICO_RP2350
    // RP2350-E5: an enabled channel can retrigger after the abort.
    hw_clear_bits(&dma_hw->ch[self->dma_tx].al1_ctrl, DMA_CH0_CTRL_TRIG_EN_BITS);
    hw_clear_bits(&dma_hw->ch[self->dma_rx].al1_ctrl, DMA_CH0_CTRL_TRIG_EN_BITS);
    #endif
    // Both channels at once; as dma_channel_abort(), the busy bits clear once in-flight
    // transfers have retired.
    dma_hw->abort = (1u << self->dma_tx) | (1u << self->dma_rx);
    while ((dma_hw->ch[self->dma_tx].ctrl_trig | dma_hw->ch[self->dma_rx].ctrl_trig) & DMA_CH0_CTRL_TRIG_BUSY_BITS) {
    }
    self->async_active = false;
    while (spi_is_busy(self->peripheral)) {
    }
    while (spi_is_readable(self->peripheral)) {
        (void)spi_get_hw(self->peripheral)->dr;
    }
    spi_get_hw(self->peripheral)->icr = SPI_SSPICR_RORIC_BITS;
}

// Stops the transfer started with done, or finishes it if done is set. Does nothing for an
// earlier transfer, and does not allocate.
static void _stop(async_spi_spi_obj_t *self, circuitpy_async_flag_t *done) {
    if (!self->async_active || self->async_done != done) {
        return;
    }
    if (CIRCUITPY_ASYNC_FLAG_IS_SET(done)) {
        _end(self);
        return;
    }
    _abort(self);
}

bool common_hal_async_spi_spi_transfer_finish(async_spi_spi_obj_t *self, void *context,
    circuitpy_async_flag_t *done) {
    _stop(self, done);
    return true;
}

mp_obj_t common_hal_async_spi_spi_transfer_end(async_spi_spi_obj_t *self, void *context,
    circuitpy_async_flag_t *done) {
    _stop(self, done);
    return mp_const_none;
}

void common_hal_async_spi_spi_transfer_cancel(async_spi_spi_obj_t *self, void *context,
    circuitpy_async_flag_t *done) {
    _stop(self, done);
}
