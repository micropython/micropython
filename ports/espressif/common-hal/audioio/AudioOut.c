
#include <stdio.h>

#include "py/runtime.h"

#include "common-hal/audioio/AudioOut.h"
#include "shared-bindings/audioio/AudioOut.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/audiocore/__init__.h"

#include "driver/dac_continuous.h"

#if defined(CONFIG_IDF_TARGET_ESP32)
#define pin_CHANNEL_0 pin_GPIO25
#define pin_CHANNEL_1 pin_GPIO26
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
#define pin_CHANNEL_0 pin_GPIO17
#define pin_CHANNEL_1 pin_GPIO18
#endif

static dac_continuous_handle_t _active_handle;

#define INCREMENT_BUF_IDX(idx) ((idx + 1) % (NUM_DMA_BUFFERS + 1))

static bool audioout_convert_noop(
    void *in_buffer,
    size_t in_buffer_size,
    uint8_t **out_buffer,
    uint32_t *out_buffer_size) {

    *out_buffer = in_buffer;
    *out_buffer_size = in_buffer_size;
    return false;
}

static bool audioout_convert_u8s_u8m(
    void *in_buffer,
    size_t in_buffer_size,
    uint8_t **out_buffer,
    uint32_t *out_buffer_size) {

    bool buffer_changed = false;
    if (in_buffer_size / 2 > *out_buffer_size) {
        *out_buffer = m_malloc_without_collect(in_buffer_size / 2);
        buffer_changed = true;
    }
    audiosample_convert_u8s_u8m(*out_buffer, (uint8_t *)in_buffer, in_buffer_size / 2);
    *out_buffer_size = in_buffer_size / 2;
    return buffer_changed;
}

static bool audioout_convert_u8m_u8s(
    void *in_buffer,
    size_t in_buffer_size,
    uint8_t **out_buffer,
    uint32_t *out_buffer_size) {

    bool buffer_changed = false;
    if (in_buffer_size * 2 > *out_buffer_size) {
        *out_buffer = m_malloc_without_collect(in_buffer_size * 2);
        buffer_changed = true;
    }
    audiosample_convert_u8m_u8s(*out_buffer, (uint8_t *)in_buffer, in_buffer_size);
    *out_buffer_size = in_buffer_size * 2;
    return buffer_changed;
}

static bool audioout_convert_s8m_u8m(
    void *in_buffer,
    size_t in_buffer_size,
    uint8_t **out_buffer,
    uint32_t *out_buffer_size) {

    bool buffer_changed = false;
    if (in_buffer_size > *out_buffer_size) {
        *out_buffer = m_malloc_without_collect(in_buffer_size);
        buffer_changed = true;
    }
    audiosample_convert_s8m_u8m(*out_buffer, (int8_t *)in_buffer, in_buffer_size);
    *out_buffer_size = in_buffer_size;
    return buffer_changed;
}

static bool audioout_convert_s8s_u8m(
    void *in_buffer,
    size_t in_buffer_size,
    uint8_t **out_buffer,
    uint32_t *out_buffer_size) {

    bool buffer_changed = false;
    if (in_buffer_size / 2 > *out_buffer_size) {
        *out_buffer = m_malloc_without_collect(in_buffer_size / 2);
        buffer_changed = true;
    }
    audiosample_convert_s8s_u8m(*out_buffer, (int8_t *)in_buffer, in_buffer_size / 2);
    *out_buffer_size = in_buffer_size / 2;
    return buffer_changed;
}

static bool audioout_convert_s8m_u8s(
    void *in_buffer,
    size_t in_buffer_size,
    uint8_t **out_buffer,
    uint32_t *out_buffer_size) {

    bool buffer_changed = false;
    if (in_buffer_size * 2 > *out_buffer_size) {
        *out_buffer = m_malloc_without_collect(in_buffer_size * 2);
        buffer_changed = true;
    }
    audiosample_convert_s8m_u8s(*out_buffer, (int8_t *)in_buffer, in_buffer_size);
    *out_buffer_size = in_buffer_size * 2;
    return buffer_changed;
}

