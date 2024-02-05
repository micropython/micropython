/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Mike Teachman
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

// This file is never compiled standalone, it's included directly from
// extmod/machine_i2s.c via MICROPY_PY_MACHINE_I2S_INCLUDEFILE.

#include <stdlib.h>
#include <string.h>
#include "py/mphal.h"

#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"

// Notes on this port's specific implementation of I2S:
// - the DMA IRQ handler is used to implement the asynchronous background operations, for non-blocking mode
// - the PIO is used to drive the I2S bus signals
// - all sample data transfers use non-blocking DMA
// - the DMA controller is configured with 2 DMA channels in chained mode

#define MAX_I2S_RP2 (2)

// The DMA buffer size was empirically determined.  It is a tradeoff between:
// 1. memory use (smaller buffer size desirable to reduce memory footprint)
// 2. interrupt frequency (larger buffer size desirable to reduce interrupt frequency)
#define SIZEOF_DMA_BUFFER_IN_BYTES (256)
#define SIZEOF_HALF_DMA_BUFFER_IN_BYTES (SIZEOF_DMA_BUFFER_IN_BYTES / 2)
#define I2S_NUM_DMA_CHANNELS (2)

// For non-blocking mode, to avoid underflow/overflow, sample data is written/read to/from the ring buffer at a rate faster
// than the DMA transfer rate
#define NON_BLOCKING_RATE_MULTIPLIER (4)
#define SIZEOF_NON_BLOCKING_COPY_IN_BYTES (SIZEOF_HALF_DMA_BUFFER_IN_BYTES * NON_BLOCKING_RATE_MULTIPLIER)

#define SAMPLES_PER_FRAME (2)
#define PIO_INSTRUCTIONS_PER_BIT (2)

typedef enum {
    RX,
    TX
} i2s_mode_t;

typedef enum {
    GP_INPUT = 0,
    GP_OUTPUT = 1
} gpio_dir_t;

typedef struct _machine_i2s_obj_t {
    mp_obj_base_t base;
    uint8_t i2s_id;
    mp_hal_pin_obj_t sck;
    mp_hal_pin_obj_t ws;
    mp_hal_pin_obj_t sd;
    i2s_mode_t mode;
    int8_t bits;
    format_t format;
    int32_t rate;
    int32_t ibuf;
    mp_obj_t callback_for_non_blocking;
    io_mode_t io_mode;
    PIO pio;
    uint8_t sm;
    const pio_program_t *pio_program;
    uint prog_offset;
    int dma_channel[I2S_NUM_DMA_CHANNELS];
    uint8_t dma_buffer[SIZEOF_DMA_BUFFER_IN_BYTES];
    ring_buf_t ring_buffer;
    uint8_t *ring_buffer_storage;
    non_blocking_descriptor_t non_blocking_descriptor;
} machine_i2s_obj_t;

// The frame map is used with the readinto() method to transform the audio sample data coming
// from DMA memory (32-bit stereo) to the format specified
// in the I2S constructor.  e.g.  16-bit mono
STATIC const int8_t i2s_frame_map[NUM_I2S_USER_FORMATS][I2S_RX_FRAME_SIZE_IN_BYTES] = {
    {-1, -1,  0,  1, -1, -1, -1, -1 },  // Mono, 16-bits
    { 0,  1,  2,  3, -1, -1, -1, -1 },  // Mono, 32-bits
    {-1, -1,  0,  1, -1, -1,  2,  3 },  // Stereo, 16-bits
    { 0,  1,  2,  3,  4,  5,  6,  7 },  // Stereo, 32-bits
};

STATIC const PIO pio_instances[NUM_PIOS] = {pio0, pio1};

//  PIO program for 16-bit write
//    set(x, 14)                  .side(0b01)
//    label('left_channel')
//    out(pins, 1)                .side(0b00)
//    jmp(x_dec, "left_channel")  .side(0b01)
//    out(pins, 1)                .side(0b10)
//    set(x, 14)                  .side(0b11)
//    label('right_channel')
//    out(pins, 1)                .side(0b10)
//    jmp(x_dec, "right_channel") .side(0b11)
//    out(pins, 1)                .side(0b00)
STATIC const uint16_t pio_instructions_write_16[] = {59438, 24577, 2113, 28673, 63534, 28673, 6213, 24577};
STATIC const pio_program_t pio_write_16 = {
    pio_instructions_write_16,
    sizeof(pio_instructions_write_16) / sizeof(uint16_t),
    -1
};

