/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Mike Teachman
 * Copyright (c) 2022 Robert Hammelrath
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
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/misc.h"
#include "py/stream.h"
#include "py/objstr.h"
#include "modmachine.h"
#include "dma_manager.h"

#include CLOCK_CONFIG_H
#include "fsl_iomuxc.h"
#include "fsl_dmamux.h"
#include "fsl_edma.h"
#include "fsl_sai.h"

#if MICROPY_PY_MACHINE_I2S
// The I2S module has 3 modes of operation:
//
// Mode1:  Blocking
// - readinto() and write() methods block until the supplied buffer is filled (read) or emptied (write)
// - this is the default mode of operation
//
// Mode2:  Non-Blocking
// - readinto() and write() methods return immediately
// - buffer filling and emptying happens asynchronously to the main MicroPython task
// - a callback function is called when the supplied buffer has been filled (read) or emptied (write)
// - non-blocking mode is enabled when a callback is set with the irq() method
// - the DMA callback is used to implement the asynchronous background operations
//
// Mode3: Uasyncio
// - implements the stream protocol
// - uasyncio mode is enabled when the ioctl() function is called
// - the state of the internal ring buffer is used to detect that I2S samples can be read or written
//
// The samples contained in the app buffer supplied for the readinto() and write() methods have the following convention:
//   Mono:  little endian format
//   Stereo:  little endian format, left channel first
//
// I2S terms:
//   "frame":  consists of two audio samples (Left audio sample + Right audio sample)
//
// Misc:
// - for Mono configuration:
//   - readinto method: samples are gathered from the L channel only
//   - write method: every sample is output to both the L and R channels
// - for readinto method the I2S hardware is read using 8-byte frames
//   (this is standard for almost all I2S hardware, such as MEMS microphones)
// - all 3 Modes of operation are implemented using the peripheral drivers in the NXP MCUXpresso SDK
// - all sample data transfers use DMA
// - the DMA ping-pong buffer needs to be aligned to a cache line size of 32 bytes.  32 byte
//   alignment is needed to use the routines that clean and invalidate D-Cache which work on a
//   32 byte address boundary.
// - master clock frequency is sampling frequency * 256

// DMA ping-pong buffer size was empirically determined.  It is a tradeoff between:
// 1. memory use (smaller buffer size desirable to reduce memory footprint)
// 2. interrupt frequency (larger buffer size desirable to reduce interrupt frequency)
// The sizeof 1/2 of the DMA buffer must be evenly divisible by the cache line size of 32 bytes.
#define SIZEOF_DMA_BUFFER_IN_BYTES (256)
#define SIZEOF_HALF_DMA_BUFFER_IN_BYTES (SIZEOF_DMA_BUFFER_IN_BYTES / 2)

// For non-blocking mode, to avoid underflow/overflow, sample data is written/read to/from the ring buffer at a rate faster
// than the DMA transfer rate
#define NON_BLOCKING_RATE_MULTIPLIER (4)
#define SIZEOF_NON_BLOCKING_COPY_IN_BYTES (SIZEOF_HALF_DMA_BUFFER_IN_BYTES * NON_BLOCKING_RATE_MULTIPLIER)

#define NUM_I2S_USER_FORMATS (4)
#define I2S_RX_FRAME_SIZE_IN_BYTES (8)
#define SAI_CHANNEL_0 (0)
#define SAI_NUM_AUDIO_CHANNELS (2U)

typedef enum {
    SCK,
    WS,
    SD,
    MCK
} i2s_pin_function_t;

typedef enum {
    RX,
    TX,
} i2s_mode_t;

typedef enum {
    MONO,
    STEREO
} format_t;

typedef enum {
    BLOCKING,
    NON_BLOCKING,
    UASYNCIO
} io_mode_t;

typedef enum {
    TOP_HALF,
    BOTTOM_HALF
} ping_pong_t;

typedef struct _ring_buf_t {
    uint8_t *buffer;
    size_t head;
    size_t tail;
    size_t size;
} ring_buf_t;

typedef struct _non_blocking_descriptor_t {
    mp_buffer_info_t appbuf;
    uint32_t index;
    bool copy_in_progress;
} non_blocking_descriptor_t;

typedef struct _machine_i2s_obj_t {
    mp_obj_base_t base;
    uint8_t i2s_id;
    mp_hal_pin_obj_t sck;
    mp_hal_pin_obj_t ws;
    mp_hal_pin_obj_t sd;
    mp_hal_pin_obj_t mck;
    i2s_mode_t mode;
    int8_t bits;
    format_t format;
    int32_t rate;
    int32_t ibuf;
    mp_obj_t callback_for_non_blocking;
    uint8_t dma_buffer[SIZEOF_DMA_BUFFER_IN_BYTES + 0x1f]; // 0x1f related to D-Cache alignment
    uint8_t *dma_buffer_dcache_aligned;
    ring_buf_t ring_buffer;
    uint8_t *ring_buffer_storage;
    non_blocking_descriptor_t non_blocking_descriptor;
    io_mode_t io_mode;
    I2S_Type *i2s_inst;
    int dma_channel;
    edma_handle_t edmaHandle;
    edma_tcd_t *edmaTcd;
} machine_i2s_obj_t;

typedef struct _iomux_table_t {
    uint32_t muxRegister;
    uint32_t muxMode;
    uint32_t inputRegister;
    uint32_t inputDaisy;
    uint32_t configRegister;
} iomux_table_t;

typedef struct _gpio_map_t {
    uint8_t hw_id;
    i2s_pin_function_t fn;
    i2s_mode_t mode;
    qstr name;
    iomux_table_t iomux;
} gpio_map_t;

typedef struct _i2s_clock_config_t {
    sai_sample_rate_t rate;
    const clock_audio_pll_config_t *pll_config;
    uint32_t clock_pre_divider;
    uint32_t clock_divider;
} i2s_clock_config_t;

STATIC mp_obj_t machine_i2s_deinit(mp_obj_t self_in);

// The frame map is used with the readinto() method to transform the audio sample data coming
// from DMA memory (32-bit stereo) to the format specified
// in the I2S constructor.  e.g.  16-bit mono
STATIC const int8_t i2s_frame_map[NUM_I2S_USER_FORMATS][I2S_RX_FRAME_SIZE_IN_BYTES] = {
    {-1, -1,  0,  1, -1, -1, -1, -1 },  // Mono, 16-bits
    { 0,  1,  2,  3, -1, -1, -1, -1 },  // Mono, 32-bits
    {-1, -1,  0,  1, -1, -1,  2,  3 },  // Stereo, 16-bits
    { 0,  1,  2,  3,  4,  5,  6,  7 },  // Stereo, 32-bits
};