static bool audioout_convert_s8s_u8s(
    void *in_buffer,
    size_t in_buffer_size,
    uint8_t **out_buffer,
    uint32_t *out_buffer_size) {

    bool buffer_changed = false;
    if (in_buffer_size > *out_buffer_size) {
        *out_buffer = m_malloc_without_collect(in_buffer_size);
        buffer_changed = true;
    }
    audiosample_convert_s8s_u8s(*out_buffer, (int8_t *)in_buffer, in_buffer_size);
    *out_buffer_size = in_buffer_size;
    return buffer_changed;
}

static bool audioout_convert_u16m_u8m(
    void *in_buffer,
    size_t in_buffer_size,
    uint8_t **out_buffer,
    uint32_t *out_buffer_size) {

    bool buffer_changed = false;
    if (in_buffer_size / 2 > *out_buffer_size) {
        *out_buffer = m_malloc_without_collect(in_buffer_size / 2);
        buffer_changed = true;
    }
    audiosample_convert_u16m_u8m(*out_buffer, (uint16_t *)in_buffer, in_buffer_size / 2);
    *out_buffer_size = in_buffer_size / 2;
    return buffer_changed;
}

static bool audioout_convert_u16m_u8s(
    void *in_buffer,
    size_t in_buffer_size,
    uint8_t **out_buffer,
    uint32_t *out_buffer_size) {

    bool buffer_changed = false;
    if (in_buffer_size > *out_buffer_size) {
        *out_buffer = m_malloc_without_collect(in_buffer_size);
        buffer_changed = true;
    }
    audiosample_convert_u16m_u8s(*out_buffer, (uint16_t *)in_buffer, in_buffer_size / 2);
    *out_buffer_size = in_buffer_size;
    return buffer_changed;
}

static bool audioout_convert_u16s_u8m(
    void *in_buffer,
    size_t in_buffer_size,
    uint8_t **out_buffer,
    uint32_t *out_buffer_size) {

    bool buffer_changed = false;
    if (in_buffer_size / 4 > *out_buffer_size) {
        *out_buffer = m_malloc(in_buffer_size / 4);
        buffer_changed = true;
    }
    audiosample_convert_u16s_u8m(*out_buffer, (uint16_t *)in_buffer, in_buffer_size / 4);
    *out_buffer_size = in_buffer_size / 4;
    return buffer_changed;
}

static bool audioout_convert_u16s_u8s(
    void *in_buffer,
    size_t in_buffer_size,
    uint8_t **out_buffer,
    uint32_t *out_buffer_size) {

    bool buffer_changed = false;
    if (in_buffer_size / 2 > *out_buffer_size) {
        *out_buffer = m_malloc_without_collect(in_buffer_size / 2);
        buffer_changed = true;
    }
    audiosample_convert_u16s_u8s(*out_buffer, (uint16_t *)in_buffer, in_buffer_size / 4);
    *out_buffer_size = in_buffer_size / 2;
    return buffer_changed;
}

static bool audioout_convert_s16m_u8m(
    void *in_buffer,
    size_t in_buffer_size,
    uint8_t **out_buffer,
    uint32_t *out_buffer_size) {

    bool buffer_changed = false;
    if (in_buffer_size / 2 > *out_buffer_size) {
        *out_buffer = m_malloc_without_collect(in_buffer_size / 2);
        buffer_changed = true;
    }
    audiosample_convert_s16m_u8m(*out_buffer, (int16_t *)in_buffer, in_buffer_size / 2);
    *out_buffer_size = in_buffer_size / 2;
    return buffer_changed;
}

