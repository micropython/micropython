// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "bindings/espidf/__init__.h"

#include "common-hal/audioi2sin/I2SIn.h"
#include "py/runtime.h"
#include "shared-bindings/audioi2sin/I2SIn.h"
#include "shared-bindings/audiocore/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/audioi2sin/__init__.h"
#include "supervisor/port.h"

#include "driver/i2s_std.h"

#if CIRCUITPY_AUDIOI2SIN

void common_hal_audioi2sin_i2sin_construct(audioi2sin_i2sin_obj_t *self,
    const mcu_pin_obj_t *bit_clock, const mcu_pin_obj_t *word_select,
    const mcu_pin_obj_t *data, const mcu_pin_obj_t *main_clock,
    uint32_t sample_rate, uint8_t bit_depth, uint8_t output_bit_depth,
    bool mono, bool left_justified, bool samples_signed,
    bool external_clock) {
    if (external_clock) {
        mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q"), MP_QSTR_external_clock);
    }

    i2s_data_bit_width_t bit_width = (i2s_data_bit_width_t)bit_depth;

    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    esp_err_t err = i2s_new_channel(&chan_cfg, NULL, &self->rx_chan);
    if (err == ESP_ERR_NOT_FOUND) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Peripheral in use"));
    }
    CHECK_ESP_RESULT(err);

    // Always configure the bus as stereo. The newer-family I2S peripherals
    // (S2/S3/C-series) ignore I2S_SLOT_MODE_MONO on RX and write both slots
    // into the DMA buffer regardless, which yields buffers that fill at 2x
    // the WS rate and produces half-speed audio. By configuring stereo and
    // dropping one slot ourselves in record_to_buffer, behavior is uniform
    // across chips.
    i2s_std_slot_config_t slot_cfg = left_justified
        ? (i2s_std_slot_config_t)I2S_STD_MSB_SLOT_DEFAULT_CONFIG(bit_width, I2S_SLOT_MODE_STEREO)
        : (i2s_std_slot_config_t)I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(bit_width, I2S_SLOT_MODE_STEREO);

    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(sample_rate),
        .slot_cfg = slot_cfg,
        .gpio_cfg = {
            .mclk = main_clock != NULL ? main_clock->number : I2S_GPIO_UNUSED,
            .bclk = bit_clock->number,
            .ws = word_select->number,
            .dout = I2S_GPIO_UNUSED,
            .din = data->number,
        },
    };
    err = i2s_channel_init_std_mode(self->rx_chan, &std_cfg);
    if (err == ESP_OK) {
        err = i2s_channel_enable(self->rx_chan);
    }
    if (err != ESP_OK) {
        i2s_del_channel(self->rx_chan);
        self->rx_chan = NULL;
        CHECK_ESP_RESULT(err);
    }

    self->bit_clock = bit_clock;
    self->word_select = word_select;
    self->data = data;
    self->mclk = main_clock;
    self->sample_rate = sample_rate;
    self->bit_depth = bit_depth;
    self->mono = mono;
    self->samples_signed = samples_signed;

    // Populate the audiosample base so I2SIn can stream into the audio pipeline.
    // Streaming requires an 8- or 16-bit output depth (reset_buffer enforces it);
    // the owned conversion buffer is allocated lazily on first reset_buffer().
    uint8_t channel_count = mono ? 1 : 2;
    self->base.sample_rate = sample_rate;
    self->base.bits_per_sample = output_bit_depth;
    self->base.channel_count = channel_count;
    self->base.samples_signed = samples_signed;
    self->base.single_buffer = false;
    self->base.max_buffer_length =
        2 * AUDIOI2SIN_STREAM_FRAMES * (output_bit_depth / 8) * channel_count;
    self->output_buffer = NULL;
    self->output_half_bytes = self->base.max_buffer_length / 2;
    self->output_index = 0;

    claim_pin(bit_clock);
    claim_pin(word_select);
    claim_pin(data);
    if (main_clock) {
        claim_pin(main_clock);
    }
}

bool common_hal_audioi2sin_i2sin_deinited(audioi2sin_i2sin_obj_t *self) {
    return self->data == NULL;
}