// 2 PLL configurations
// PLL output frequency = 24MHz * (.loopDivider + .numerator/.denominator)

// Configuration 1: for sampling frequencies [Hz]:  8000, 12000, 16000, 24000, 32000, 48000
// Clock frequency = 786,432,000 Hz = 48000 * 64 * 256
STATIC const clock_audio_pll_config_t audioPllConfig_8000_48000 = {
    .loopDivider = 32,          // PLL loop divider. Valid range for DIV_SELECT divider value: 27~54
    .postDivider = 1,           // Divider after the PLL, should only be 1, 2, 4, 8, 16
    .numerator = 76800,         // 30 bit numerator of fractional loop divider
    .denominator = 100000,      // 30 bit denominator of fractional loop divider
    #if !defined(MIMXRT117x_SERIES)
    .src = kCLOCK_PllClkSrc24M  // Pll clock source
    #endif
};

// Configuration 2: for sampling frequencies [Hz]:  11025, 22050, 44100
// Clock frequency = 722,534,400 = 44100 * 64 * 256
STATIC const clock_audio_pll_config_t audioPllConfig_11025_44100 = {
    .loopDivider = 30,          // PLL loop divider. Valid range for DIV_SELECT divider value: 27~54
    .postDivider = 1,           // Divider after the PLL, should only be 1, 2, 4, 8, 16
    .numerator = 10560,         // 30 bit numerator of fractional loop divider
    .denominator = 100000,      // 30 bit denominator of fractional loop divider
    #if !defined(MIMXRT117x_SERIES)
    .src = kCLOCK_PllClkSrc24M  // Pll clock source
    #endif
};

#if defined(MIMXRT117x_SERIES)
// for 1176 the pre_div value is used for post_div of the Audio PLL,
// which is 2**n: 0->1, 1->2, 2->4, 3->8, 4->16, 5->32
// The divider is 8 bit and must be given as n (not n-1)
// So the total division factor is given by (2**p) * d
STATIC const i2s_clock_config_t clock_config_map[] = {
    {kSAI_SampleRate8KHz, &audioPllConfig_8000_48000, 1, 192},     // 384
    {kSAI_SampleRate11025Hz, &audioPllConfig_11025_44100, 1, 128}, // 256
    {kSAI_SampleRate12KHz, &audioPllConfig_8000_48000, 1, 128},    // 256
    {kSAI_SampleRate16KHz, &audioPllConfig_8000_48000, 0, 192},    // 192
    {kSAI_SampleRate22050Hz, &audioPllConfig_11025_44100, 0, 128}, // 128
    {kSAI_SampleRate24KHz, &audioPllConfig_8000_48000, 0, 128},    // 128
    {kSAI_SampleRate32KHz, &audioPllConfig_8000_48000, 0, 96},     // 96
    {kSAI_SampleRate44100Hz, &audioPllConfig_11025_44100, 0, 64},  // 64
    {kSAI_SampleRate48KHz, &audioPllConfig_8000_48000, 0, 64}      // 64
};

STATIC const clock_root_t i2s_clock_mux[] = I2S_CLOCK_MUX;
#else
// for 10xx the total division factor is given by (p + 1) * (d + 1)
STATIC const i2s_clock_config_t clock_config_map[] = {
    {kSAI_SampleRate8KHz, &audioPllConfig_8000_48000, 5, 63},      // 384
    {kSAI_SampleRate11025Hz, &audioPllConfig_11025_44100, 3, 63},  // 256
    {kSAI_SampleRate12KHz, &audioPllConfig_8000_48000, 3, 63},     // 256
    {kSAI_SampleRate16KHz, &audioPllConfig_8000_48000, 2, 63},     // 192
    {kSAI_SampleRate22050Hz, &audioPllConfig_11025_44100, 1, 63},  // 128
    {kSAI_SampleRate24KHz, &audioPllConfig_8000_48000, 1, 63},     // 128
    {kSAI_SampleRate32KHz, &audioPllConfig_8000_48000, 1, 47},     // 96
    {kSAI_SampleRate44100Hz, &audioPllConfig_11025_44100, 0, 63},  // 64
    {kSAI_SampleRate48KHz, &audioPllConfig_8000_48000, 0, 63}      // 64
};

STATIC const clock_mux_t i2s_clock_mux[] = I2S_CLOCK_MUX;
STATIC const clock_div_t i2s_clock_pre_div[] = I2S_CLOCK_PRE_DIV;
STATIC const clock_div_t i2s_clock_div[] = I2S_CLOCK_DIV;
STATIC const iomuxc_gpr_mode_t i2s_iomuxc_gpr_mode[] = I2S_IOMUXC_GPR_MODE;
#endif

STATIC const I2S_Type *i2s_base_ptr[] = I2S_BASE_PTRS;

STATIC const dma_request_source_t i2s_dma_req_src_tx[] = I2S_DMA_REQ_SRC_TX;
STATIC const dma_request_source_t i2s_dma_req_src_rx[] = I2S_DMA_REQ_SRC_RX;
STATIC const gpio_map_t i2s_gpio_map[] = I2S_GPIO_MAP;
AT_NONCACHEABLE_SECTION_ALIGN(STATIC edma_tcd_t edmaTcd[MICROPY_HW_I2S_NUM], 32);

// called on processor reset
void machine_i2s_init0() {
    for (uint8_t i = 0; i < MICROPY_HW_I2S_NUM; i++) {
        MP_STATE_PORT(machine_i2s_obj)[i] = NULL;
    }
}

// called on soft reboot
void machine_i2s_deinit_all(void) {
    for (uint8_t i = 0; i < MICROPY_HW_I2S_NUM; i++) {
        machine_i2s_obj_t *i2s_obj = MP_STATE_PORT(machine_i2s_obj)[i];
        if (i2s_obj != NULL) {
            machine_i2s_deinit(i2s_obj);
            MP_STATE_PORT(machine_i2s_obj)[i] = NULL;
        }
    }
}

// Ring Buffer
// Thread safe when used with these constraints:
// - Single Producer, Single Consumer
// - Sequential atomic operations
// One byte of capacity is used to detect buffer empty/full

