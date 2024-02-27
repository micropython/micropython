/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Bryan Morrissey
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
#include "pin.h"
#include "dma.h"
#include "genhdr/plli2stable.h"

// Notes on this port's specific implementation of I2S:
// - the DMA callbacks (1/2 complete and complete) are used to implement the asynchronous background operations
// - all 3 Modes of operation are implemented using the HAL I2S Generic Driver
// - all sample data transfers use DMA
// - the DMA controller is configured in Circular mode to fulfil continuous and gapless sample flows
// - the DMA ping-pong buffer needs to be aligned to a cache line size of 32 bytes.  32 byte
//   alignment is needed to use the routines that clean and invalidate D-Cache which work on a
//   32 byte address boundary.  Not all STM32 devices have a D-Cache.  Buffer alignment
//   will still happen on these devices to keep this code simple.

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

typedef enum {
    TOP_HALF,
    BOTTOM_HALF
} ping_pong_t;

typedef struct _plli2s_config_t {
    uint32_t rate;
    uint8_t bits;
    uint8_t plli2sr;
    uint16_t plli2sn;
} plli2s_config_t;

typedef struct _machine_i2s_obj_t {
    mp_obj_base_t base;
    uint8_t i2s_id;
    mp_hal_pin_obj_t sck;
    mp_hal_pin_obj_t ws;
    mp_hal_pin_obj_t sd;
    uint16_t mode;
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
    I2S_HandleTypeDef hi2s;
    DMA_HandleTypeDef hdma_tx;
    DMA_HandleTypeDef hdma_rx;
    const dma_descr_t *dma_descr_tx;
    const dma_descr_t *dma_descr_rx;
} machine_i2s_obj_t;

static mp_obj_t machine_i2s_deinit(mp_obj_t self_in);

// The frame map is used with the readinto() method to transform the audio sample data coming
// from DMA memory (32-bit stereo) to the format specified
// in the I2S constructor.  e.g.  16-bit mono
static const int8_t i2s_frame_map[NUM_I2S_USER_FORMATS][I2S_RX_FRAME_SIZE_IN_BYTES] = {
    { 0,  1, -1, -1, -1, -1, -1, -1 },  // Mono, 16-bits
    { 2,  3,  0,  1, -1, -1, -1, -1 },  // Mono, 32-bits
    { 0,  1, -1, -1,  2,  3, -1, -1 },  // Stereo, 16-bits
    { 2,  3,  0,  1,  6,  7,  4,  5 },  // Stereo, 32-bits
};

static const plli2s_config_t plli2s_config[] = PLLI2S_TABLE;

void machine_i2s_init0() {
    for (uint8_t i = 0; i < MICROPY_HW_MAX_I2S; i++) {
        MP_STATE_PORT(machine_i2s_obj)[i] = NULL;
    }
}

static bool lookup_plli2s_config(int8_t bits, int32_t rate, uint16_t *plli2sn, uint16_t *plli2sr) {
    for (uint16_t i = 0; i < MP_ARRAY_SIZE(plli2s_config); i++) {
        if ((plli2s_config[i].bits == bits) && (plli2s_config[i].rate == rate)) {
            *plli2sn = plli2s_config[i].plli2sn;
            *plli2sr = plli2s_config[i].plli2sr;
            return true;
        }
    }

    return false;
}

