/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
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

#include <string.h>

#include "py/runtime.h"

#include "common-hal/audiobusio/__init__.h"
#include "shared-module/audiocore/__init__.h"

#define SAI_CLOCK_SOURCE_SELECT (2U)
#define SAI_CLOCK_SOURCE_DIVIDER (63U)
#define SAI_CLOCK_SOURCE_PRE_DIVIDER (0U)

#define SAI_CLOCK_FREQ (CLOCK_GetFreq(kCLOCK_AudioPllClk) / (SAI_CLOCK_SOURCE_DIVIDER + 1U) / \
    (SAI_CLOCK_SOURCE_PRE_DIVIDER + 1U))

#define AUDIO_BUFFER_FRAME_COUNT (128) // in uint32_t; there are 4, giving 2048 bytes. In all they hold 10ms @ stereo 16-bit 48kHz before all buffers drain

/*
 * AUDIO PLL setting: Frequency = Fref * (DIV_SELECT + NUM / DENOM)
 *                              = 24 * (32 + 96 / 125) = 24 * (32.768)
 *                              = 786.432 MHz = 48kHz * 16384
 *
 * This default clocking is used during initial configuration; it also works well for
 * frequencies that evenly divide 192kHz, such as 8/12/24/48kHz. However, it doesn't work
 * well for 44.1/22/11kHz, so there's the possibility of using a different
 * setting when playing a particular sample.
 */
const clock_audio_pll_config_t audioPllConfig = {
    .loopDivider = 32,  /* PLL loop divider. Valid range for DIV_SELECT divider value: 27~54. */
    .postDivider = 1,   /* Divider after the PLL, should only be 1, 2, 4, 8, 16. */
    .numerator = 96,    /* 30 bit numerator of fractional loop divider. */
    .denominator = 125, /* 30 bit denominator of fractional loop divider */
};

static I2S_Type *const i2s_instances[] = I2S_BASE_PTRS;
static uint8_t i2s_in_use, i2s_playing;

static I2S_Type *SAI_GetPeripheral(int idx) {
    if (idx < 0 || idx >= (int)MP_ARRAY_SIZE(i2s_instances)) {
        return NULL;
    }
    return i2s_instances[idx];
}

static int SAI_GetInstance(I2S_Type *peripheral) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(i2s_instances); i++) { if (peripheral == i2s_instances[i]) {
                                                                    return i;
                                                                }
    }
    return -1;
}

static bool i2s_clock_off(I2S_Type *peripheral) {
    int index = SAI_GetInstance(peripheral);
    switch (index) {
        #if defined(SAI0)
        case 0:
            CLOCK_DisableClock(kCLOCK_Sai0);
            return true;
        #endif
        #if defined(SAI1)
        case 1:
            CLOCK_DisableClock(kCLOCK_Sai1);
            return true;
        #endif
        #if defined(SAI2)
        case 2:
            CLOCK_DisableClock(kCLOCK_Sai2);
            return true;
        #endif
        #if defined(SAI3)
        case 3:
            CLOCK_DisableClock(kCLOCK_Sai3);
            return true;
        #endif
        #if defined(SAI4)
        case 4:
            CLOCK_DisableClock(kCLOCK_Sai4);
            return true;
        #endif
        #if defined(SAI5)
        case 5:
            CLOCK_DisableClock(kCLOCK_Sai5);
            return true;
        #endif
        #if defined(SAI6)
        case 6:
            CLOCK_DisableClock(kCLOCK_Sai6);
            return true;
        #endif
        #if defined(SAI7)
        case 7:
            CLOCK_DisableClock(kCLOCK_Sai7);
            return true;
        #endif
    }
    return false;
}