STATIC void ringbuf_init(ring_buf_t *rbuf, uint8_t *buffer, size_t size) {
    rbuf->buffer = buffer;
    rbuf->size = size;
    rbuf->head = 0;
    rbuf->tail = 0;
}

STATIC bool ringbuf_push(ring_buf_t *rbuf, uint8_t data) {
    size_t next_tail = (rbuf->tail + 1) % rbuf->size;

    if (next_tail != rbuf->head) {
        rbuf->buffer[rbuf->tail] = data;
        rbuf->tail = next_tail;
        return true;
    }

    // full
    return false;
}

STATIC bool ringbuf_pop(ring_buf_t *rbuf, uint8_t *data) {
    if (rbuf->head == rbuf->tail) {
        // empty
        return false;
    }

    *data = rbuf->buffer[rbuf->head];
    rbuf->head = (rbuf->head + 1) % rbuf->size;
    return true;
}

STATIC bool ringbuf_is_empty(ring_buf_t *rbuf) {
    return rbuf->head == rbuf->tail;
}

STATIC bool ringbuf_is_full(ring_buf_t *rbuf) {
    return ((rbuf->tail + 1) % rbuf->size) == rbuf->head;
}

STATIC size_t ringbuf_available_data(ring_buf_t *rbuf) {
    return (rbuf->tail - rbuf->head + rbuf->size) % rbuf->size;
}