static bool audioout_convert_s16m_u8s(
    void *in_buffer,
    size_t in_buffer_size,
    uint8_t **out_buffer,
    uint32_t *out_buffer_size) {

    bool buffer_changed = false;
    if (in_buffer_size > *out_buffer_size) {
        *out_buffer = m_malloc_without_collect(in_buffer_size);
        buffer_changed = true;
    }
    audiosample_convert_s16m_u8s(*out_buffer, (int16_t *)in_buffer, in_buffer_size / 2);
    *out_buffer_size = in_buffer_size;
    return buffer_changed;
}

static bool audioout_convert_s16s_u8m(
    void *in_buffer,
    size_t in_buffer_size,
    uint8_t **out_buffer,
    uint32_t *out_buffer_size) {

    bool buffer_changed = false;
    if (in_buffer_size / 4 > *out_buffer_size) {
        *out_buffer = m_malloc(in_buffer_size / 4);
        buffer_changed = true;
    }
    audiosample_convert_s16s_u8m(*out_buffer, (int16_t *)in_buffer, in_buffer_size / 4);
    *out_buffer_size = in_buffer_size / 4;
    return buffer_changed;
}

static bool audioout_convert_s16s_u8s(
    void *in_buffer,
    size_t in_buffer_size,
    uint8_t **out_buffer,
    uint32_t *out_buffer_size) {

    bool buffer_changed = false;
    if (in_buffer_size / 2 > *out_buffer_size) {
        *out_buffer = m_malloc_without_collect(in_buffer_size / 2);
        buffer_changed = true;
    }
    audiosample_convert_s16s_u8s(*out_buffer, (int16_t *)in_buffer, in_buffer_size / 4);
    *out_buffer_size = in_buffer_size / 2;
    return buffer_changed;
}

#define CONV_MATCH(bps, sign, ichans, ochans) ((bps & 0xf) | ((sign & 0x1) << 4) | ((ichans & 0x3) << 5) | ((ochans & 0x3) << 7))

static audioout_sample_convert_func_t audioout_get_samples_convert_func(
    size_t in_bits_per_sample,
    int in_channels,
    bool in_signed,
    int out_channels) {

    switch CONV_MATCH(in_bits_per_sample, in_signed, in_channels, out_channels) {
        case CONV_MATCH(8, false, 1, 1):
        case CONV_MATCH(8, false, 2, 2):
            return audioout_convert_noop;
        case CONV_MATCH(8, false, 2, 1):
            return audioout_convert_u8s_u8m;
        case CONV_MATCH(8, false, 1, 2):
            return audioout_convert_u8m_u8s;
        case CONV_MATCH(8, true, 1, 1):
            return audioout_convert_s8m_u8m;
        case CONV_MATCH(8, true, 2, 1):
            return audioout_convert_s8s_u8m;
        case CONV_MATCH(8, true, 1, 2):
            return audioout_convert_s8m_u8s;
        case CONV_MATCH(8, true, 2, 2):
            return audioout_convert_s8s_u8s;
        case CONV_MATCH(16, false, 1, 1):
            return audioout_convert_u16m_u8m;
        case CONV_MATCH(16, false, 1, 2):
            return audioout_convert_u16m_u8s;
        case CONV_MATCH(16, false, 2, 1):
            return audioout_convert_u16s_u8m;
        case CONV_MATCH(16, false, 2, 2):
            return audioout_convert_u16s_u8s;
        case CONV_MATCH(16, true, 1, 1):
            return audioout_convert_s16m_u8m;
        case CONV_MATCH(16, true, 1, 2):
            return audioout_convert_s16m_u8s;
        case CONV_MATCH(16, true, 2, 1):
            return audioout_convert_s16s_u8m;
        case CONV_MATCH(16, true, 2, 2):
            return audioout_convert_s16s_u8s;
        default:
            mp_raise_RuntimeError(MP_ERROR_TEXT("audio format not supported"));
    }
}