static bool i2s_clocking(I2S_Type *peripheral) {
    int index = SAI_GetInstance(peripheral);
    switch (index) {
        #if defined(SAI0)
        case 0:
            CLOCK_SetDiv(kCLOCK_Sai0PreDiv, SAI_CLOCK_SOURCE_PRE_DIVIDER);
            CLOCK_SetDiv(kCLOCK_Sai0Div, SAI_CLOCK_SOURCE_DIVIDER);
            CLOCK_SetMux(kCLOCK_Sai0Mux, SAI_CLOCK_SOURCE_SELECT);
            CLOCK_EnableClock(kCLOCK_Sai0);
            return true;
        #endif
        #if defined(SAI1)
        case 1:
            CLOCK_SetDiv(kCLOCK_Sai1PreDiv, SAI_CLOCK_SOURCE_PRE_DIVIDER);
            CLOCK_SetDiv(kCLOCK_Sai1Div, SAI_CLOCK_SOURCE_DIVIDER);
            CLOCK_SetMux(kCLOCK_Sai1Mux, SAI_CLOCK_SOURCE_SELECT);
            CLOCK_EnableClock(kCLOCK_Sai1);
            return true;
        #endif
        #if defined(SAI2)
        case 2:
            CLOCK_SetDiv(kCLOCK_Sai2PreDiv, SAI_CLOCK_SOURCE_PRE_DIVIDER);
            CLOCK_SetDiv(kCLOCK_Sai2Div, SAI_CLOCK_SOURCE_DIVIDER);
            CLOCK_SetMux(kCLOCK_Sai2Mux, SAI_CLOCK_SOURCE_SELECT);
            CLOCK_EnableClock(kCLOCK_Sai2);
            return true;
        #endif
        #if defined(SAI3)
        case 3:
            CLOCK_SetDiv(kCLOCK_Sai3PreDiv, SAI_CLOCK_SOURCE_PRE_DIVIDER);
            CLOCK_SetDiv(kCLOCK_Sai3Div, SAI_CLOCK_SOURCE_DIVIDER);
            CLOCK_SetMux(kCLOCK_Sai3Mux, SAI_CLOCK_SOURCE_SELECT);
            CLOCK_EnableClock(kCLOCK_Sai3);
            return true;
        #endif
        #if defined(SAI4)
        case 4:
            CLOCK_SetDiv(kCLOCK_Sai4PreDiv, SAI_CLOCK_SOURCE_PRE_DIVIDER);
            CLOCK_SetDiv(kCLOCK_Sai4Div, SAI_CLOCK_SOURCE_DIVIDER);
            CLOCK_SetMux(kCLOCK_Sai4Mux, SAI_CLOCK_SOURCE_SELECT);
            CLOCK_EnableClock(kCLOCK_Sai4);
            return true;
        #endif
        #if defined(SAI5)
        case 5:
            CLOCK_SetDiv(kCLOCK_Sai5PreDiv, SAI_CLOCK_SOURCE_PRE_DIVIDER);
            CLOCK_SetDiv(kCLOCK_Sai5Div, SAI_CLOCK_SOURCE_DIVIDER);
            CLOCK_SetMux(kCLOCK_Sai5Mux, SAI_CLOCK_SOURCE_SELECT);
            CLOCK_EnableClock(kCLOCK_Sai5);
            return true;
        #endif
        #if defined(SAI6)
        case 6:
            CLOCK_SetDiv(kCLOCK_Sai6PreDiv, SAI_CLOCK_SOURCE_PRE_DIVIDER);
            CLOCK_SetDiv(kCLOCK_Sai6Div, SAI_CLOCK_SOURCE_DIVIDER);
            CLOCK_SetMux(kCLOCK_Sai6Mux, SAI_CLOCK_SOURCE_SELECT);
            CLOCK_EnableClock(kCLOCK_Sai6);
            return true;
        #endif
        #if defined(SAI7)
        case 7:
            CLOCK_SetDiv(kCLOCK_Sai7PreDiv, SAI_CLOCK_SOURCE_PRE_DIVIDER);
            CLOCK_SetDiv(kCLOCK_Sai7Div, SAI_CLOCK_SOURCE_DIVIDER);
            CLOCK_SetMux(kCLOCK_Sai7Mux, SAI_CLOCK_SOURCE_SELECT);
            CLOCK_EnableClock(kCLOCK_Sai7);
            return true;
        #endif
    }
    return false;
}