void common_hal_audioi2sin_i2sin_deinit(audioi2sin_i2sin_obj_t *self) {
    if (common_hal_audioi2sin_i2sin_deinited(self)) {
        return;
    }

    if (self->rx_chan) {
        i2s_channel_disable(self->rx_chan);
        i2s_del_channel(self->rx_chan);
        self->rx_chan = NULL;
    }

    if (self->bit_clock) {
        reset_pin_number(self->bit_clock->number);
    }
    self->bit_clock = NULL;

    if (self->word_select) {
        reset_pin_number(self->word_select->number);
    }
    self->word_select = NULL;

    if (self->data) {
        reset_pin_number(self->data->number);
    }
    self->data = NULL;

    if (self->mclk) {
        reset_pin_number(self->mclk->number);
    }
    self->mclk = NULL;

    if (self->output_buffer != NULL) {
        port_free(self->output_buffer);
        self->output_buffer = NULL;
    }
    audiosample_mark_deinit(&self->base);
}

// Sign-extend a raw I2S sample (in the low `in_depth` bits of `raw`) to a
// canonical int32 value.
static inline int32_t i2sin_normalize_signed(uint32_t raw, uint8_t in_depth) {
    if (in_depth == 32) {
        return (int32_t)raw;
    }
    if (in_depth == 24) {
        uint32_t sign_bit = 0x800000u;
        return (int32_t)((raw ^ sign_bit) - sign_bit);
    }
    if (in_depth == 16) {
        return (int16_t)(raw & 0xffffu);
    }
    return (int8_t)(raw & 0xffu);
}

// Read a single sample from the DMA scratch at the given byte offset for the
// configured input bit depth.
static inline uint32_t i2sin_read_raw(const uint8_t *src, uint8_t in_depth) {
    if (in_depth == 8) {
        return (uint32_t)(*src);
    }
    if (in_depth == 16) {
        uint16_t v;
        memcpy(&v, src, sizeof(v));
        return v;
    }
    uint32_t v;
    memcpy(&v, src, sizeof(v));
    return v;
}

// Normalize `raw` from `in_depth` for this port's wire format, then hand off to
// the shared converter to rescale `in_depth` -> `out_depth` and write it to
// `buffer` at sample index `idx`. The depth conversion + unsigned-WAV flip +
// container store live in `shared_audioi2sin_write_converted` (shared with other
// ports); see that helper for the upscale/downscale semantics.
static inline void i2sin_write_converted(void *buffer, uint32_t idx,
    uint32_t raw, uint8_t in_depth, uint8_t out_depth, bool samples_signed) {
    int32_t s = i2sin_normalize_signed(raw, in_depth);
    shared_audioi2sin_write_converted(buffer, idx, s, in_depth, out_depth, samples_signed);
}

// I2S delivers signed PCM. When samples_signed is false, XOR each sample with
// the sign bit for its width to convert to unsigned PCM (WAV convention).
static void i2sin_convert_to_unsigned(void *buffer, uint32_t samples,
    uint8_t bit_depth, size_t element_size) {
    (void)element_size;
    uint32_t *p = (uint32_t *)buffer;

    if (bit_depth == 8) {
        // 4 samples per word
        uint32_t words = samples / 4;
        for (uint32_t i = 0; i < words; i++) {
            p[i] ^= 0x80808080u;
        }
        // tail: 0–3 leftover bytes
        uint8_t *tail = (uint8_t *)(p + words);
        for (uint32_t i = 0; i < (samples & 3u); i++) {
            tail[i] ^= 0x80u;
        }
    } else if (bit_depth == 16) {
        // 2 samples per word
        uint32_t words = samples / 2;
        for (uint32_t i = 0; i < words; i++) {
            p[i] ^= 0x80008000u;
        }
        if (samples & 1u) {
            ((uint16_t *)(p + words))[0] ^= 0x8000u;
        }
    } else {
        // 24- or 32-bit: one sample per 32-bit slot
        uint32_t mask = (bit_depth == 24) ? 0x00800000u : 0x80000000u;
        for (uint32_t i = 0; i < samples; i++) {
            p[i] ^= mask;
        }
    }
}