STATIC size_t ringbuf_available_space(ring_buf_t *rbuf) {
    return rbuf->size - ringbuf_available_data(rbuf) - 1;
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

STATIC int8_t get_dma_bits(uint16_t mode, int8_t bits) {
    if (mode == TX) {
        if (bits == 16) {
            return 16;
        } else {
            return 32;
        }
        return bits;
    } else { // RX
        // always read 32 bit words for I2S e.g.  I2S MEMS microphones
        return 32;
    }
}

STATIC bool lookup_gpio(const machine_pin_obj_t *pin, i2s_pin_function_t fn, uint8_t hw_id, uint16_t *index) {
    for (uint16_t i = 0; i < ARRAY_SIZE(i2s_gpio_map); i++) {
        if ((pin->name == i2s_gpio_map[i].name) &&
            (i2s_gpio_map[i].fn == fn) &&
            (i2s_gpio_map[i].hw_id == hw_id)) {
            *index = i;
            return true;
        }
    }
    return false;
}

STATIC bool set_iomux(const machine_pin_obj_t *pin, i2s_pin_function_t fn, uint8_t hw_id) {
    uint16_t mapping_index;
    if (lookup_gpio(pin, fn, hw_id, &mapping_index)) {
        iomux_table_t iom = i2s_gpio_map[mapping_index].iomux;
        IOMUXC_SetPinMux(iom.muxRegister, iom.muxMode, iom.inputRegister, iom.inputDaisy, iom.configRegister, 1U);
        IOMUXC_SetPinConfig(iom.muxRegister, iom.muxMode, iom.inputRegister, iom.inputDaisy, iom.configRegister,
            pin_generate_config(PIN_PULL_DISABLED, PIN_MODE_OUT, 2, iom.configRegister));
        return true;
    } else {
        return false;
    }
}

STATIC bool is_rate_supported(int32_t rate) {
    for (uint16_t i = 0; i < ARRAY_SIZE(clock_config_map); i++) {
        if (clock_config_map[i].rate == rate) {
            return true;
        }
    }
    return false;
}

STATIC const clock_audio_pll_config_t *get_pll_config(int32_t rate) {
    for (uint16_t i = 0; i < ARRAY_SIZE(clock_config_map); i++) {
        if (clock_config_map[i].rate == rate) {
            return clock_config_map[i].pll_config;
        }
    }
    return 0;
}

STATIC const uint32_t get_clock_pre_divider(int32_t rate) {
    for (uint16_t i = 0; i < ARRAY_SIZE(clock_config_map); i++) {
        if (clock_config_map[i].rate == rate) {
            return clock_config_map[i].clock_pre_divider;
        }
    }
    return 0;
}

STATIC const uint32_t get_clock_divider(int32_t rate) {
    for (uint16_t i = 0; i < ARRAY_SIZE(clock_config_map); i++) {
        if (clock_config_map[i].rate == rate) {
            return clock_config_map[i].clock_divider;
        }
    }
    return 0;
}

STATIC uint32_t fill_appbuf_from_ringbuf(machine_i2s_obj_t *self, mp_buffer_info_t *appbuf) {

    // copy audio samples from the ring buffer to the app buffer
    // loop, copying samples until the app buffer is filled
    // For uasyncio mode, the loop will make an early exit if the ring buffer becomes empty
    // Example:
    //   a MicroPython I2S object is configured for 16-bit mono (2 bytes per audio sample).
    //   For every frame coming from the ring buffer (8 bytes), 2 bytes are "cherry picked" and
    //   copied to the supplied app buffer.
    //   Thus, for every 1 byte copied to the app buffer, 4 bytes are read from the ring buffer.
    //   If a 8kB app buffer is supplied, 32kB of audio samples is read from the ring buffer.

    uint32_t num_bytes_copied_to_appbuf = 0;
    uint8_t *app_p = (uint8_t *)appbuf->buf;
    uint8_t appbuf_sample_size_in_bytes = (self->bits == 16? 2 : 4) * (self->format == STEREO ? 2: 1);
    uint32_t num_bytes_needed_from_ringbuf = appbuf->len * (I2S_RX_FRAME_SIZE_IN_BYTES / appbuf_sample_size_in_bytes);
    uint8_t discard_byte;
    while (num_bytes_needed_from_ringbuf) {

        uint8_t f_index = get_frame_mapping_index(self->bits, self->format);

        for (uint8_t i = 0; i < I2S_RX_FRAME_SIZE_IN_BYTES; i++) {
            int8_t r_to_a_mapping = i2s_frame_map[f_index][i];
            if (r_to_a_mapping != -1) {
                if (self->io_mode == BLOCKING) {
                    // poll the ringbuf until a sample becomes available,  copy into appbuf using the mapping transform
                    while (ringbuf_pop(&self->ring_buffer, app_p + r_to_a_mapping) == false) {
                        ;
                    }
                    num_bytes_copied_to_appbuf++;
                } else if (self->io_mode == UASYNCIO) {
                    if (ringbuf_pop(&self->ring_buffer, app_p + r_to_a_mapping) == false) {
                        // ring buffer is empty, exit
                        goto exit;
                    } else {
                        num_bytes_copied_to_appbuf++;
                    }
                } else {
                    return 0;  // should never get here (non-blocking mode does not use this function)
                }
            } else { // r_a_mapping == -1
                // discard unused byte from ring buffer
                if (self->io_mode == BLOCKING) {
                    // poll the ringbuf until a sample becomes available
                    while (ringbuf_pop(&self->ring_buffer, &discard_byte) == false) {
                        ;
                    }
                } else if (self->io_mode == UASYNCIO) {
                    if (ringbuf_pop(&self->ring_buffer, &discard_byte) == false) {
                        // ring buffer is empty, exit
                        goto exit;
                    }
                } else {
                    return 0;  // should never get here (non-blocking mode does not use this function)
                }
            }
            num_bytes_needed_from_ringbuf--;
        }
        app_p += appbuf_sample_size_in_bytes;
    }
exit:
    return num_bytes_copied_to_appbuf;
}

// function is used in IRQ context
STATIC void fill_appbuf_from_ringbuf_non_blocking(machine_i2s_obj_t *self) {

    // attempt to copy a block of audio samples from the ring buffer to the supplied app buffer.
    // audio samples will be formatted as part of the copy operation

    uint32_t num_bytes_copied_to_appbuf = 0;
    uint8_t *app_p = &(((uint8_t *)self->non_blocking_descriptor.appbuf.buf)[self->non_blocking_descriptor.index]);

    uint8_t appbuf_sample_size_in_bytes = (self->bits == 16? 2 : 4) * (self->format == STEREO ? 2: 1);
    uint32_t num_bytes_remaining_to_copy_to_appbuf = self->non_blocking_descriptor.appbuf.len - self->non_blocking_descriptor.index;
    uint32_t num_bytes_remaining_to_copy_from_ring_buffer = num_bytes_remaining_to_copy_to_appbuf *
        (I2S_RX_FRAME_SIZE_IN_BYTES / appbuf_sample_size_in_bytes);
    uint32_t num_bytes_needed_from_ringbuf = MIN(SIZEOF_NON_BLOCKING_COPY_IN_BYTES, num_bytes_remaining_to_copy_from_ring_buffer);
    uint8_t discard_byte;
    if (ringbuf_available_data(&self->ring_buffer) >= num_bytes_needed_from_ringbuf) {
        while (num_bytes_needed_from_ringbuf) {

            uint8_t f_index = get_frame_mapping_index(self->bits, self->format);

            for (uint8_t i = 0; i < I2S_RX_FRAME_SIZE_IN_BYTES; i++) {
                int8_t r_to_a_mapping = i2s_frame_map[f_index][i];
                if (r_to_a_mapping != -1) {
                    ringbuf_pop(&self->ring_buffer, app_p + r_to_a_mapping);
                    num_bytes_copied_to_appbuf++;
                } else { // r_a_mapping == -1
                    // discard unused byte from ring buffer
                    ringbuf_pop(&self->ring_buffer, &discard_byte);
                }
                num_bytes_needed_from_ringbuf--;
            }
            app_p += appbuf_sample_size_in_bytes;
        }
        self->non_blocking_descriptor.index += num_bytes_copied_to_appbuf;

        if (self->non_blocking_descriptor.index >= self->non_blocking_descriptor.appbuf.len) {
            self->non_blocking_descriptor.copy_in_progress = false;
            mp_sched_schedule(self->callback_for_non_blocking, MP_OBJ_FROM_PTR(self));
        }
    }
}

STATIC uint32_t copy_appbuf_to_ringbuf(machine_i2s_obj_t *self, mp_buffer_info_t *appbuf) {

    // copy audio samples from the app buffer to the ring buffer
    // loop, reading samples until the app buffer is emptied
    // for uasyncio mode, the loop will make an early exit if the ring buffer becomes full

    uint32_t a_index = 0;

    while (a_index < appbuf->len) {
        if (self->io_mode == BLOCKING) {
            // copy a byte to the ringbuf when space becomes available
            while (ringbuf_push(&self->ring_buffer, ((uint8_t *)appbuf->buf)[a_index]) == false) {
                ;
            }
            a_index++;
        } else if (self->io_mode == UASYNCIO) {
            if (ringbuf_push(&self->ring_buffer, ((uint8_t *)appbuf->buf)[a_index]) == false) {
                // ring buffer is full, exit
                break;
            } else {
                a_index++;
            }
        } else {
            return 0;  // should never get here (non-blocking mode does not use this function)
        }
    }

    return a_index;
}

// function is used in IRQ context
STATIC void copy_appbuf_to_ringbuf_non_blocking(machine_i2s_obj_t *self) {

    // copy audio samples from app buffer into ring buffer
    uint32_t num_bytes_remaining_to_copy = self->non_blocking_descriptor.appbuf.len - self->non_blocking_descriptor.index;
    uint32_t num_bytes_to_copy = MIN(SIZEOF_NON_BLOCKING_COPY_IN_BYTES, num_bytes_remaining_to_copy);

    if (ringbuf_available_space(&self->ring_buffer) >= num_bytes_to_copy) {
        for (uint32_t i = 0; i < num_bytes_to_copy; i++) {
            ringbuf_push(&self->ring_buffer,
                ((uint8_t *)self->non_blocking_descriptor.appbuf.buf)[self->non_blocking_descriptor.index + i]);
        }

        self->non_blocking_descriptor.index += num_bytes_to_copy;
        if (self->non_blocking_descriptor.index >= self->non_blocking_descriptor.appbuf.len) {
            self->non_blocking_descriptor.copy_in_progress = false;
            mp_sched_schedule(self->callback_for_non_blocking, MP_OBJ_FROM_PTR(self));
        }
    }
}

// function is used in IRQ context
STATIC void empty_dma(machine_i2s_obj_t *self, ping_pong_t dma_ping_pong) {
    uint16_t dma_buffer_offset = 0;

    if (dma_ping_pong == TOP_HALF) {
        dma_buffer_offset = 0;
    } else { // BOTTOM_HALF
        dma_buffer_offset = SIZEOF_HALF_DMA_BUFFER_IN_BYTES;
    }

    uint8_t *dma_buffer_p = &self->dma_buffer_dcache_aligned[dma_buffer_offset];

    // flush and invalidate cache so the CPU reads data placed into RAM by DMA
    MP_HAL_CLEANINVALIDATE_DCACHE(dma_buffer_p, SIZEOF_HALF_DMA_BUFFER_IN_BYTES);

    // when space exists, copy samples into ring buffer
    if (ringbuf_available_space(&self->ring_buffer) >= SIZEOF_HALF_DMA_BUFFER_IN_BYTES) {
        for (uint32_t i = 0; i < SIZEOF_HALF_DMA_BUFFER_IN_BYTES; i++) {
            ringbuf_push(&self->ring_buffer, dma_buffer_p[i]);
        }
    }
}

// function is used in IRQ context
STATIC void feed_dma(machine_i2s_obj_t *self, ping_pong_t dma_ping_pong) {
    uint16_t dma_buffer_offset = 0;

    if (dma_ping_pong == TOP_HALF) {
        dma_buffer_offset = 0;
    } else { // BOTTOM_HALF
        dma_buffer_offset = SIZEOF_HALF_DMA_BUFFER_IN_BYTES;
    }

    uint8_t *dma_buffer_p = &self->dma_buffer_dcache_aligned[dma_buffer_offset];

    // when data exists, copy samples from ring buffer
    if (ringbuf_available_data(&self->ring_buffer) >= SIZEOF_HALF_DMA_BUFFER_IN_BYTES) {

        // copy a block of samples from the ring buffer to the dma buffer.
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

    // flush cache to RAM so DMA can read the sample data
    MP_HAL_CLEAN_DCACHE(dma_buffer_p, SIZEOF_HALF_DMA_BUFFER_IN_BYTES);
}

STATIC void edma_i2s_callback(edma_handle_t *handle, void *userData, bool transferDone, uint32_t tcds) {
    machine_i2s_obj_t *self = userData;

    if (self->mode == TX) {
        // for non-blocking mode, sample copying (appbuf->ibuf) is initiated in this callback routine
        if ((self->io_mode == NON_BLOCKING) && (self->non_blocking_descriptor.copy_in_progress)) {
            copy_appbuf_to_ringbuf_non_blocking(self);
        }

        if (transferDone) {
            // bottom half of buffer now emptied,
            // safe to fill the bottom half while the top half of buffer is being emptied
            feed_dma(self, BOTTOM_HALF);
        } else {
            // top half of buffer now emptied,
            // safe to fill the top half while the bottom half of buffer is being emptied
            feed_dma(self, TOP_HALF);
        }
    } else { // RX
        if (transferDone) {
            // bottom half of buffer now filled,
            // safe to empty the bottom half while the top half of buffer is being filled
            empty_dma(self, BOTTOM_HALF);
        } else {
            // top half of buffer now filled,
            // safe to empty the top half while the bottom half of buffer is being filled
            empty_dma(self, TOP_HALF);
        }

        // for non-blocking mode, sample copying (ibuf->appbuf) is initiated in this callback routine
        if ((self->io_mode == NON_BLOCKING) && (self->non_blocking_descriptor.copy_in_progress)) {
            fill_appbuf_from_ringbuf_non_blocking(self);
        }
    }
}

STATIC bool i2s_init(machine_i2s_obj_t *self) {

    #if defined(MIMXRT117x_SERIES)
    clock_audio_pll_config_t pll_config = *get_pll_config(self->rate);
    pll_config.postDivider = get_clock_pre_divider(self->rate);
    CLOCK_InitAudioPll(&pll_config);
    CLOCK_SetRootClockMux(i2s_clock_mux[self->i2s_id], I2S_AUDIO_PLL_CLOCK);
    CLOCK_SetRootClockDiv(i2s_clock_mux[self->i2s_id], get_clock_divider(self->rate));
    uint32_t clock_freq = CLOCK_GetFreq(kCLOCK_AudioPllOut) / get_clock_divider(self->rate);

    #else

    CLOCK_InitAudioPll(get_pll_config(self->rate));
    CLOCK_SetMux(i2s_clock_mux[self->i2s_id], I2S_AUDIO_PLL_CLOCK);
    CLOCK_SetDiv(i2s_clock_pre_div[self->i2s_id], get_clock_pre_divider(self->rate));
    CLOCK_SetDiv(i2s_clock_div[self->i2s_id], get_clock_divider(self->rate));
    uint32_t clock_freq =
        (CLOCK_GetFreq(kCLOCK_AudioPllClk) / (get_clock_divider(self->rate) + 1U) /
            (get_clock_pre_divider(self->rate) + 1U));
    #endif

    if (!set_iomux(self->sck, SCK, self->i2s_id)) {
        return false;
    }

    if (!set_iomux(self->ws, WS, self->i2s_id)) {
        return false;
    }

    if (!set_iomux(self->sd, SD, self->i2s_id)) {
        return false;
    }

    if (self->mck) {
        if (!set_iomux(self->mck, MCK, self->i2s_id)) {
            return false;
        }
        #if defined(MIMXRT117x_SERIES)
        switch (self->i2s_id) {
            case 1:
                IOMUXC_GPR->GPR0 |= IOMUXC_GPR_GPR0_SAI1_MCLK_DIR_MASK;
                break;
            case 2:
                IOMUXC_GPR->GPR1 |= IOMUXC_GPR_GPR1_SAI2_MCLK_DIR_MASK;
                break;
            case 3:
                IOMUXC_GPR->GPR2 |= IOMUXC_GPR_GPR2_SAI3_MCLK_DIR_MASK;
                break;
            case 4:
                IOMUXC_GPR->GPR2 |= IOMUXC_GPR_GPR2_SAI4_MCLK_DIR_MASK;
                break;
        }
        #else
        IOMUXC_EnableMode(IOMUXC_GPR, i2s_iomuxc_gpr_mode[self->i2s_id], true);
        #endif
    }

    self->dma_channel = allocate_dma_channel();

    DMAMUX_Init(DMAMUX);
    if (self->mode == TX) {
        DMAMUX_SetSource(DMAMUX, self->dma_channel, i2s_dma_req_src_tx[self->i2s_id]);
    } else { // RX
        DMAMUX_SetSource(DMAMUX, self->dma_channel, i2s_dma_req_src_rx[self->i2s_id]);
    }
    DMAMUX_EnableChannel(DMAMUX, self->dma_channel);

    dma_init();
    EDMA_CreateHandle(&self->edmaHandle, DMA0, self->dma_channel);
    EDMA_SetCallback(&self->edmaHandle, edma_i2s_callback, self);
    EDMA_ResetChannel(DMA0, self->dma_channel);

    SAI_Init(self->i2s_inst);

    sai_transceiver_t saiConfig;
    SAI_GetClassicI2SConfig(&saiConfig, get_dma_bits(self->mode, self->bits), kSAI_Stereo, kSAI_Channel0Mask);
    saiConfig.masterSlave = kSAI_Master;

    uint16_t sck_index;
    lookup_gpio(self->sck, SCK, self->i2s_id, &sck_index);

    if ((self->mode == TX) && (i2s_gpio_map[sck_index].mode == TX)) {
        saiConfig.syncMode = kSAI_ModeAsync;
        SAI_TxSetConfig(self->i2s_inst, &saiConfig);
    } else if ((self->mode == RX) && (i2s_gpio_map[sck_index].mode == RX)) {
        saiConfig.syncMode = kSAI_ModeAsync;
        SAI_RxSetConfig(self->i2s_inst, &saiConfig);
    } else if ((self->mode == TX) && (i2s_gpio_map[sck_index].mode == RX)) {
        saiConfig.syncMode = kSAI_ModeAsync;
        SAI_RxSetConfig(self->i2s_inst, &saiConfig);
        saiConfig.bitClock.bclkSrcSwap = true;
        saiConfig.syncMode = kSAI_ModeSync;
        SAI_TxSetConfig(self->i2s_inst, &saiConfig);
    } else if ((self->mode == RX) && (i2s_gpio_map[sck_index].mode == TX)) {
        saiConfig.syncMode = kSAI_ModeAsync;
        SAI_TxSetConfig(self->i2s_inst, &saiConfig);
        saiConfig.syncMode = kSAI_ModeSync;
        SAI_RxSetConfig(self->i2s_inst, &saiConfig);
    } else {
        return false; // should never happen
    }

    SAI_TxSetBitClockRate(self->i2s_inst, clock_freq, self->rate, get_dma_bits(self->mode, self->bits),
        SAI_NUM_AUDIO_CHANNELS);
    SAI_RxSetBitClockRate(self->i2s_inst, clock_freq, self->rate, get_dma_bits(self->mode, self->bits),
        SAI_NUM_AUDIO_CHANNELS);

    edma_transfer_config_t transferConfig;
    uint8_t bytes_per_sample = get_dma_bits(self->mode, self->bits) / 8;

    if (self->mode == TX) {
        uint32_t destAddr = SAI_TxGetDataRegisterAddress(self->i2s_inst, SAI_CHANNEL_0);
        EDMA_PrepareTransfer(&transferConfig,
            self->dma_buffer_dcache_aligned, bytes_per_sample,
            (void *)destAddr, bytes_per_sample,
            (FSL_FEATURE_SAI_FIFO_COUNT - saiConfig.fifo.fifoWatermark) * bytes_per_sample,
            SIZEOF_DMA_BUFFER_IN_BYTES, kEDMA_MemoryToPeripheral);
    } else { // RX
        uint32_t srcAddr = SAI_RxGetDataRegisterAddress(self->i2s_inst, SAI_CHANNEL_0);
        EDMA_PrepareTransfer(&transferConfig,
            (void *)srcAddr, bytes_per_sample,
            self->dma_buffer_dcache_aligned, bytes_per_sample,
            (FSL_FEATURE_SAI_FIFO_COUNT - saiConfig.fifo.fifoWatermark) * bytes_per_sample,
            SIZEOF_DMA_BUFFER_IN_BYTES, kEDMA_PeripheralToMemory);
    }

    memset(self->edmaTcd, 0, sizeof(edma_tcd_t));

    // continuous DMA operation is acheived using the scatter/gather feature, with one TCD linked back to itself
    EDMA_TcdSetTransferConfig(self->edmaTcd, &transferConfig, self->edmaTcd);
    EDMA_TcdEnableInterrupts(self->edmaTcd, kEDMA_MajorInterruptEnable | kEDMA_HalfInterruptEnable);
    EDMA_InstallTCD(DMA0, self->dma_channel, self->edmaTcd);
    EDMA_StartTransfer(&self->edmaHandle);

    if (self->mode == TX) {
        SAI_TxEnableDMA(self->i2s_inst, kSAI_FIFORequestDMAEnable, true);
        SAI_TxEnable(self->i2s_inst, true);
        SAI_TxSetChannelFIFOMask(self->i2s_inst, kSAI_Channel0Mask);
    } else { // RX
        SAI_RxEnableDMA(self->i2s_inst, kSAI_FIFORequestDMAEnable, true);
        SAI_RxEnable(self->i2s_inst, true);
        SAI_RxSetChannelFIFOMask(self->i2s_inst, kSAI_Channel0Mask);
    }

    return true;
}

STATIC void machine_i2s_init_helper(machine_i2s_obj_t *self, size_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    enum {
        ARG_sck,
        ARG_ws,
        ARG_sd,
        ARG_mck,
        ARG_mode,
        ARG_bits,
        ARG_format,
        ARG_rate,
        ARG_ibuf,
    };

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_sck,      MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ,   {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_ws,       MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ,   {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_sd,       MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ,   {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_mck,      MP_ARG_KW_ONLY | MP_ARG_OBJ,   {.u_obj = mp_const_none} },
        { MP_QSTR_mode,     MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT,   {.u_int = -1} },
        { MP_QSTR_bits,     MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT,   {.u_int = -1} },
        { MP_QSTR_format,   MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT,   {.u_int = -1} },
        { MP_QSTR_rate,     MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT,   {.u_int = -1} },
        { MP_QSTR_ibuf,     MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT,   {.u_int = -1} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_pos_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    //
    // ---- Check validity of arguments ----
    //

    // is Mode valid?
    uint16_t i2s_mode = args[ARG_mode].u_int;
    if ((i2s_mode != (RX)) &&
        (i2s_mode != (TX))) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid mode"));
    }

    // are I2S pin assignments valid?
    uint16_t not_used;

    // is SCK valid?
    const machine_pin_obj_t *pin_sck = pin_find(args[ARG_sck].u_obj);
    if (!lookup_gpio(pin_sck, SCK, self->i2s_id, &not_used)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid SCK pin"));
    }

    // is WS valid?
    const machine_pin_obj_t *pin_ws = pin_find(args[ARG_ws].u_obj);
    if (!lookup_gpio(pin_ws, WS, self->i2s_id, &not_used)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid WS pin"));
    }

    // is SD valid?
    const machine_pin_obj_t *pin_sd = pin_find(args[ARG_sd].u_obj);
    uint16_t mapping_index;
    bool invalid_sd = true;
    if (lookup_gpio(pin_sd, SD, self->i2s_id, &mapping_index)) {
        if (i2s_mode == i2s_gpio_map[mapping_index].mode) {
            invalid_sd = false;
        }
    }

    if (invalid_sd) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid SD pin"));
    }

    // is MCK defined and valid?
    const machine_pin_obj_t *pin_mck = NULL;
    if (args[ARG_mck].u_obj != mp_const_none) {
        pin_mck = pin_find(args[ARG_mck].u_obj);
        if (!lookup_gpio(pin_mck, MCK, self->i2s_id, &not_used)) {
            mp_raise_ValueError(MP_ERROR_TEXT("invalid MCK pin"));
        }
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
    int32_t i2s_rate = args[ARG_rate].u_int;
    if (!is_rate_supported(i2s_rate)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid rate"));
    }

    // is Ibuf valid?
    int32_t ring_buffer_len = args[ARG_ibuf].u_int;
    if (ring_buffer_len > 0) {
        uint8_t *buffer = m_new(uint8_t, ring_buffer_len);
        self->ring_buffer_storage = buffer;
        ringbuf_init(&self->ring_buffer, buffer, ring_buffer_len);
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid ibuf"));
    }

    self->sck = pin_sck;
    self->ws = pin_ws;
    self->sd = pin_sd;
    self->mck = pin_mck;
    self->mode = i2s_mode;
    self->bits = i2s_bits;
    self->format = i2s_format;
    self->rate = i2s_rate;
    self->ibuf = ring_buffer_len;
    self->callback_for_non_blocking = MP_OBJ_NULL;
    self->non_blocking_descriptor.copy_in_progress = false;
    self->io_mode = BLOCKING;
    self->i2s_inst = (I2S_Type *)i2s_base_ptr[self->i2s_id];

    // init the I2S bus
    if (!i2s_init(self)) {
        mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("I2S init failed"));
    }
}

STATIC void machine_i2s_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_i2s_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "I2S(id=%u,\n"
        "sck="MP_HAL_PIN_FMT ",\n"
        "ws="MP_HAL_PIN_FMT ",\n"
        "sd="MP_HAL_PIN_FMT ",\n"
        "mck="MP_HAL_PIN_FMT ",\n"
        "mode=%u,\n"
        "bits=%u, format=%u,\n"
        "rate=%d, ibuf=%d)",
        self->i2s_id,
        mp_hal_pin_name(self->sck),
        mp_hal_pin_name(self->ws),
        mp_hal_pin_name(self->sd),
        mp_hal_pin_name(self->mck),
        self->mode,
        self->bits, self->format,
        self->rate, self->ibuf
        );
}

STATIC mp_obj_t machine_i2s_make_new(const mp_obj_type_t *type, size_t n_pos_args, size_t n_kw_args, const mp_obj_t *args) {
    mp_arg_check_num(n_pos_args, n_kw_args, 1, MP_OBJ_FUN_ARGS_MAX, true);
    uint8_t i2s_id = mp_obj_get_int(args[0]);

    if (i2s_id < 1 || i2s_id > MICROPY_HW_I2S_NUM) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("I2S(%d) does not exist"), i2s_id);
    }

    uint8_t i2s_id_zero_base = i2s_id - 1;

    machine_i2s_obj_t *self;
    if (MP_STATE_PORT(machine_i2s_obj)[i2s_id_zero_base] == NULL) {
        self = mp_obj_malloc(machine_i2s_obj_t, &machine_i2s_type);
        MP_STATE_PORT(machine_i2s_obj)[i2s_id_zero_base] = self;
        self->i2s_id = i2s_id;
        self->edmaTcd = &edmaTcd[i2s_id_zero_base];
    } else {
        self = MP_STATE_PORT(machine_i2s_obj)[i2s_id_zero_base];
        machine_i2s_deinit(MP_OBJ_FROM_PTR(self));
    }

    // align DMA buffer to the cache line size (32 bytes)
    self->dma_buffer_dcache_aligned = (uint8_t *)((uint32_t)(self->dma_buffer + 0x1f) & ~0x1f);

    // fill the DMA buffer with NULLs
    memset(self->dma_buffer_dcache_aligned, 0, SIZEOF_DMA_BUFFER_IN_BYTES);

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw_args, args + n_pos_args);
    machine_i2s_init_helper(self, n_pos_args - 1, args + 1, &kw_args);
    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t machine_i2s_init(size_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    machine_i2s_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    machine_i2s_deinit(MP_OBJ_FROM_PTR(self));
    machine_i2s_init_helper(self, n_pos_args - 1, pos_args + 1, kw_args);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_i2s_init_obj, 1, machine_i2s_init);