static bool i2s_queue_available(i2s_t *self) {
    return !self->handle.saiQueue[self->handle.queueUser].data;
}

static void i2s_fill_buffer(i2s_t *self) {
    if (!self->peripheral) {
        return;
    }
    while (i2s_queue_available(self)) {
        uint32_t *buffer = self->buffers[self->buffer_idx];
        uint32_t *ptr = buffer, *end = buffer + AUDIO_BUFFER_FRAME_COUNT;
        self->buffer_idx = (self->buffer_idx + 1) % SAI_XFER_QUEUE_SIZE;

        while (self->playing && !self->paused && ptr < end) {
            if (self->sample_data == self->sample_end) {
                if (self->stopping) {
                    // non-looping sample, previously returned GET_BUFFER_DONE
                    self->playing = false;
                    break;
                }
                uint32_t sample_buffer_length;
                audioio_get_buffer_result_t get_buffer_result =
                    audiosample_get_buffer(self->sample, false, 0,
                        &self->sample_data, &sample_buffer_length);
                self->sample_end = self->sample_data + sample_buffer_length;
                if (get_buffer_result == GET_BUFFER_DONE) {
                    if (self->loop) {
                        audiosample_reset_buffer(self->sample, false, 0);
                    } else {
                        self->stopping = true;
                    }
                }
                if (get_buffer_result == GET_BUFFER_ERROR || sample_buffer_length == 0) {
                    self->stopping = true;
                }
            }
            size_t input_bytecount = self->sample_end - self->sample_data;
            size_t bytes_per_input_frame = self->channel_count * self->bytes_per_sample;
            size_t framecount = MIN((size_t)(end - ptr), input_bytecount / bytes_per_input_frame);

#define SAMPLE_TYPE(is_signed, channel_count, bytes_per_sample) ((is_signed) | ((channel_count) << 1) | ((bytes_per_sample) << 3))

            switch (SAMPLE_TYPE(self->samples_signed, self->channel_count, self->bytes_per_sample)) {

                case SAMPLE_TYPE(true, 2, 2):
                    memcpy(ptr, self->sample_data, 4 * framecount);
                    break;

                case SAMPLE_TYPE(false, 2, 2):
                    audiosample_convert_u16s_s16s((int16_t *)ptr, (uint16_t *)(void *)self->sample_data, framecount);
                    break;

                case SAMPLE_TYPE(true, 1, 2):
                    audiosample_convert_s16m_s16s((int16_t *)ptr, (int16_t *)(void *)self->sample_data, framecount);
                    break;

                case SAMPLE_TYPE(false, 1, 2):
                    audiosample_convert_u16m_s16s((int16_t *)ptr, (uint16_t *)(void *)self->sample_data, framecount);
                    break;

                case SAMPLE_TYPE(true, 2, 1):
                    audiosample_convert_s8s_s16s((int16_t *)ptr, (int8_t *)(void *)self->sample_data, framecount);
                    memcpy(ptr, self->sample_data, 4 * framecount);
                    break;

                case SAMPLE_TYPE(false, 2, 1):
                    audiosample_convert_u8s_s16s((int16_t *)ptr, (uint8_t *)(void *)self->sample_data, framecount);
                    break;

                case SAMPLE_TYPE(true, 1, 1):
                    audiosample_convert_s8m_s16s((int16_t *)ptr, (int8_t *)(void *)self->sample_data, framecount);
                    break;

                case SAMPLE_TYPE(false, 1, 1):
                    audiosample_convert_u8m_s16s((int16_t *)ptr, (uint8_t *)(void *)self->sample_data, framecount);
                    break;
            }
            self->sample_data += bytes_per_input_frame * framecount; // in bytes
            ptr += framecount; // in frames
        }
        // Fill any remaining portion of the buffer with 'no sound'
        memset(ptr, 0, (end - ptr) * sizeof(uint32_t));
        sai_transfer_t xfer = {
            .data = (uint8_t *)buffer,
            .dataSize = AUDIO_BUFFER_FRAME_COUNT * sizeof(uint32_t),
        };
        int r = SAI_TransferSendNonBlocking(self->peripheral, &self->handle, &xfer);
        if (r != kStatus_Success) {
            mp_printf(&mp_plat_print, "transfer returned %d\n", (int)r);
        }
    }
}