//  PIO program for 32-bit write
//    set(x, 30)                  .side(0b01)
//    label('left_channel')
//    out(pins, 1)                .side(0b00)
//    jmp(x_dec, "left_channel")  .side(0b01)
//    out(pins, 1)                .side(0b10)
//    set(x, 30)                  .side(0b11)
//    label('right_channel')
//    out(pins, 1)                .side(0b10)
//    jmp(x_dec, "right_channel") .side(0b11)
//    out(pins, 1)                .side(0b00)
STATIC const uint16_t pio_instructions_write_32[] = {59454, 24577, 2113, 28673, 63550, 28673, 6213, 24577};
STATIC const pio_program_t pio_write_32 = {
    pio_instructions_write_32,
    sizeof(pio_instructions_write_32) / sizeof(uint16_t),
    -1
};

//  PIO program for 32-bit read
//    set(x, 30)                  .side(0b00)
//    label('left_channel')
//    in_(pins, 1)                .side(0b01)
//    jmp(x_dec, "left_channel")  .side(0b00)
//    in_(pins, 1)                .side(0b11)
//    set(x, 30)                  .side(0b10)
//    label('right_channel')
//    in_(pins, 1)                .side(0b11)
//    jmp(x_dec, "right_channel") .side(0b10)
//    in_(pins, 1)                .side(0b01)
STATIC const uint16_t pio_instructions_read_32[] = {57406, 18433, 65, 22529, 61502, 22529, 4165, 18433};
STATIC const pio_program_t pio_read_32 = {
    pio_instructions_read_32,
    sizeof(pio_instructions_read_32) / sizeof(uint16_t),
    -1
};

STATIC uint8_t dma_get_bits(i2s_mode_t mode, int8_t bits);
STATIC void dma_irq0_handler(void);
STATIC void dma_irq1_handler(void);
STATIC mp_obj_t machine_i2s_deinit(mp_obj_t self_in);

void machine_i2s_init0(void) {
    for (uint8_t i = 0; i < MAX_I2S_RP2; i++) {
        MP_STATE_PORT(machine_i2s_obj[i]) = NULL;
    }
}

STATIC int8_t get_frame_mapping_index(int8_t bits, format_t format) {
    if (format == MONO) {
        if (bits == 16) {
            return 0;
        } else { // 32 bits
            return 1;
        }
    } else { // STEREO
        if (bits == 16) {
            return 2;
        } else { // 32 bits
            return 3;
        }
    }
}

// function is used in IRQ context
STATIC void empty_dma(machine_i2s_obj_t *self, uint8_t *dma_buffer_p) {
    // when space exists, copy samples into ring buffer
    if (ringbuf_available_space(&self->ring_buffer) >= SIZEOF_HALF_DMA_BUFFER_IN_BYTES) {
        for (uint32_t i = 0; i < SIZEOF_HALF_DMA_BUFFER_IN_BYTES; i++) {
            ringbuf_push(&self->ring_buffer, dma_buffer_p[i]);
        }
    }
}