STATIC mp_obj_t machine_i2s_deinit(mp_obj_t self_in) {
    machine_i2s_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // use self->i2s_inst as in indication that I2S object has already been de-initialized
    if (self->i2s_inst != NULL) {
        EDMA_AbortTransfer(&self->edmaHandle);

        if (self->mode == TX) {
            SAI_TxSetChannelFIFOMask(self->i2s_inst, 0);
            SAI_TxEnableDMA(self->i2s_inst, kSAI_FIFORequestDMAEnable, false);
            SAI_TxEnable(self->i2s_inst, false);
            SAI_TxReset(self->i2s_inst);
        } else { // RX
            SAI_RxSetChannelFIFOMask(self->i2s_inst, 0);
            SAI_RxEnableDMA(self->i2s_inst, kSAI_FIFORequestDMAEnable, false);
            SAI_RxEnable(self->i2s_inst, false);
            SAI_RxReset(self->i2s_inst);
        }

        SAI_Deinit(self->i2s_inst);
        free_dma_channel(self->dma_channel);
        m_free(self->ring_buffer_storage);
        self->i2s_inst = NULL;  // flag object as de-initialized
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_i2s_deinit_obj, machine_i2s_deinit);

STATIC mp_obj_t machine_i2s_irq(mp_obj_t self_in, mp_obj_t handler) {
    machine_i2s_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (handler != mp_const_none && !mp_obj_is_callable(handler)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid callback"));
    }

    if (handler != mp_const_none) {
        self->io_mode = NON_BLOCKING;
    } else {
        self->io_mode = BLOCKING;
    }

    self->callback_for_non_blocking = handler;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(machine_i2s_irq_obj, machine_i2s_irq);

// Shift() is typically used as a volume control.
// shift=1 increases volume by 6dB, shift=-1 decreases volume by 6dB
STATIC mp_obj_t machine_i2s_shift(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buf, ARG_bits, ARG_shift};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buf,    MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_bits,   MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_shift,  MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buf].u_obj, &bufinfo, MP_BUFFER_RW);

    int16_t *buf_16 = bufinfo.buf;
    int32_t *buf_32 = bufinfo.buf;

    uint8_t bits = args[ARG_bits].u_int;
    int8_t shift = args[ARG_shift].u_int;

    uint32_t num_audio_samples;
    switch (bits) {
        case 16:
            num_audio_samples = bufinfo.len / sizeof(uint16_t);
            break;

        case 32:
            num_audio_samples = bufinfo.len / sizeof(uint32_t);
            break;

        default:
            mp_raise_ValueError(MP_ERROR_TEXT("invalid bits"));
            break;
    }

    for (uint32_t i = 0; i < num_audio_samples; i++) {
        switch (bits) {
            case 16:
                if (shift >= 0) {
                    buf_16[i] = buf_16[i] << shift;
                } else {
                    buf_16[i] = buf_16[i] >> abs(shift);
                }
                break;
            case 32:
                if (shift >= 0) {
                    buf_32[i] = buf_32[i] << shift;
                } else {
                    buf_32[i] = buf_32[i] >> abs(shift);
                }
                break;
        }
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_i2s_shift_fun_obj, 0, machine_i2s_shift);
STATIC MP_DEFINE_CONST_STATICMETHOD_OBJ(machine_i2s_shift_obj, MP_ROM_PTR(&machine_i2s_shift_fun_obj));