uint32_t common_hal_audioi2sin_i2sin_record_to_buffer(audioi2sin_i2sin_obj_t *self,
    void *buffer, uint32_t length) {
    size_t element_size = self->bit_depth / 8;
    // 24-bit samples occupy a 32-bit slot on the I2S bus.
    if (self->bit_depth == 24) {
        element_size = 4;
    }

    if (self->base.bits_per_sample != self->bit_depth) {
        // Bit-depth conversion path: always read at input width into scratch,
        // convert each sample into the user's buffer at output width.
        const uint8_t in_depth = self->bit_depth;
        const uint8_t out_depth = self->base.bits_per_sample;
        const bool samples_signed = self->samples_signed;
        uint8_t scratch[256];
        const size_t in_frame_bytes = 2 * element_size;
        const size_t scratch_frames = sizeof(scratch) / in_frame_bytes;
        uint32_t produced = 0;
        while (produced < length) {
            size_t want_frames;
            if (self->mono) {
                want_frames = length - produced;
            } else {
                want_frames = (length - produced + 1) / 2;
            }
            if (want_frames > scratch_frames) {
                want_frames = scratch_frames;
            }
            size_t got_bytes = 0;
            esp_err_t err = i2s_channel_read(self->rx_chan, scratch,
                want_frames * in_frame_bytes, &got_bytes, portMAX_DELAY);
            CHECK_ESP_RESULT(err);
            size_t got_frames = got_bytes / in_frame_bytes;
            for (size_t i = 0; i < got_frames && produced < length; i++) {
                const uint8_t *frame = scratch + i * in_frame_bytes;
                uint32_t left_raw = i2sin_read_raw(frame, in_depth);
                i2sin_write_converted(buffer, produced++, left_raw,
                    in_depth, out_depth, samples_signed);
                if (!self->mono && produced < length) {
                    uint32_t right_raw = i2sin_read_raw(frame + element_size, in_depth);
                    i2sin_write_converted(buffer, produced++, right_raw,
                        in_depth, out_depth, samples_signed);
                }
            }
            if (got_frames < want_frames) {
                break;
            }
        }
        return produced;
    }

    uint32_t produced;
    if (!self->mono) {
        size_t result = 0;
        esp_err_t err = i2s_channel_read(self->rx_chan, buffer, length * element_size,
            &result, portMAX_DELAY);
        CHECK_ESP_RESULT(err);
        produced = result / element_size;
    } else {
        // Mono: bus is configured stereo, so each WS frame yields two slots in
        // the DMA buffer. Read in chunks into a scratch and keep only the left
        // slot of each frame.
        uint8_t scratch[256];
        const size_t frame_bytes = 2 * element_size;
        const size_t scratch_frames = sizeof(scratch) / frame_bytes;
        uint8_t *out = (uint8_t *)buffer;
        produced = 0;
        while (produced < length) {
            size_t want_frames = length - produced;
            if (want_frames > scratch_frames) {
                want_frames = scratch_frames;
            }
            size_t got_bytes = 0;
            esp_err_t err = i2s_channel_read(self->rx_chan, scratch,
                want_frames * frame_bytes, &got_bytes, portMAX_DELAY);
            CHECK_ESP_RESULT(err);
            size_t got_frames = got_bytes / frame_bytes;
            for (size_t i = 0; i < got_frames; i++) {
                memcpy(out + produced * element_size,
                    scratch + i * frame_bytes,
                    element_size);
                produced++;
            }
            if (got_frames < want_frames) {
                break;
            }
        }
    }

    if (!self->samples_signed && produced > 0) {
        i2sin_convert_to_unsigned(buffer, produced, self->bit_depth, element_size);
    }
    return produced;
}

uint8_t common_hal_audioi2sin_i2sin_get_bit_depth(audioi2sin_i2sin_obj_t *self) {
    return self->bit_depth;
}

uint32_t common_hal_audioi2sin_i2sin_get_sample_rate(audioi2sin_i2sin_obj_t *self) {
    return self->sample_rate;
}

bool common_hal_audioi2sin_i2sin_get_samples_signed(audioi2sin_i2sin_obj_t *self) {
    return self->samples_signed;
}

// Always false here: this port hands the capture ring to the IDF driver, which
// drops frames internally without telling us. Detecting it would require
// registering an on_recv_q_ovf callback on the channel.
bool common_hal_audioi2sin_i2sin_get_overflow(audioi2sin_i2sin_obj_t *self) {
    (void)self;
    return false;
}

// Write `count` silence samples at output depth starting at sample index `idx`.
// For signed PCM silence is 0; for unsigned (WAV) it is mid-scale.
static void i2sin_fill_silence(void *buffer, uint32_t idx, uint32_t count,
    uint8_t out_depth, bool samples_signed) {
    if (out_depth == 8) {
        uint8_t v = samples_signed ? 0 : 0x80u;
        memset((uint8_t *)buffer + idx, v, count);
    } else { // 16-bit (the only other streamable width)
        uint16_t v = samples_signed ? 0 : 0x8000u;
        uint16_t *p = (uint16_t *)buffer + idx;
        for (uint32_t i = 0; i < count; i++) {
            p[i] = v;
        }
    }
}