// function is used in IRQ context
STATIC void feed_dma(machine_i2s_obj_t *self, uint8_t *dma_buffer_p) {
    // when data exists, copy samples from ring buffer
    if (ringbuf_available_data(&self->ring_buffer) >= SIZEOF_HALF_DMA_BUFFER_IN_BYTES) {

        // copy a block of samples from the ring buffer to the dma buffer.
        // STM32 HAL API has a stereo I2S implementation, but not mono
        // mono format is implemented by duplicating each sample into both L and R channels.
        if ((self->format == MONO) && (self->bits == 16)) {
            for (uint32_t i = 0; i < SIZEOF_HALF_DMA_BUFFER_IN_BYTES / 4; i++) {
                for (uint8_t b = 0; b < sizeof(uint16_t); b++) {
                    ringbuf_pop(&self->ring_buffer, &dma_buffer_p[i * 4 + b]);
                    dma_buffer_p[i * 4 + b + 2] = dma_buffer_p[i * 4 + b]; // duplicated mono sample
                }
            }
        } else if ((self->format == MONO) && (self->bits == 32)) {
            for (uint32_t i = 0; i < SIZEOF_HALF_DMA_BUFFER_IN_BYTES / 8; i++) {
                for (uint8_t b = 0; b < sizeof(uint32_t); b++) {
                    ringbuf_pop(&self->ring_buffer, &dma_buffer_p[i * 8 + b]);
                    dma_buffer_p[i * 8 + b + 4] = dma_buffer_p[i * 8 + b]; // duplicated mono sample
                }
            }
        } else { // STEREO, both 16-bit and 32-bit
            for (uint32_t i = 0; i < SIZEOF_HALF_DMA_BUFFER_IN_BYTES; i++) {
                ringbuf_pop(&self->ring_buffer, &dma_buffer_p[i]);
            }
        }
    } else {
        // underflow.  clear buffer to transmit "silence" on the I2S bus
        memset(dma_buffer_p, 0, SIZEOF_HALF_DMA_BUFFER_IN_BYTES);
    }
}

STATIC void irq_configure(machine_i2s_obj_t *self) {
    if (self->i2s_id == 0) {
        irq_add_shared_handler(DMA_IRQ_0, dma_irq0_handler, PICO_SHARED_IRQ_HANDLER_DEFAULT_ORDER_PRIORITY);
        irq_set_enabled(DMA_IRQ_0, true);
    } else {
        irq_add_shared_handler(DMA_IRQ_1, dma_irq1_handler, PICO_SHARED_IRQ_HANDLER_DEFAULT_ORDER_PRIORITY);
        irq_set_enabled(DMA_IRQ_1, true);
    }
}

STATIC void irq_deinit(machine_i2s_obj_t *self) {
    if (self->i2s_id == 0) {
        irq_remove_handler(DMA_IRQ_0, dma_irq0_handler);
    } else {
        irq_remove_handler(DMA_IRQ_1, dma_irq1_handler);
    }
}

STATIC void pio_configure(machine_i2s_obj_t *self) {
    if (self->mode == TX) {
        if (self->bits == 16) {
            self->pio_program = &pio_write_16;
        } else {
            self->pio_program = &pio_write_32;
        }
    } else { // RX
        self->pio_program = &pio_read_32;
    }

    // find a PIO with a free state machine and adequate program space
    PIO candidate_pio;
    bool is_free_sm;
    bool can_add_program;
    for (uint8_t p = 0; p < NUM_PIOS; p++) {
        candidate_pio = pio_instances[p];
        is_free_sm = false;
        can_add_program = false;

        for (uint8_t sm = 0; sm < NUM_PIO_STATE_MACHINES; sm++) {
            if (!pio_sm_is_claimed(candidate_pio, sm)) {
                is_free_sm = true;
                break;
            }
        }

        if (pio_can_add_program(candidate_pio,  self->pio_program)) {
            can_add_program = true;
        }

        if (is_free_sm && can_add_program) {
            break;
        }
    }

    if (!is_free_sm) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("no free state machines"));
    }

    if (!can_add_program) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("not enough PIO program space"));
    }

    self->pio = candidate_pio;
    self->sm = pio_claim_unused_sm(self->pio, false);
    self->prog_offset = pio_add_program(self->pio, self->pio_program);
    pio_sm_init(self->pio, self->sm, self->prog_offset, NULL);

    pio_sm_config config = pio_get_default_sm_config();

    float pio_freq = self->rate *
        SAMPLES_PER_FRAME *
        dma_get_bits(self->mode, self->bits) *
        PIO_INSTRUCTIONS_PER_BIT;
    float clkdiv = clock_get_hz(clk_sys) / pio_freq;
    sm_config_set_clkdiv(&config, clkdiv);

    if (self->mode == TX) {
        sm_config_set_out_pins(&config, self->sd, 1);
        sm_config_set_out_shift(&config, false, true, dma_get_bits(self->mode, self->bits));
        sm_config_set_fifo_join(&config, PIO_FIFO_JOIN_TX);  // double TX FIFO size
    } else { // RX
        sm_config_set_in_pins(&config, self->sd);
        sm_config_set_in_shift(&config, false, true, dma_get_bits(self->mode, self->bits));
        sm_config_set_fifo_join(&config, PIO_FIFO_JOIN_RX);  // double RX FIFO size
    }

    sm_config_set_sideset(&config, 2, false, false);
    sm_config_set_sideset_pins(&config, self->sck);
    sm_config_set_wrap(&config, self->prog_offset, self->prog_offset + self->pio_program->length - 1);
    pio_sm_set_config(self->pio, self->sm, &config);
}

