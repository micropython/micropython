/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Jeff Epler for Adafruit Industries
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

#include <math.h>
#include <string.h>

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/audiobusio/I2SOut.h"
#include "shared-bindings/audiobusio/I2SOut.h"
#include "shared-module/audiocore/__init__.h"
#include "supervisor/shared/tick.h"

#include "py/obj.h"
#include "py/runtime.h"

static audiobusio_i2sout_obj_t *instance;

struct { int16_t l, r;
} static_sample16 = {0x8000, 0x8000};
struct { uint8_t l1, r1, l2, r2;
} static_sample8 = {0x80, 0x80, 0x80, 0x80};

struct frequency_info { uint32_t RATIO;
                        uint32_t MCKFREQ;
                        int sample_rate;
                        float abserr;
};
struct ratio_info { uint32_t RATIO;
                    int16_t divisor;
                    bool can_16bit;
};
struct ratio_info ratios[] = {
    { I2S_CONFIG_RATIO_RATIO_32X,   32,  true },
    { I2S_CONFIG_RATIO_RATIO_48X,   48, false },
    { I2S_CONFIG_RATIO_RATIO_64X,   64,  true },
    { I2S_CONFIG_RATIO_RATIO_96X,   96,  true },
    { I2S_CONFIG_RATIO_RATIO_128X, 128,  true },
    { I2S_CONFIG_RATIO_RATIO_192X, 192,  true },
    { I2S_CONFIG_RATIO_RATIO_256X, 256,  true },
    { I2S_CONFIG_RATIO_RATIO_384X, 384,  true },
    { I2S_CONFIG_RATIO_RATIO_512X, 512,  true },
};

struct mclk_info { uint32_t MCKFREQ;
                   int divisor;
};
struct mclk_info mclks[] = {
    { I2S_CONFIG_MCKFREQ_MCKFREQ_32MDIV8,     8 },
    { I2S_CONFIG_MCKFREQ_MCKFREQ_32MDIV10,   10 },
    { I2S_CONFIG_MCKFREQ_MCKFREQ_32MDIV11,   11 },
    { I2S_CONFIG_MCKFREQ_MCKFREQ_32MDIV15,   15 },
    { I2S_CONFIG_MCKFREQ_MCKFREQ_32MDIV16,   16 },
    { I2S_CONFIG_MCKFREQ_MCKFREQ_32MDIV21,   21 },
    { I2S_CONFIG_MCKFREQ_MCKFREQ_32MDIV23,   23 },
    { I2S_CONFIG_MCKFREQ_MCKFREQ_32MDIV31,   31 },
    { I2S_CONFIG_MCKFREQ_MCKFREQ_32MDIV42,   42 },
    { I2S_CONFIG_MCKFREQ_MCKFREQ_32MDIV63,   63 },
    { I2S_CONFIG_MCKFREQ_MCKFREQ_32MDIV125, 125 },
};

static void calculate_ratio_info(uint32_t target_sample_rate, struct frequency_info *info,
    int ratio_index, int mclk_index) {
    info->RATIO = ratios[ratio_index].RATIO;
    info->MCKFREQ = mclks[mclk_index].MCKFREQ;
    info->sample_rate = 32000000
        / ratios[ratio_index].divisor / mclks[mclk_index].divisor;
    info->abserr = fabsf(1.0f * target_sample_rate - info->sample_rate)
        / target_sample_rate;
}

void choose_i2s_clocking(audiobusio_i2sout_obj_t *self, uint32_t sample_rate) {
    struct frequency_info best = {0, 0, 0, 1.0};
    for (size_t ri = 0; ri < sizeof(ratios) / sizeof(ratios[0]); ri++) {
        if (NRF_I2S->CONFIG.SWIDTH == I2S_CONFIG_SWIDTH_SWIDTH_16Bit
            && !ratios[ri].can_16bit) {
            continue;
        }

        for (size_t mi = 0; mi < sizeof(mclks) / sizeof(mclks[0]); mi++) {
            struct frequency_info info = {0, 0, 1.0};
            calculate_ratio_info(sample_rate, &info, ri, mi);
            if (info.abserr < best.abserr) {
                best = info;
            }
            #ifdef DEBUG_CLOCKING
            mp_printf(&mp_plat_print,
                "RATIO=%3d MCKFREQ=%08x rate=%d abserr=%.4f\n",
                info.RATIO, info.MCKFREQ, info.sample_rate,
                (double)info.abserr);
            #endif
        }
    }
    NRF_I2S->CONFIG.RATIO = best.RATIO;
    NRF_I2S->CONFIG.MCKFREQ = best.MCKFREQ;
    self->sample_rate = best.sample_rate;
}