// Non-blocking fill: read whatever frames are immediately available from the I2S
// driver and convert them into `buffer` (output depth, interleaved), padding the
// remainder with silence on underrun. The bus is configured stereo, so each WS
// frame yields two slots; for mono we keep the left slot. `out_depth` is always 8
// or 16 here (reset_buffer rejects 24/32). Runs in the output backend's refill
// (background callback) context, so i2s_channel_read is called with a 0 ms
// timeout and never blocks.
void common_hal_audioi2sin_i2sin_fill_buffer(audioi2sin_i2sin_obj_t *self,
    uint8_t *buffer, uint32_t frames) {
    const uint8_t in_depth = self->bit_depth;
    const uint8_t out_depth = self->base.bits_per_sample;
    const bool samples_signed = self->samples_signed;
    const bool stereo = !self->mono;
    const uint8_t channel_count = stereo ? 2 : 1;
    size_t element_size = (in_depth == 24) ? 4 : (in_depth / 8);
    const size_t in_frame_bytes = 2 * element_size; // bus is always stereo
    const uint32_t total_samples = frames * channel_count;

    uint8_t scratch[256];
    const size_t scratch_frames = sizeof(scratch) / in_frame_bytes;

    uint32_t produced = 0;
    while (produced < total_samples) {
        size_t want_frames = (total_samples - produced + channel_count - 1) / channel_count;
        if (want_frames > scratch_frames) {
            want_frames = scratch_frames;
        }
        size_t got_bytes = 0;
        // 0 ms timeout: return immediately with whatever is buffered.
        esp_err_t err = i2s_channel_read(self->rx_chan, scratch,
            want_frames * in_frame_bytes, &got_bytes, 0);
        if (err != ESP_OK && err != ESP_ERR_TIMEOUT) {
            break;
        }
        size_t got_frames = got_bytes / in_frame_bytes;
        if (got_frames == 0) {
            break; // underrun: pad with silence below
        }
        for (size_t i = 0; i < got_frames && produced < total_samples; i++) {
            const uint8_t *frame = scratch + i * in_frame_bytes;
            uint32_t left_raw = i2sin_read_raw(frame, in_depth);
            i2sin_write_converted(buffer, produced++, left_raw,
                in_depth, out_depth, samples_signed);
            if (stereo && produced < total_samples) {
                uint32_t right_raw = i2sin_read_raw(frame + element_size, in_depth);
                i2sin_write_converted(buffer, produced++, right_raw,
                    in_depth, out_depth, samples_signed);
            }
        }
        if (got_frames < want_frames) {
            break;
        }
    }
    if (produced < total_samples) {
        i2sin_fill_silence(buffer, produced, total_samples - produced,
            out_depth, samples_signed);
    }
}

void common_hal_audioi2sin_i2sin_reset_buffer(audioi2sin_i2sin_obj_t *self,
    bool single_channel_output, uint8_t channel) {
    (void)single_channel_output;
    (void)channel;
    // The audio pipeline only carries 8- or 16-bit samples; 24/32-bit modes can
    // still record() but cannot stream.
    if (self->base.bits_per_sample != 8 && self->base.bits_per_sample != 16) {
        mp_raise_ValueError_varg(
            MP_ERROR_TEXT("%q must be 8 or 16"), MP_QSTR_output_bit_depth);
    }
    if (self->output_buffer == NULL) {
        self->output_buffer = (uint8_t *)port_malloc(self->base.max_buffer_length, false);
        if (self->output_buffer == NULL) {
            m_malloc_fail(self->base.max_buffer_length);
        }
    }
    self->output_index = 0;
}

audioio_get_buffer_result_t common_hal_audioi2sin_i2sin_get_buffer(
    audioi2sin_i2sin_obj_t *self, bool single_channel_output, uint8_t channel,
    uint8_t **buffer, uint32_t *buffer_length) {
    uint32_t half = self->output_half_bytes;
    uint8_t *out = self->output_buffer + half * self->output_index;
    self->output_index = 1 - self->output_index;

    uint32_t bytes_per_sample = self->base.bits_per_sample / 8;
    uint32_t frames = half / (bytes_per_sample * self->base.channel_count);
    common_hal_audioi2sin_i2sin_fill_buffer(self, out, frames);

    if (single_channel_output) {
        out += (channel % self->base.channel_count) * bytes_per_sample;
    }
    *buffer = out;
    *buffer_length = half;
    // A live mic is an infinite stream; never report DONE or the backend stops.
    return GET_BUFFER_MORE_DATA;
}

#endif // CIRCUITPY_AUDIOI2SIN