STATIC void pio_deinit(machine_i2s_obj_t *self) {
    if (self->pio) {
        pio_sm_set_enabled(self->pio, self->sm, false);
        pio_sm_unclaim(self->pio, self->sm);
        pio_remove_program(self->pio, self->pio_program, self->prog_offset);
    }
}

STATIC void gpio_init_i2s(PIO pio, uint8_t sm, mp_hal_pin_obj_t pin_num, uint8_t pin_val, gpio_dir_t pin_dir) {
    uint32_t pinmask = 1 << pin_num;
    pio_sm_set_pins_with_mask(pio, sm, pin_val << pin_num, pinmask);
    pio_sm_set_pindirs_with_mask(pio, sm, pin_dir << pin_num, pinmask);
    pio_gpio_init(pio, pin_num);
}

STATIC void gpio_configure(machine_i2s_obj_t *self) {
    gpio_init_i2s(self->pio, self->sm, self->sck, 0, GP_OUTPUT);
    gpio_init_i2s(self->pio, self->sm, self->ws, 0, GP_OUTPUT);
    if (self->mode == TX) {
        gpio_init_i2s(self->pio, self->sm, self->sd, 0, GP_OUTPUT);
    } else { // RX
        gpio_init_i2s(self->pio, self->sm, self->sd, 0, GP_INPUT);
    }
}

STATIC uint8_t dma_get_bits(i2s_mode_t mode, int8_t bits) {
    if (mode == TX) {
        return bits;
    } else { // RX
        // always read 32 bit words for I2S e.g.  I2S MEMS microphones
        return 32;
    }
}

// determine which DMA channel is associated to this IRQ
STATIC uint dma_map_irq_to_channel(uint irq_index) {
    for (uint ch = 0; ch < NUM_DMA_CHANNELS; ch++) {
        if ((dma_irqn_get_channel_status(irq_index, ch))) {
            return ch;
        }
    }
    // This should never happen
    return -1;
}

// note:  first DMA channel is mapped to the top half of buffer, second is mapped to the bottom half
STATIC uint8_t *dma_get_buffer(machine_i2s_obj_t *i2s_obj, uint channel) {
    for (uint8_t ch = 0; ch < I2S_NUM_DMA_CHANNELS; ch++) {
        if (i2s_obj->dma_channel[ch] == channel) {
            return i2s_obj->dma_buffer + (SIZEOF_HALF_DMA_BUFFER_IN_BYTES * ch);
        }
    }
    // This should never happen
    return NULL;
}