STATIC const mp_rom_map_elem_t machine_i2s_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_init),            MP_ROM_PTR(&machine_i2s_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto),        MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write),           MP_ROM_PTR(&mp_stream_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),          MP_ROM_PTR(&machine_i2s_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq),             MP_ROM_PTR(&machine_i2s_irq_obj) },

    // Static method
    { MP_ROM_QSTR(MP_QSTR_shift),           MP_ROM_PTR(&machine_i2s_shift_obj) },

    // Constants
    { MP_ROM_QSTR(MP_QSTR_RX),              MP_ROM_INT(RX) },
    { MP_ROM_QSTR(MP_QSTR_TX),              MP_ROM_INT(TX) },
    { MP_ROM_QSTR(MP_QSTR_STEREO),          MP_ROM_INT(STEREO) },
    { MP_ROM_QSTR(MP_QSTR_MONO),            MP_ROM_INT(MONO) },
};
MP_DEFINE_CONST_DICT(machine_i2s_locals_dict, machine_i2s_locals_dict_table);

STATIC mp_uint_t machine_i2s_stream_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    machine_i2s_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->mode != RX) {
        *errcode = MP_EPERM;
        return MP_STREAM_ERROR;
    }

    uint8_t appbuf_sample_size_in_bytes = (self->bits / 8) * (self->format == STEREO ? 2: 1);
    if (size % appbuf_sample_size_in_bytes != 0) {
        *errcode = MP_EINVAL;
        return MP_STREAM_ERROR;
    }

    if (size == 0) {
        return 0;
    }

    if (self->io_mode == NON_BLOCKING) {
        self->non_blocking_descriptor.appbuf.buf = (void *)buf_in;
        self->non_blocking_descriptor.appbuf.len = size;
        self->non_blocking_descriptor.index = 0;
        self->non_blocking_descriptor.copy_in_progress = true;
        return size;
    } else { // blocking or uasyncio mode
        mp_buffer_info_t appbuf;
        appbuf.buf = (void *)buf_in;
        appbuf.len = size;
        uint32_t num_bytes_read = fill_appbuf_from_ringbuf(self, &appbuf);
        return num_bytes_read;
    }
}