static void i2s_buffer_fill(audiobusio_i2sout_obj_t *self) {
    void *buffer = self->buffers[self->next_buffer];
    void *buffer_start = buffer;
    NRF_I2S->TXD.PTR = (uintptr_t)buffer;
    self->next_buffer = !self->next_buffer;
    size_t bytesleft = self->buffer_length;

    while (!self->paused && !self->stopping && bytesleft) {
        if (self->sample_data == self->sample_end) {
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
                    break;
                }
            }
            if (get_buffer_result == GET_BUFFER_ERROR || sample_buffer_length == 0) {
                self->stopping = true;
                break;
            }
        }
        uint16_t bytecount = MIN(bytesleft, (size_t)(self->sample_end - self->sample_data));
        if (self->samples_signed) {
            memcpy(buffer, self->sample_data, bytecount);
        } else if (self->bytes_per_sample == 2) {
            uint16_t *bp = (uint16_t *)buffer;
            uint16_t *be = (uint16_t *)(buffer + bytecount);
            uint16_t *sp = (uint16_t *)self->sample_data;
            for (; bp < be;) {
                *bp++ = *sp++ + 0x8000;
            }
        } else {
            uint8_t *bp = (uint8_t *)buffer;
            uint8_t *be = (uint8_t *)(buffer + bytecount);
            uint8_t *sp = (uint8_t *)self->sample_data;
            for (; bp < be;) {
                *bp++ = *sp++ + 0x80;
            }
        }
        buffer += bytecount;
        self->sample_data += bytecount;
        bytesleft -= bytecount;
    }

    // Find the last frame of real audio data and replicate its samples until
    // you have 32 bits worth, which is the fundamental unit of nRF I2S DMA
    if (buffer != buffer_start) {
        if (self->bytes_per_sample == 1 && self->channel_count == 1) {
            // For 8-bit mono, 4 copies of the final sample are required
            self->hold_value = 0x01010101 * *(uint8_t *)(buffer - 1);
        } else if (self->bytes_per_sample == 2 && self->channel_count == 2) {
            // For 16-bit stereo, 1 copy of the final sample is required
            self->hold_value = *(uint32_t *)(buffer - 4);
        } else {
            // For 8-bit stereo and 16-bit mono, 2 copies of the final sample are required
            self->hold_value = 0x00010001 * *(uint16_t *)(buffer - 2);
        }
    }

    // Emulate pausing and stopping by filling the DMA buffer with copies of
    // the last sample.  This includes the case where this iteration of
    // i2s_buffer_fill exhausted a non-looping sample.
    if (self->paused || self->stopping) {
        if (self->stopping) {
            NRF_I2S->TASKS_STOP = 1;
            self->playing = false;
        }
        uint32_t *bp = (uint32_t *)buffer;
        uint32_t *be = (uint32_t *)(buffer + bytesleft);
        for (; bp != be;) {
            *bp++ = self->hold_value;
        }
        return;
    }
}

void common_hal_audiobusio_i2sout_construct(audiobusio_i2sout_obj_t *self,
    const mcu_pin_obj_t *bit_clock, const mcu_pin_obj_t *word_select,
    const mcu_pin_obj_t *data, bool left_justified) {
    if (instance) {
        mp_raise_RuntimeError(translate("Device in use"));
    }
    instance = self;

    claim_pin(bit_clock);
    claim_pin(word_select);
    claim_pin(data);

    NRF_I2S->PSEL.SCK = self->bit_clock_pin_number = bit_clock->number;
    NRF_I2S->PSEL.LRCK = self->word_select_pin_number = word_select->number;
    NRF_I2S->PSEL.SDOUT = self->data_pin_number = data->number;

    NRF_I2S->CONFIG.MODE = I2S_CONFIG_MODE_MODE_Master;
    NRF_I2S->CONFIG.RXEN = I2S_CONFIG_RXEN_RXEN_Disabled;
    NRF_I2S->CONFIG.TXEN = I2S_CONFIG_TXEN_TXEN_Enabled;
    NRF_I2S->CONFIG.MCKEN = I2S_CONFIG_MCKEN_MCKEN_Enabled;
    NRF_I2S->CONFIG.SWIDTH = I2S_CONFIG_SWIDTH_SWIDTH_16Bit;

    NRF_I2S->CONFIG.ALIGN = I2S_CONFIG_ALIGN_ALIGN_Left;
    NRF_I2S->CONFIG.FORMAT = left_justified ? I2S_CONFIG_FORMAT_FORMAT_Aligned
                                    : I2S_CONFIG_FORMAT_FORMAT_I2S;

    supervisor_enable_tick();
}

bool common_hal_audiobusio_i2sout_deinited(audiobusio_i2sout_obj_t *self) {
    return self->data_pin_number == 0xff;
}

void common_hal_audiobusio_i2sout_deinit(audiobusio_i2sout_obj_t *self) {
    if (common_hal_audiobusio_i2sout_deinited(self)) {
        return;
    }
    NRF_I2S->TASKS_STOP = 1;
    NRF_I2S->ENABLE = I2S_ENABLE_ENABLE_Disabled;
    reset_pin_number(self->bit_clock_pin_number);
    self->bit_clock_pin_number = 0xff;
    reset_pin_number(self->word_select_pin_number);
    self->word_select_pin_number = 0xff;
    reset_pin_number(self->data_pin_number);
    self->data_pin_number = 0xff;
    instance = NULL;
    supervisor_disable_tick();
}