STATIC void dma_configure(machine_i2s_obj_t *self) {
    uint8_t num_free_dma_channels = 0;
    for (uint8_t ch = 0; ch < NUM_DMA_CHANNELS; ch++) {
        if (!dma_channel_is_claimed(ch)) {
            num_free_dma_channels++;
        }
    }
    if (num_free_dma_channels < I2S_NUM_DMA_CHANNELS) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("cannot claim 2 DMA channels"));
    }

    for (uint8_t ch = 0; ch < I2S_NUM_DMA_CHANNELS; ch++) {
        self->dma_channel[ch] = dma_claim_unused_channel(false);
    }

    // The DMA channels are chained together.  The first DMA channel is used to access
    // the top half of the DMA buffer.  The second DMA channel accesses the bottom half of the DMA buffer.
    // With chaining, when one DMA channel has completed a data transfer, the other
    // DMA channel automatically starts a new data transfer.
    enum dma_channel_transfer_size dma_size = (dma_get_bits(self->mode, self->bits) == 16) ? DMA_SIZE_16 : DMA_SIZE_32;
    for (uint8_t ch = 0; ch < I2S_NUM_DMA_CHANNELS; ch++) {
        dma_channel_config dma_config = dma_channel_get_default_config(self->dma_channel[ch]);
        channel_config_set_transfer_data_size(&dma_config, dma_size);
        channel_config_set_chain_to(&dma_config, self->dma_channel[(ch + 1) % I2S_NUM_DMA_CHANNELS]);

        uint8_t *dma_buffer = self->dma_buffer + (SIZEOF_HALF_DMA_BUFFER_IN_BYTES * ch);
        if (self->mode == TX) {
            channel_config_set_dreq(&dma_config, pio_get_dreq(self->pio, self->sm, true));
            channel_config_set_read_increment(&dma_config, true);
            channel_config_set_write_increment(&dma_config, false);
            dma_channel_configure(self->dma_channel[ch],
                &dma_config,
                (void *)&self->pio->txf[self->sm],                      // dest = PIO TX FIFO
                dma_buffer,                                             // src = DMA buffer
                SIZEOF_HALF_DMA_BUFFER_IN_BYTES / (dma_get_bits(self->mode, self->bits) / 8),
                false);
        } else { // RX
            channel_config_set_dreq(&dma_config, pio_get_dreq(self->pio, self->sm, false));
            channel_config_set_read_increment(&dma_config, false);
            channel_config_set_write_increment(&dma_config, true);
            dma_channel_configure(self->dma_channel[ch],
                &dma_config,
                dma_buffer,                                             // dest = DMA buffer
                (void *)&self->pio->rxf[self->sm],                      // src = PIO RX FIFO
                SIZEOF_HALF_DMA_BUFFER_IN_BYTES / (dma_get_bits(self->mode, self->bits) / 8),
                false);
        }
    }

    for (uint8_t ch = 0; ch < I2S_NUM_DMA_CHANNELS; ch++) {
        dma_irqn_acknowledge_channel(self->i2s_id, self->dma_channel[ch]);  // clear pending.  e.g. from SPI
        dma_irqn_set_channel_enabled(self->i2s_id, self->dma_channel[ch], true);
    }
}

STATIC void dma_deinit(machine_i2s_obj_t *self) {
    for (uint8_t ch = 0; ch < I2S_NUM_DMA_CHANNELS; ch++) {
        int channel = self->dma_channel[ch];

        // unchain the channel to prevent triggering a transfer in the chained-to channel
        dma_channel_config dma_config = dma_get_channel_config(channel);
        channel_config_set_chain_to(&dma_config, channel);
        dma_channel_set_config(channel, &dma_config, false);

        dma_irqn_set_channel_enabled(self->i2s_id, channel, false);
        dma_channel_abort(channel);  // in case a transfer is in flight
        dma_channel_unclaim(channel);
    }
}

STATIC void dma_irq_handler(uint8_t irq_index) {
    int dma_channel = dma_map_irq_to_channel(irq_index);
    if (dma_channel == -1) {
        // This should never happen
        return;
    }

    machine_i2s_obj_t *self = MP_STATE_PORT(machine_i2s_obj[irq_index]);
    if (self == NULL) {
        // This should never happen
        return;
    }

    uint8_t *dma_buffer = dma_get_buffer(self, dma_channel);
    if (dma_buffer == NULL) {
        // This should never happen
        return;
    }

    if (self->mode == TX) {
        // for non-blocking operation handle the write() method requests.
        if ((self->io_mode == NON_BLOCKING) && (self->non_blocking_descriptor.copy_in_progress)) {
            copy_appbuf_to_ringbuf_non_blocking(self);
        }

        feed_dma(self, dma_buffer);
        dma_irqn_acknowledge_channel(irq_index, dma_channel);
        dma_channel_set_read_addr(dma_channel, dma_buffer, false);
    } else { // RX
        empty_dma(self, dma_buffer);
        dma_irqn_acknowledge_channel(irq_index, dma_channel);
        dma_channel_set_write_addr(dma_channel, dma_buffer, false);

        // for non-blocking operation handle the readinto() method requests.
        if ((self->io_mode == NON_BLOCKING) && (self->non_blocking_descriptor.copy_in_progress)) {
            fill_appbuf_from_ringbuf_non_blocking(self);
        }
    }
}