STATIC mp_uint_t machine_i2s_stream_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    machine_i2s_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->mode != TX) {
        *errcode = MP_EPERM;
        return MP_STREAM_ERROR;
    }

    if (size == 0) {
        return 0;
    }

    if (self->io_mode == NON_BLOCKING) {
        self->non_blocking_descriptor.appbuf.buf = (void *)buf_in;
        self->non_blocking_descriptor.appbuf.len = size;
        self->non_blocking_descriptor.index = 0;
        self->non_blocking_descriptor.copy_in_progress = true;
        return size;
    } else { // blocking or uasyncio mode
        mp_buffer_info_t appbuf;
        appbuf.buf = (void *)buf_in;
        appbuf.len = size;
        uint32_t num_bytes_written = copy_appbuf_to_ringbuf(self, &appbuf);
        return num_bytes_written;
    }
}

STATIC mp_uint_t machine_i2s_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    machine_i2s_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_uint_t ret;
    uintptr_t flags = arg;
    self->io_mode = UASYNCIO; // a call to ioctl() is an indication that uasyncio is being used

    if (request == MP_STREAM_POLL) {
        ret = 0;

        if (flags & MP_STREAM_POLL_RD) {
            if (self->mode != RX) {
                *errcode = MP_EPERM;
                return MP_STREAM_ERROR;
            }

            if (!ringbuf_is_empty(&self->ring_buffer)) {
                ret |= MP_STREAM_POLL_RD;
            }
        }

        if (flags & MP_STREAM_POLL_WR) {
            if (self->mode != TX) {
                *errcode = MP_EPERM;
                return MP_STREAM_ERROR;
            }

            if (!ringbuf_is_full(&self->ring_buffer)) {
                ret |= MP_STREAM_POLL_WR;
            }
        }
    } else {
        *errcode = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }

    return ret;
}

STATIC const mp_stream_p_t i2s_stream_p = {
    .read = machine_i2s_stream_read,
    .write = machine_i2s_stream_write,
    .ioctl = machine_i2s_ioctl,
    .is_text = false,
};

MP_DEFINE_CONST_OBJ_TYPE(
    machine_i2s_type,
    MP_QSTR_I2S,
    MP_TYPE_FLAG_ITER_IS_STREAM,
    make_new, machine_i2s_make_new,
    print, machine_i2s_print,
    protocol, &i2s_stream_p,
    locals_dict, &machine_i2s_locals_dict
    );

MP_REGISTER_ROOT_POINTER(struct _machine_i2s_obj_t *machine_i2s_obj[MICROPY_HW_I2S_NUM]);

#endif // MICROPY_PY_MACHINE_I2S