static void audioio_audioout_start(audioio_audioout_obj_t *self) {
    esp_err_t ret;

    self->playing = true;
    self->paused = false;

    ret = dac_continuous_start_async_writing(self->handle);
    if (ret != ESP_OK) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Failed to start async audio"));
    }
}

static void audioio_audioout_stop(audioio_audioout_obj_t *self, bool full_stop) {
    dac_continuous_stop_async_writing(self->handle);
    if (full_stop) {
        self->get_buffer_index = 0;
        self->put_buffer_index = 0;
        self->sample_buffer = NULL;
        self->sample = NULL;
        self->playing = false;
        self->paused = false;
    } else {
        self->paused = true;
    }
}

static bool audioout_fill_buffer(audioio_audioout_obj_t *self) {
    if (!self->playing) {
        return false;
    }

    audioio_get_buffer_result_t get_buffer_result;

    uint8_t dma_buf_idx = self->get_buffer_index;

    if (dma_buf_idx == self->put_buffer_index) {
        return false;
    }

    uint8_t *dma_buf = self->dma_buffers[dma_buf_idx].ptr;
    size_t dma_buf_size = self->dma_buffers[dma_buf_idx].size;

    self->get_buffer_index = INCREMENT_BUF_IDX(dma_buf_idx);

    bool single_channel_output = true; // whether or not we have 1 or 2 output channels
    uint8_t channel = 0; // which channel right now?
    uint8_t *raw_sample_buf; // raw audio sample buffer
    uint32_t raw_sample_buf_size; // raw audio sample buffer len
    uint8_t *sample_buf = self->scratch_buffer; // converted audio sample buffer
    uint32_t sample_buf_size = self->scratch_buffer_size; // converted audio sample buffer len
    size_t bytes_loaded;
    esp_err_t ret;

    if (self->sample_buffer != NULL && self->sample_buffer_size > 0) {
        sample_buf = self->sample_buffer;
        sample_buf_size = self->sample_buffer_size;
        get_buffer_result = self->sample_buffer_result;
    } else {
        get_buffer_result = audiosample_get_buffer(self->sample,
            single_channel_output,
            channel,
            &raw_sample_buf, &raw_sample_buf_size);

        if (get_buffer_result == GET_BUFFER_ERROR) {
            audioio_audioout_stop(self, true);
            return false;
        }

        bool buffer_changed;
        buffer_changed = self->samples_convert(
            raw_sample_buf,
            raw_sample_buf_size,
            &sample_buf,
            &sample_buf_size);

        if (buffer_changed) {
            if (self->scratch_buffer != NULL) {
                m_free(self->scratch_buffer);
            }
            self->scratch_buffer = sample_buf;
            self->scratch_buffer_size = sample_buf_size;
        }
    }

    if (sample_buf_size > 0) {
        ret = dac_continuous_write_asynchronously(self->handle,
            dma_buf, dma_buf_size,
            sample_buf, sample_buf_size,
            &bytes_loaded);

        if (ret != ESP_OK) {
            return false;
        }
    }

    sample_buf_size -= bytes_loaded;
    if (sample_buf_size == 0) {
        sample_buf = NULL;
    } else {
        sample_buf += bytes_loaded;
    }

    self->sample_buffer = sample_buf;
    self->sample_buffer_size = sample_buf_size;
    self->sample_buffer_result = get_buffer_result;

    if (get_buffer_result == GET_BUFFER_DONE && sample_buf_size == 0) {
        if (self->looping) {
            audiosample_reset_buffer(self->sample, true, 0);
        } else {
            // TODO: figure out if it is ok to call this here or do we need
            // to somehow wait for all of the samples to be flushed
            audioio_audioout_stop(self, true);
            return false;
        }
    }

    return true;
}

static void audioout_fill_buffers(audioio_audioout_obj_t *self) {
    while (audioout_fill_buffer(self)) {
        ;
    }
}

static void audioout_buf_callback_fun(void *user_data) {
    audioio_audioout_obj_t *self = (audioio_audioout_obj_t *)user_data;
    audioout_fill_buffers(self);
}