//  For 32-bit audio samples, the STM32 HAL API expects each 32-bit sample to be encoded
//  in an unusual byte ordering:  Byte_2, Byte_3, Byte_0, Byte_1
//      where:  Byte_0 is the least significant byte of the 32-bit sample
//
//  The following function takes a buffer containing 32-bits sample values formatted as little endian
//  and performs an in-place modification into the STM32 HAL API convention
//
//  Example:
//
//   wav_samples[] = [L_0-7,   L_8-15,  L_16-23, L_24-31, R_0-7,   R_8-15,  R_16-23, R_24-31] =  [Left channel, Right channel]
//   stm_api[] =     [L_16-23, L_24-31, L_0-7,   L_8-15,  R_16-23, R_24-31, R_0-7,   R_8-15] = [Left channel, Right channel]
//
//   where:
//      L_0-7 is the least significant byte of the 32 bit sample in the Left channel
//      L_24-31 is the most significant byte of the 32 bit sample in the Left channel
//
//   wav_samples[] =  [0x99, 0xBB, 0x11, 0x22, 0x44, 0x55, 0xAB, 0x77] = [Left channel, Right channel]
//   stm_api[] =      [0x11, 0x22, 0x99, 0xBB, 0xAB, 0x77, 0x44, 0x55] = [Left channel, Right channel]
//
//   where:
//      LEFT Channel =  0x99, 0xBB, 0x11, 0x22
//      RIGHT Channel = 0x44, 0x55, 0xAB, 0x77
static void reformat_32_bit_samples(int32_t *sample, uint32_t num_samples) {
    int16_t sample_ms;
    int16_t sample_ls;
    for (uint32_t i = 0; i < num_samples; i++) {
        sample_ls = sample[i] & 0xFFFF;
        sample_ms = sample[i] >> 16;
        sample[i] = (sample_ls << 16) + sample_ms;
    }
}

static int8_t get_frame_mapping_index(int8_t bits, format_t format) {
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

static int8_t get_dma_bits(uint16_t mode, int8_t bits) {
    if (mode == I2S_MODE_MASTER_TX) {
        if (bits == 16) {
            return I2S_DATAFORMAT_16B;
        } else {
            return I2S_DATAFORMAT_32B;
        }
        return bits;
    } else { // Master Rx
        // always read 32 bit words for I2S e.g.  I2S MEMS microphones
        return I2S_DATAFORMAT_32B;
    }
}

// function is used in IRQ context
static void empty_dma(machine_i2s_obj_t *self, ping_pong_t dma_ping_pong) {
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
static void feed_dma(machine_i2s_obj_t *self, ping_pong_t dma_ping_pong) {
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

        // reformat 32 bit samples to match STM32 HAL API format
        if (self->bits == 32) {
            reformat_32_bit_samples((int32_t *)dma_buffer_p, SIZEOF_HALF_DMA_BUFFER_IN_BYTES / (sizeof(uint32_t)));
        }
    } else {
        // underflow.  clear buffer to transmit "silence" on the I2S bus
        memset(dma_buffer_p, 0, SIZEOF_HALF_DMA_BUFFER_IN_BYTES);
    }

    // flush cache to RAM so DMA can read the sample data
    MP_HAL_CLEAN_DCACHE(dma_buffer_p, SIZEOF_HALF_DMA_BUFFER_IN_BYTES);
}

static bool i2s_init(machine_i2s_obj_t *self) {

    // init the GPIO lines
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    GPIO_InitStructure.Pull = GPIO_PULLUP;

    if (self->i2s_id == 1) {
        self->hi2s.Instance = I2S1;
        __SPI1_CLK_ENABLE();
        // configure DMA streams
        if (self->mode == I2S_MODE_MASTER_RX) {
            self->dma_descr_rx = &dma_I2S_1_RX;
        } else {
            self->dma_descr_tx = &dma_I2S_1_TX;
        }
    } else if (self->i2s_id == 2) {
        self->hi2s.Instance = I2S2;
        __SPI2_CLK_ENABLE();
        // configure DMA streams
        if (self->mode == I2S_MODE_MASTER_RX) {
            self->dma_descr_rx = &dma_I2S_2_RX;
        } else {
            self->dma_descr_tx = &dma_I2S_2_TX;
        }
    } else {
        // invalid id number; should not get here as i2s object should not
        // have been created without setting a valid i2s instance number
        return false;
    }

    // GPIO Pin initialization
    if (self->sck != MP_OBJ_TO_PTR(MP_OBJ_NULL)) {
        GPIO_InitStructure.Pin = self->sck->pin_mask;
        const pin_af_obj_t *af = pin_find_af(self->sck, AF_FN_I2S, self->i2s_id);
        GPIO_InitStructure.Alternate = (uint8_t)af->idx;
        HAL_GPIO_Init(self->sck->gpio, &GPIO_InitStructure);
    }

    if (self->ws != MP_OBJ_TO_PTR(MP_OBJ_NULL)) {
        GPIO_InitStructure.Pin = self->ws->pin_mask;
        const pin_af_obj_t *af = pin_find_af(self->ws, AF_FN_I2S, self->i2s_id);
        GPIO_InitStructure.Alternate = (uint8_t)af->idx;
        HAL_GPIO_Init(self->ws->gpio, &GPIO_InitStructure);
    }

    if (self->sd != MP_OBJ_TO_PTR(MP_OBJ_NULL)) {
        GPIO_InitStructure.Pin = self->sd->pin_mask;
        const pin_af_obj_t *af = pin_find_af(self->sd, AF_FN_I2S, self->i2s_id);
        GPIO_InitStructure.Alternate = (uint8_t)af->idx;
        HAL_GPIO_Init(self->sd->gpio, &GPIO_InitStructure);
    }

    // configure I2S PLL
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2S;

    // lookup optimal PLL multiplier (PLLI2SN) and divisor (PLLI2SR) for a given sample size and sampling frequency
    uint16_t plli2sn;
    uint16_t plli2sr;

    if (lookup_plli2s_config(self->mode == I2S_MODE_MASTER_RX ? 32 : self->bits, self->rate, &plli2sn, &plli2sr)) {
        // match found
        PeriphClkInitStruct.PLLI2S.PLLI2SN = plli2sn;
        PeriphClkInitStruct.PLLI2S.PLLI2SR = plli2sr;
    } else {
        // no match for sample size and rate
        // configure PLL to use power-on default values when a non-standard sampling frequency is used
        PeriphClkInitStruct.PLLI2S.PLLI2SN = 192;
        PeriphClkInitStruct.PLLI2S.PLLI2SR = 2;
    }

    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
        return false;
    }

    if (HAL_I2S_Init(&self->hi2s) == HAL_OK) {
        // Reset and initialize Tx and Rx DMA channels
        if (self->mode == I2S_MODE_MASTER_RX) {
            dma_invalidate_channel(self->dma_descr_rx);
            dma_init(&self->hdma_rx, self->dma_descr_rx, DMA_PERIPH_TO_MEMORY, &self->hi2s);
            self->hi2s.hdmarx = &self->hdma_rx;
        } else {  // I2S_MODE_MASTER_TX
            dma_invalidate_channel(self->dma_descr_tx);
            dma_init(&self->hdma_tx, self->dma_descr_tx, DMA_MEMORY_TO_PERIPH, &self->hi2s);
            self->hi2s.hdmatx = &self->hdma_tx;
        }

        return true;
    } else {
        return false;
    }
}