void common_hal_audiobusio_i2sout_play(audiobusio_i2sout_obj_t *self,
    mp_obj_t sample, bool loop) {
    if (common_hal_audiobusio_i2sout_get_playing(self)) {
        common_hal_audiobusio_i2sout_stop(self);
    }

    self->sample = sample;
    self->loop = loop;
    uint32_t sample_rate = audiosample_sample_rate(sample);
    self->bytes_per_sample = audiosample_bits_per_sample(sample) / 8;

    uint32_t max_buffer_length;
    bool single_buffer, samples_signed;
    audiosample_get_buffer_structure(sample, /* single channel */ true,
        &single_buffer, &samples_signed, &max_buffer_length,
        &self->channel_count);
    self->single_buffer = single_buffer;
    self->samples_signed = samples_signed;


    NRF_I2S->CONFIG.SWIDTH = self->bytes_per_sample == 1
        ? I2S_CONFIG_SWIDTH_SWIDTH_8Bit
        : I2S_CONFIG_SWIDTH_SWIDTH_16Bit;
    NRF_I2S->CONFIG.CHANNELS = self->channel_count == 1
        ? I2S_CONFIG_CHANNELS_CHANNELS_Left
        : I2S_CONFIG_CHANNELS_CHANNELS_Stereo;

    choose_i2s_clocking(self, sample_rate);
    /* Allocate buffers based on a maximum duration
     * This duration was chosen empirically based on what would
     * cause os.listdir('') to cause stuttering.  It seems like a
     * rather long time.
     */
    enum { buffer_length_ms = 16 };
    self->buffer_length = sample_rate * buffer_length_ms
        * self->bytes_per_sample * self->channel_count / 1000;
    self->buffer_length = (self->buffer_length + 3) & ~3;
    self->buffers[0] = m_malloc(self->buffer_length, false);
    self->buffers[1] = m_malloc(self->buffer_length, false);


    audiosample_reset_buffer(self->sample, false, 0);

    self->next_buffer = 0;
    self->sample_data = self->sample_end = 0;
    self->playing = true;
    self->paused = false;
    self->stopping = false;
    i2s_buffer_fill(self);

    NRF_I2S->RXTXD.MAXCNT = self->buffer_length / 4;
    // Turn on the interrupt to the NVIC but not within the NVIC itself. This will wake the CPU and
    // keep it awake until it is serviced without triggering an interrupt handler.
    NRF_I2S->INTENSET = I2S_INTENSET_TXPTRUPD_Msk;
    NRF_I2S->ENABLE = I2S_ENABLE_ENABLE_Enabled;

    NRF_I2S->TASKS_START = 1;

    i2s_background();
}

void common_hal_audiobusio_i2sout_pause(audiobusio_i2sout_obj_t *self) {
    self->paused = true;
}

void common_hal_audiobusio_i2sout_resume(audiobusio_i2sout_obj_t *self) {
    self->paused = false;
}

bool common_hal_audiobusio_i2sout_get_paused(audiobusio_i2sout_obj_t *self) {
    return self->paused;
}

void common_hal_audiobusio_i2sout_stop(audiobusio_i2sout_obj_t *self) {
    NRF_I2S->TASKS_STOP = 1;
    self->stopping = true;
    NRF_I2S->INTENCLR = I2S_INTENSET_TXPTRUPD_Msk;
}

bool common_hal_audiobusio_i2sout_get_playing(audiobusio_i2sout_obj_t *self) {
    if (NRF_I2S->EVENTS_STOPPED) {
        self->playing = false;
        NRF_I2S->EVENTS_STOPPED = 0;
    }
    return self->playing;
}

void i2s_background(void) {
    if (NVIC_GetPendingIRQ(I2S_IRQn) && NRF_I2S->EVENTS_TXPTRUPD) {
        NRF_I2S->EVENTS_TXPTRUPD = 0;
        NVIC_ClearPendingIRQ(I2S_IRQn);
        if (instance) {
            i2s_buffer_fill(instance);
        } else {
            NRF_I2S->TASKS_STOP = 1;
        }
    }
}

void i2s_reset(void) {
    NRF_I2S->TASKS_STOP = 1;
    NRF_I2S->INTENCLR = I2S_INTENSET_TXPTRUPD_Msk;
    NRF_I2S->ENABLE = I2S_ENABLE_ENABLE_Disabled;
    NRF_I2S->PSEL.MCK = 0xFFFFFFFF;
    NRF_I2S->PSEL.SCK = 0xFFFFFFFF;
    NRF_I2S->PSEL.LRCK = 0xFFFFFFFF;
    NRF_I2S->PSEL.SDOUT = 0xFFFFFFFF;
    NRF_I2S->PSEL.SDIN = 0xFFFFFFFF;
    if (instance) {
        supervisor_disable_tick();
    }
    instance = NULL;
}