static bool IRAM_ATTR handle_convert_done(dac_continuous_handle_t handle, const dac_event_data_t *event, void *user_data) {
    audioio_audioout_obj_t *self = (audioio_audioout_obj_t *)user_data;

    uint8_t *newly_freed_dma_buf = event->buf;
    size_t newly_freed_dma_buf_size = event->buf_size;

    uint8_t get_buf_idx = self->get_buffer_index;
    uint8_t put_buf_idx = self->put_buffer_index;
    uint8_t new_put_buf_idx = INCREMENT_BUF_IDX(put_buf_idx);

    // if the ring buffer of dma buffers is full then drop this one
    if (get_buf_idx == new_put_buf_idx) {
        return false;
    }

    self->dma_buffers[put_buf_idx].ptr = newly_freed_dma_buf;
    self->dma_buffers[put_buf_idx].size = newly_freed_dma_buf_size;

    self->put_buffer_index = new_put_buf_idx;

    background_callback_add(&self->callback, audioout_buf_callback_fun, user_data);

    return false;
}

static void audioout_init(audioio_audioout_obj_t *self) {
    dac_continuous_digi_clk_src_t clk_src = DAC_DIGI_CLK_SRC_DEFAULT;
    if (self->freq_hz < 19600) {
        clk_src = DAC_DIGI_CLK_SRC_APLL;
    }

    dac_continuous_config_t cfg = {
        .chan_mask = self->channel_mask,
        .desc_num = NUM_DMA_BUFFERS,
        .buf_size = DMA_BUFFER_SIZE,
        .freq_hz = self->freq_hz,
        .offset = 0,
        .clk_src = clk_src,
        .chan_mode = self->channel_mode,
    };

    esp_err_t ret;

    ret = dac_continuous_new_channels(&cfg, &self->handle);
    if (ret == ESP_ERR_INVALID_ARG) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Failed to create continuous channels: invalid arg"));
    } else if (ret == ESP_ERR_INVALID_STATE) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Failed to create continuous channels: invalid state"));
    } else if (ret == ESP_ERR_NOT_FOUND) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Failed to create continuous channels: not found"));
    } else if (ret == ESP_ERR_NO_MEM) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Failed to create continuous channels: no mem"));
    }

    _active_handle = self->handle;

    dac_event_callbacks_t callbacks = {
        .on_convert_done = handle_convert_done,
        .on_stop = NULL,
    };

    ret = dac_continuous_register_event_callback(self->handle, &callbacks, (void *)self);
    if (ret != ESP_OK) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Failed to register continuous events callback"));
    }

    ret = dac_continuous_enable(self->handle);
    if (ret != ESP_OK) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Failed to enable continuous"));
    }
}

void common_hal_audioio_audioout_construct(audioio_audioout_obj_t *self,
    const mcu_pin_obj_t *left_channel_pin, const mcu_pin_obj_t *right_channel_pin, uint16_t quiescent_value) {

    if (_active_handle != NULL) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Can't construct AudioOut because continuous channel already open"));
    }

    self->playing = false;
    self->paused = false;
    self->freq_hz = DEFAULT_SAMPLE_RATE;

    // The case of left_channel == right_channel is already disallowed in shared-bindings.

    if ((left_channel_pin == &pin_CHANNEL_0 &&
         right_channel_pin == &pin_CHANNEL_1) ||
        (left_channel_pin == &pin_CHANNEL_1 &&
         right_channel_pin == &pin_CHANNEL_0)) {
        self->channel_mask = DAC_CHANNEL_MASK_ALL;
        self->num_channels = 2;
        self->channel_mode = DAC_CHANNEL_MODE_ALTER;
    } else if (left_channel_pin == &pin_CHANNEL_0 &&
               right_channel_pin == NULL) {
        self->channel_mask = DAC_CHANNEL_MASK_CH0;
        self->num_channels = 1;
        self->channel_mode = DAC_CHANNEL_MODE_SIMUL;
    } else if (left_channel_pin == &pin_CHANNEL_1 &&
               right_channel_pin == NULL) {
        self->channel_mask = DAC_CHANNEL_MASK_CH1;
        self->num_channels = 1;
        self->channel_mode = DAC_CHANNEL_MODE_SIMUL;
    } else {
        raise_ValueError_invalid_pin();
    }

    audioout_init(self);

    claim_pin(left_channel_pin);
    if (right_channel_pin != NULL) {
        claim_pin(right_channel_pin);
    }
}