void HAL_I2S_ErrorCallback(I2S_HandleTypeDef *hi2s) {
    uint32_t errorCode = HAL_I2S_GetError(hi2s);
    mp_printf(MICROPY_ERROR_PRINTER, "I2S Error = %ld\n", errorCode);
}

void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s) {
    machine_i2s_obj_t *self;
    if (hi2s->Instance == I2S1) {
        self = MP_STATE_PORT(machine_i2s_obj)[0];
    } else {
        self = MP_STATE_PORT(machine_i2s_obj)[1];
    }

    // bottom half of buffer now filled,
    // safe to empty the bottom half while the top half of buffer is being filled
    empty_dma(self, BOTTOM_HALF);

    // for non-blocking operation, this IRQ-based callback handles
    // the readinto() method requests.
    if ((self->io_mode == NON_BLOCKING) && (self->non_blocking_descriptor.copy_in_progress)) {
        fill_appbuf_from_ringbuf_non_blocking(self);
    }
}

void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s) {
    machine_i2s_obj_t *self;
    if (hi2s->Instance == I2S1) {
        self = MP_STATE_PORT(machine_i2s_obj)[0];
    } else {
        self = MP_STATE_PORT(machine_i2s_obj)[1];
    }

    // top half of buffer now filled,
    // safe to empty the top half while the bottom half of buffer is being filled
    empty_dma(self, TOP_HALF);

    // for non-blocking operation, this IRQ-based callback handles
    // the readinto() method requests.
    if ((self->io_mode == NON_BLOCKING) && (self->non_blocking_descriptor.copy_in_progress)) {
        fill_appbuf_from_ringbuf_non_blocking(self);
    }
}