static void i2s_callback_fun(void *self_in) {
    i2s_t *self = self_in;
    i2s_fill_buffer(self);
}

static void i2s_transfer_callback(I2S_Type *base, sai_handle_t *handle, status_t status, void *self_in) {
    i2s_t *self = self_in;
    if (status == kStatus_SAI_TxIdle) {
        // a block has been finished
        background_callback_add(&self->callback, i2s_callback_fun, self_in);
    }
}


void port_i2s_initialize(i2s_t *self, int instance, sai_transceiver_t *config) {
    if (!i2s_in_use) {
        // need to set audio pll up!

        /* DeInit Audio PLL. */
        CLOCK_DeinitAudioPll();
        /* Bypass Audio PLL. */
        CLOCK_SetPllBypass(CCM_ANALOG, kCLOCK_PllAudio, 1);
        /* Set divider for Audio PLL. */
        CCM_ANALOG->MISC2 &= ~CCM_ANALOG_MISC2_AUDIO_DIV_LSB_MASK;
        CCM_ANALOG->MISC2 &= ~CCM_ANALOG_MISC2_AUDIO_DIV_MSB_MASK;
        /* Enable Audio PLL output. */
        CCM_ANALOG->PLL_AUDIO |= CCM_ANALOG_PLL_AUDIO_ENABLE_MASK;

        CLOCK_InitAudioPll(&audioPllConfig);
    }

    I2S_Type *peripheral = SAI_GetPeripheral(instance);
    if (!peripheral) {
        mp_raise_ValueError_varg(translate("Invalid %q"), MP_QSTR_I2SOut);
    }
    if (i2s_in_use & (1 << instance)) {
        mp_raise_ValueError_varg(translate("%q in use"), MP_QSTR_I2SOut);
    }
    if (!i2s_clocking(peripheral)) {
        mp_raise_ValueError_varg(translate("Invalid %q"), MP_QSTR_I2SOut);
    }
    for (size_t i = 0; i < MP_ARRAY_SIZE(self->buffers); i++) {
        self->buffers[i] = m_malloc(AUDIO_BUFFER_FRAME_COUNT * sizeof(uint32_t));
    }
    self->peripheral = peripheral;
    SAI_Init(self->peripheral);
    SAI_TransferTxCreateHandle(peripheral, &self->handle, i2s_transfer_callback, (void *)self);
    SAI_TransferTxSetConfig(peripheral, &self->handle, config);
    self->sample_rate = 0;
    i2s_in_use |= (1 << instance);
}

bool port_i2s_deinited(i2s_t *self) {
    return !self->peripheral;
}

void port_i2s_deinit(i2s_t *self) {
    if (port_i2s_deinited(self)) {
        return;
    }
    SAI_TransferAbortSend(self->peripheral, &self->handle);
    i2s_clock_off(self->peripheral);

    uint32_t instance_mask = 1 << SAI_GetInstance(self->peripheral);
    i2s_in_use &= ~instance_mask;
    i2s_playing &= ~instance_mask;

    if (!i2s_in_use) {
        CCM_ANALOG->PLL_AUDIO = CCM_ANALOG_PLL_AUDIO_BYPASS_MASK | CCM_ANALOG_PLL_AUDIO_POWERDOWN_MASK | CCM_ANALOG_PLL_AUDIO_BYPASS_CLK_SRC(kCLOCK_PllClkSrc24M);
    }
    self->peripheral = NULL;
    for (size_t i = 0; i < MP_ARRAY_SIZE(self->buffers); i++) {
        self->buffers[i] = NULL;
    }
}