bool common_hal_audioio_audioout_deinited(audioio_audioout_obj_t *self) {
    return self->handle == NULL;
}

void common_hal_audioio_audioout_deinit(audioio_audioout_obj_t *self) {
    if (common_hal_audioio_audioout_deinited(self)) {
        return;
    }

    if (self->playing) {
        common_hal_audioio_audioout_stop(self);
    }
    dac_continuous_disable(self->handle);
    dac_continuous_del_channels(self->handle);
    if (self->scratch_buffer != NULL) {
        m_free(self->scratch_buffer);
        self->scratch_buffer = NULL;
        self->scratch_buffer_size = 0;
    }
    self->handle = NULL;
    _active_handle = NULL;
    if (self->channel_mask & DAC_CHANNEL_MASK_CH0) {
        reset_pin_number(pin_CHANNEL_0.number);
    }
    if (self->channel_mask & DAC_CHANNEL_MASK_CH1) {
        reset_pin_number(pin_CHANNEL_1.number);
    }
}

void common_hal_audioio_audioout_play(audioio_audioout_obj_t *self,
    mp_obj_t sample, bool loop) {

    if (self->playing) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("already playing"));
    }

    audiosample_check(sample);

    size_t samples_size;
    uint8_t channel_count;
    bool samples_signed;
    bool _single_buffer;
    uint32_t _max_buffer_length;
    uint8_t _spacing;
    uint32_t freq_hz;

    audiosample_reset_buffer(sample, true, 0);

    self->sample = sample;
    self->looping = loop;
    freq_hz = audiosample_get_sample_rate(self->sample);

    // Workaround: always reset the DAC completely between plays,
    // due to a bug that causes the left and right channels to be swapped randomly.
    // See https://github.com/espressif/esp-idf/issues/11425
    // TODO: Remove the `true` when this issue is fixed.
    if (true || freq_hz != self->freq_hz) {
        common_hal_audioio_audioout_deinit(self);
        self->freq_hz = freq_hz;
        audioout_init(self);
    }

    samples_size = audiosample_get_bits_per_sample(self->sample);
    channel_count = audiosample_get_channel_count(self->sample);
    audiosample_get_buffer_structure(self->sample, false,
        &_single_buffer, &samples_signed,
        &_max_buffer_length, &_spacing);

    self->samples_convert = audioout_get_samples_convert_func(
        samples_size,
        channel_count,
        samples_signed,
        self->num_channels);

    audioio_audioout_start(self);
}

void common_hal_audioio_audioout_pause(audioio_audioout_obj_t *self) {
    if (!self->playing || self->paused) {
        return;
    }
    audioio_audioout_stop(self, false);
}

void common_hal_audioio_audioout_resume(audioio_audioout_obj_t *self) {
    if (!self->playing || !self->paused) {
        return;
    }
    audioio_audioout_start(self);
}

bool common_hal_audioio_audioout_get_paused(audioio_audioout_obj_t *self) {
    return self->playing && self->paused;
}

void common_hal_audioio_audioout_stop(audioio_audioout_obj_t *self) {
    if (!self->playing) {
        return;
    }
    audioio_audioout_stop(self, true);
}

bool common_hal_audioio_audioout_get_playing(audioio_audioout_obj_t *self) {
    return self->playing && !self->paused;
}