STATIC void dma_irq0_handler(void) {
    dma_irq_handler(0);
}

STATIC void dma_irq1_handler(void) {
    dma_irq_handler(1);
}

STATIC void mp_machine_i2s_init_helper(machine_i2s_obj_t *self, mp_arg_val_t *args) {
    // are Pins valid?
    mp_hal_pin_obj_t sck = args[ARG_sck].u_obj == MP_OBJ_NULL ? -1 : mp_hal_get_pin_obj(args[ARG_sck].u_obj);
    mp_hal_pin_obj_t ws = args[ARG_ws].u_obj == MP_OBJ_NULL ? -1 : mp_hal_get_pin_obj(args[ARG_ws].u_obj);
    mp_hal_pin_obj_t sd = args[ARG_sd].u_obj == MP_OBJ_NULL ? -1 : mp_hal_get_pin_obj(args[ARG_sd].u_obj);

    // does WS pin follow SCK pin?
    // note:  SCK and WS are implemented as PIO sideset pins.  Sideset pins must be sequential.
    if (ws != (sck + 1)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid ws (must be sck+1)"));
    }

    // is Mode valid?
    i2s_mode_t i2s_mode = args[ARG_mode].u_int;
    if ((i2s_mode != RX) &&
        (i2s_mode != TX)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid mode"));
    }

    // is Bits valid?
    int8_t i2s_bits = args[ARG_bits].u_int;
    if ((i2s_bits != 16) &&
        (i2s_bits != 32)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid bits"));
    }

    // is Format valid?
    format_t i2s_format = args[ARG_format].u_int;
    if ((i2s_format != MONO) &&
        (i2s_format != STEREO)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid format"));
    }

    // is Rate valid?
    // Not checked

    // is Ibuf valid?
    int32_t ring_buffer_len = args[ARG_ibuf].u_int;
    if (ring_buffer_len > 0) {
        self->ring_buffer_storage = m_new(uint8_t, ring_buffer_len);
        ;
        ringbuf_init(&self->ring_buffer, self->ring_buffer_storage, ring_buffer_len);
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid ibuf"));
    }

    self->sck = sck;
    self->ws = ws;
    self->sd = sd;
    self->mode = i2s_mode;
    self->bits = i2s_bits;
    self->format = i2s_format;
    self->rate = args[ARG_rate].u_int;
    self->ibuf = ring_buffer_len;
    self->callback_for_non_blocking = MP_OBJ_NULL;
    self->non_blocking_descriptor.copy_in_progress = false;
    self->io_mode = BLOCKING;

    irq_configure(self);
    pio_configure(self);
    gpio_configure(self);
    dma_configure(self);

    pio_sm_set_enabled(self->pio, self->sm, true);
    dma_channel_start(self->dma_channel[0]);
}

STATIC machine_i2s_obj_t *mp_machine_i2s_make_new_instance(mp_int_t i2s_id) {
    if (i2s_id >= MAX_I2S_RP2) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid id"));
    }

    machine_i2s_obj_t *self;
    if (MP_STATE_PORT(machine_i2s_obj[i2s_id]) == NULL) {
        self = mp_obj_malloc(machine_i2s_obj_t, &machine_i2s_type);
        MP_STATE_PORT(machine_i2s_obj[i2s_id]) = self;
        self->i2s_id = i2s_id;
    } else {
        self = MP_STATE_PORT(machine_i2s_obj[i2s_id]);
        machine_i2s_deinit(MP_OBJ_FROM_PTR(self));
    }

    return self;
}

STATIC void mp_machine_i2s_deinit(machine_i2s_obj_t *self) {
    // use self->pio as in indication that I2S object has already been de-initialized
    if (self->pio != NULL) {
        pio_deinit(self);
        dma_deinit(self);
        irq_deinit(self);
        m_free(self->ring_buffer_storage);
        self->pio = NULL;  // flag object as de-initialized
    }
}

STATIC void mp_machine_i2s_irq_update(machine_i2s_obj_t *self) {
    (void)self;
}

MP_REGISTER_ROOT_POINTER(void *machine_i2s_obj[2]);