static uint32_t gcd(uint32_t a, uint32_t b) {
    while (b) {
        uint32_t tmp = a % b;
        a = b;
        b = tmp;
    }
    return a;
}

static void set_sai_clocking_for_sample_rate(uint32_t sample_rate) {
    mp_arg_validate_int_range((mp_uint_t)sample_rate, 4000, 192000, MP_QSTR_sample_rate);

    uint32_t target_rate = sample_rate;
    // ensure the PWM rate of MQS will be adequately high
    while (target_rate < 175000) {
        target_rate <<= 1;
    }
    target_rate *= 4096; // various prescalers divide by this much
    uint32_t div = gcd(target_rate % 24000000, 24000000);
    clock_audio_pll_config_t config = {
        .loopDivider = target_rate / 24000000,
        .postDivider = 1,
        .numerator = (target_rate % 24000000) / div,
        .denominator = 24000000 / div,
    };
    CLOCK_InitAudioPll(&config);
}

void port_i2s_play(i2s_t *self, mp_obj_t sample, bool loop) {
    self->sample = sample;
    self->loop = loop;
    self->bytes_per_sample = audiosample_bits_per_sample(sample) / 8;
    self->channel_count = audiosample_channel_count(sample);
    int instance = SAI_GetInstance(self->peripheral);
    i2s_playing |= (1 << instance);
    uint32_t sample_rate = audiosample_sample_rate(sample);
    if (sample_rate != self->sample_rate) {
        if (__builtin_popcount(i2s_playing) <= 1) {
            // as this is the first/only i2s instance playing audio, we can
            // safely change the overall clock used by the SAI peripheral, to
            // get more accurate frequency reproduction. If another i2s
            // instance is playing, then we can't touch the audio PLL and have
            // to live with what we can get, which may be inaccurate
            set_sai_clocking_for_sample_rate(sample_rate);
        }
        SAI_TxSetBitClockRate(self->peripheral, SAI_CLOCK_FREQ, sample_rate, 16, 2);
        self->sample_rate = sample_rate;
    }
    bool single_buffer;
    bool samples_signed;
    uint32_t max_buffer_length;
    uint8_t spacing;
    audiosample_get_buffer_structure(sample, false, &single_buffer, &samples_signed,
        &max_buffer_length, &spacing);
    self->samples_signed = samples_signed;
    self->playing = true;
    self->paused = false;
    self->stopping = false;
    self->sample_data = self->sample_end = NULL;

    audiosample_reset_buffer(self->sample, false, 0);

// TODO
    #if 0
    uint32_t sample_rate = audiosample_sample_rate(sample);
    if (sample_rate != self->i2s_config.sample_rate) {
        CHECK_ESP_RESULT(i2s_set_sample_rates(self->instance, audiosample_sample_rate(sample)));
        self->i2s_config.sample_rate = sample_rate;
    }
    #endif
    background_callback_add(&self->callback, i2s_callback_fun, self);
}

bool port_i2s_get_playing(i2s_t *self) {
    return self->playing;
}

bool port_i2s_get_paused(i2s_t *self) {
    return self->paused;
}

void port_i2s_stop(i2s_t *self) {
    self->sample = NULL;
    self->paused = false;
    self->playing = false;
    self->stopping = false;
}

void port_i2s_pause(i2s_t *self) {
    self->paused = true;
}

void port_i2s_resume(i2s_t *self) {
    self->paused = false;
}

void i2s_reset() {
// this port relies on object finalizers for reset
}