void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s) {
    machine_i2s_obj_t *self;

    if (hi2s->Instance == I2S1) {
        self = MP_STATE_PORT(machine_i2s_obj)[0];
    } else {
        self = MP_STATE_PORT(machine_i2s_obj)[1];
    }

    // for non-blocking operation, this IRQ-based callback handles
    // the write() method requests.
    if ((self->io_mode == NON_BLOCKING) && (self->non_blocking_descriptor.copy_in_progress)) {
        copy_appbuf_to_ringbuf_non_blocking(self);
    }

    // bottom half of buffer now emptied,
    // safe to fill the bottom half while the top half of buffer is being emptied
    feed_dma(self, BOTTOM_HALF);
}

void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s) {
    machine_i2s_obj_t *self;
    if (hi2s->Instance == I2S1) {
        self = MP_STATE_PORT(machine_i2s_obj)[0];
    } else {
        self = MP_STATE_PORT(machine_i2s_obj)[1];
    }

    // for non-blocking operation, this IRQ-based callback handles
    // the write() method requests.
    if ((self->io_mode == NON_BLOCKING) && (self->non_blocking_descriptor.copy_in_progress)) {
        copy_appbuf_to_ringbuf_non_blocking(self);
    }

    // top half of buffer now emptied,
    // safe to fill the top half while the bottom half of buffer is being emptied
    feed_dma(self, TOP_HALF);
}

static void mp_machine_i2s_init_helper(machine_i2s_obj_t *self, mp_arg_val_t *args) {
    memset(&self->hi2s, 0, sizeof(self->hi2s));

    // are I2S pin assignments valid?
    const pin_af_obj_t *pin_af;

    // is SCK valid?
    if (mp_obj_is_type(args[ARG_sck].u_obj, &pin_type)) {
        pin_af = pin_find_af(MP_OBJ_TO_PTR(args[ARG_sck].u_obj), AF_FN_I2S, self->i2s_id);
        if (pin_af->type != AF_PIN_TYPE_I2S_CK) {
            mp_raise_ValueError(MP_ERROR_TEXT("invalid SCK pin"));
        }
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("SCK not a Pin type"));
    }

    // is WS valid?
    if (mp_obj_is_type(args[ARG_ws].u_obj, &pin_type)) {
        pin_af = pin_find_af(MP_OBJ_TO_PTR(args[ARG_ws].u_obj), AF_FN_I2S, self->i2s_id);
        if (pin_af->type != AF_PIN_TYPE_I2S_WS) {
            mp_raise_ValueError(MP_ERROR_TEXT("invalid WS pin"));
        }
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("WS not a Pin type"));
    }

    // is SD valid?
    if (mp_obj_is_type(args[ARG_sd].u_obj, &pin_type)) {
        pin_af = pin_find_af(MP_OBJ_TO_PTR(args[ARG_sd].u_obj), AF_FN_I2S, self->i2s_id);
        if (pin_af->type != AF_PIN_TYPE_I2S_SD) {
            mp_raise_ValueError(MP_ERROR_TEXT("invalid SD pin"));
        }
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("SD not a Pin type"));
    }

    // is Mode valid?
    uint16_t i2s_mode = args[ARG_mode].u_int;
    if ((i2s_mode != (I2S_MODE_MASTER_RX)) &&
        (i2s_mode != (I2S_MODE_MASTER_TX))) {
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
        uint8_t *buffer = m_new(uint8_t, ring_buffer_len);
        self->ring_buffer_storage = buffer;
        ringbuf_init(&self->ring_buffer, buffer, ring_buffer_len);
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid ibuf"));
    }

    self->sck = MP_OBJ_TO_PTR(args[ARG_sck].u_obj);
    self->ws = MP_OBJ_TO_PTR(args[ARG_ws].u_obj);
    self->sd = MP_OBJ_TO_PTR(args[ARG_sd].u_obj);
    self->mode = i2s_mode;
    self->bits = i2s_bits;
    self->format = i2s_format;
    self->rate = args[ARG_rate].u_int;
    self->ibuf = ring_buffer_len;
    self->callback_for_non_blocking = MP_OBJ_NULL;
    self->non_blocking_descriptor.copy_in_progress = false;
    self->io_mode = BLOCKING;

    I2S_InitTypeDef *init = &self->hi2s.Init;
    init->Mode = i2s_mode;
    init->Standard = I2S_STANDARD_PHILIPS;
    init->DataFormat = get_dma_bits(self->mode, self->bits);
    init->MCLKOutput = I2S_MCLKOUTPUT_DISABLE;
    init->AudioFreq = args[ARG_rate].u_int;
    init->CPOL = I2S_CPOL_LOW;
    init->ClockSource = I2S_CLOCK_PLL;
    #if defined(STM32F4)
    init->FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;
    #endif

    // init the I2S bus
    if (!i2s_init(self)) {
        mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("I2S init failed"));
    }

    // start DMA.  DMA is configured to run continuously, using a circular buffer configuration
    uint32_t number_of_samples = 0;
    if (init->DataFormat == I2S_DATAFORMAT_16B) {
        number_of_samples = SIZEOF_DMA_BUFFER_IN_BYTES / sizeof(uint16_t);
    } else {  // 32 bits
        number_of_samples = SIZEOF_DMA_BUFFER_IN_BYTES / sizeof(uint32_t);
    }

    HAL_StatusTypeDef status;
    if (self->mode == I2S_MODE_MASTER_TX) {
        status = HAL_I2S_Transmit_DMA(&self->hi2s, (void *)self->dma_buffer_dcache_aligned, number_of_samples);
    } else {  // RX
        status = HAL_I2S_Receive_DMA(&self->hi2s, (void *)self->dma_buffer_dcache_aligned, number_of_samples);
    }

    if (status != HAL_OK) {
        mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("DMA init failed"));
    }
}

