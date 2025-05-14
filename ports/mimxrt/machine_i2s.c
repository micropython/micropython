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

// This file is never compiled standalone, it's included directly from
// extmod/machine_i2s.c via MICROPY_PY_MACHINE_I2S_INCLUDEFILE.

#include "py/mphal.h"
#include "dma_manager.h"

#include CLOCK_CONFIG_H
#include "fsl_iomuxc.h"
#include "fsl_dmamux.h"
#include "fsl_edma.h"
#include "fsl_sai.h"

// Notes on this port's specific implementation of I2S:
// - the DMA callback is used to implement the asynchronous background operations, for non-blocking mode
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
    TOP_HALF,
    BOTTOM_HALF
} ping_pong_t;

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

static mp_obj_t machine_i2s_deinit(mp_obj_t self_in);

// The frame map is used with the readinto() method to transform the audio sample data coming
// from DMA memory (32-bit stereo) to the format specified
// in the I2S constructor.  e.g.  16-bit mono
static const int8_t i2s_frame_map[NUM_I2S_USER_FORMATS][I2S_RX_FRAME_SIZE_IN_BYTES] = {
    {-1, -1,  0,  1, -1, -1, -1, -1 },  // Mono, 16-bits
    { 0,  1,  2,  3, -1, -1, -1, -1 },  // Mono, 32-bits
    {-1, -1,  0,  1, -1, -1,  2,  3 },  // Stereo, 16-bits
    { 0,  1,  2,  3,  4,  5,  6,  7 },  // Stereo, 32-bits
};

// 2 PLL configurations
// PLL output frequency = 24MHz * (.loopDivider + .numerator/.denominator)

// Configuration 1: for sampling frequencies [Hz]:  8000, 12000, 16000, 24000, 32000, 48000
// Clock frequency = 786,432,000 Hz = 48000 * 64 * 256
static const clock_audio_pll_config_t audioPllConfig_8000_48000 = {
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
static const clock_audio_pll_config_t audioPllConfig_11025_44100 = {
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
static const i2s_clock_config_t clock_config_map[] = {
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

static const clock_root_t i2s_clock_mux[] = I2S_CLOCK_MUX;
#else
// for 10xx the total division factor is given by (p + 1) * (d + 1)
static const i2s_clock_config_t clock_config_map[] = {
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

static const clock_mux_t i2s_clock_mux[] = I2S_CLOCK_MUX;
static const clock_div_t i2s_clock_pre_div[] = I2S_CLOCK_PRE_DIV;
static const clock_div_t i2s_clock_div[] = I2S_CLOCK_DIV;
static const iomuxc_gpr_mode_t i2s_iomuxc_gpr_mode[] = I2S_IOMUXC_GPR_MODE;
#endif

static const I2S_Type *i2s_base_ptr[] = I2S_BASE_PTRS;

static const dma_request_source_t i2s_dma_req_src_tx[] = I2S_DMA_REQ_SRC_TX;
static const dma_request_source_t i2s_dma_req_src_rx[] = I2S_DMA_REQ_SRC_RX;
static const gpio_map_t i2s_gpio_map[] = I2S_GPIO_MAP;
AT_NONCACHEABLE_SECTION_ALIGN(edma_tcd_t edmaTcd[MICROPY_HW_I2S_NUM], 32);

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

static bool lookup_gpio(const machine_pin_obj_t *pin, i2s_pin_function_t fn, uint8_t hw_id, uint16_t *index) {
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

static bool set_iomux(const machine_pin_obj_t *pin, i2s_pin_function_t fn, uint8_t hw_id) {
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

static bool is_rate_supported(int32_t rate) {
    for (uint16_t i = 0; i < ARRAY_SIZE(clock_config_map); i++) {
        if (clock_config_map[i].rate == rate) {
            return true;
        }
    }
    return false;
}

static const clock_audio_pll_config_t *get_pll_config(int32_t rate) {
    for (uint16_t i = 0; i < ARRAY_SIZE(clock_config_map); i++) {
        if (clock_config_map[i].rate == rate) {
            return clock_config_map[i].pll_config;
        }
    }
    return 0;
}

static const uint32_t get_clock_pre_divider(int32_t rate) {
    for (uint16_t i = 0; i < ARRAY_SIZE(clock_config_map); i++) {
        if (clock_config_map[i].rate == rate) {
            return clock_config_map[i].clock_pre_divider;
        }
    }
    return 0;
}

static const uint32_t get_clock_divider(int32_t rate) {
    for (uint16_t i = 0; i < ARRAY_SIZE(clock_config_map); i++) {
        if (clock_config_map[i].rate == rate) {
            return clock_config_map[i].clock_divider;
        }
    }
    return 0;
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

static void edma_i2s_callback(edma_handle_t *handle, void *userData, bool transferDone, uint32_t tcds) {
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

static bool i2s_init(machine_i2s_obj_t *self) {

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

    // continuous DMA operation is achieved using the scatter/gather feature, with one TCD linked back to itself
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

static void mp_machine_i2s_init_helper(machine_i2s_obj_t *self, mp_arg_val_t *args) {
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

static machine_i2s_obj_t *mp_machine_i2s_make_new_instance(mp_int_t i2s_id) {
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

    return self;
}

static void mp_machine_i2s_deinit(machine_i2s_obj_t *self) {
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
}

static void mp_machine_i2s_irq_update(machine_i2s_obj_t *self) {
    (void)self;
}

MP_REGISTER_ROOT_POINTER(struct _machine_i2s_obj_t *machine_i2s_obj[MICROPY_HW_I2S_NUM]);