static machine_i2s_obj_t *mp_machine_i2s_make_new_instance(mp_int_t i2s_id) {
    uint8_t i2s_id_zero_base = 0;

    if (0) {
    #ifdef MICROPY_HW_I2S1
    } else if (i2s_id == 1) {
        i2s_id_zero_base = 0;
    #endif
    #ifdef MICROPY_HW_I2S2
    } else if (i2s_id == 2) {
        i2s_id_zero_base = 1;
    #endif
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid id"));
    }

    machine_i2s_obj_t *self;
    if (MP_STATE_PORT(machine_i2s_obj)[i2s_id_zero_base] == NULL) {
        self = mp_obj_malloc(machine_i2s_obj_t, &machine_i2s_type);
        MP_STATE_PORT(machine_i2s_obj)[i2s_id_zero_base] = self;
        self->i2s_id = i2s_id;
    } else {
        self = MP_STATE_PORT(machine_i2s_obj)[i2s_id_zero_base];
        machine_i2s_deinit(MP_OBJ_FROM_PTR(self));
    }

    // align DMA buffer start to the cache line size (32 bytes)
    self->dma_buffer_dcache_aligned = (uint8_t *)((uint32_t)(self->dma_buffer + 0x1f) & ~0x1f);

    return self;
}

static void mp_machine_i2s_deinit(machine_i2s_obj_t *self) {
    if (self->ring_buffer_storage != NULL) {
        dma_deinit(self->dma_descr_tx);
        dma_deinit(self->dma_descr_rx);
        HAL_I2S_DeInit(&self->hi2s);

        if (self->hi2s.Instance == I2S1) {
            __SPI1_FORCE_RESET();
            __SPI1_RELEASE_RESET();
            __SPI1_CLK_DISABLE();
        } else if (self->hi2s.Instance == I2S2) {
            __SPI2_FORCE_RESET();
            __SPI2_RELEASE_RESET();
            __SPI2_CLK_DISABLE();
        }

        m_free(self->ring_buffer_storage);
        self->ring_buffer_storage = NULL;
    }
}

static void mp_machine_i2s_irq_update(machine_i2s_obj_t *self) {
    (void)self;
}

MP_REGISTER_ROOT_POINTER(struct _machine_i2s_obj_t *machine_i2s_obj[MICROPY_HW_MAX_I2S]);
