// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tyeth Gundry
//
// SPDX-License-Identifier: MIT

// PWM audio for the ESP32 family.
//
// LEDC has no DMA path for its duty registers, and the ESP32-S2's I2S has no
// PDM transmitter, so samples are pushed one at a time from a GPTimer alarm
// ISR running at the sample rate. Two device buffers are used: the ISR drains
// one while audiopwmout_background() refills the other.
//
// The ISR reads and writes only the internal-RAM control block, for the two
// reasons set out in PWMAudioOut.h and below. In particular it must not queue
// a background callback: on this port that macro is taskENTER_CRITICAL(),
// which aborts when called from ISR context.

#include "common-hal/audiopwmio/PWMAudioOut.h"

#include <stdint.h>
#include <string.h>

#include "py/gc.h"
#include "py/runtime.h"

#include "bindings/espidf/__init__.h"
#include "shared-bindings/audiopwmio/PWMAudioOut.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/pwmio/PWMOut.h"
#include "shared-module/audiocore/__init__.h"

#include "driver/gptimer.h"
#include "esp_attr.h"
#include "esp_heap_caps.h"
#include "esp_memory_utils.h"
#include "hal/ledc_ll.h"
#include "soc/soc.h"

// Carrier frequency: 80 MHz / 1024 = 78125 Hz. pwmio derives PWM_BITS back out
// of this when it picks a duty resolution, so the two stay in step.
#define PWM_FREQUENCY (APB_CLK_FREQ >> PWM_BITS)

// GPTimer tick rate. The timer group divider must be at least 2, so 40 MHz is
// the finest available, and it keeps the sample-rate error below a cent
// (44100 Hz -> 907 ticks -> 44101.4 Hz).
#define TIMER_RESOLUTION_HZ (APB_CLK_FREQ / 2)

// Internal RAM is scarce, and both buffers come out of it. This is generous
// for the buffer sizes the audiosample protocol actually hands over (a
// WaveFile uses 512 bytes) while still failing clearly on something silly.
#define MAX_BUFFER_BYTES (8192)

// Each object needs its own LEDC timer, so that bounds how many can play.
static audiopwmio_pwmaudioout_obj_t *active_audio[SOC_LEDC_TIMER_NUM];

// Write one duty value to a LEDC channel. This mirrors what ledc_update_duty()
// does, minus the argument checks and the spinlock, so it is cheap enough to
// run at the sample rate. The fade parameters it relies on (scale 0, so the
// duty is latched rather than ramped) are left set up by pwmio's construct.
static inline void IRAM_ATTR set_duty(uint8_t channel, uint32_t duty) {
    ledc_ll_set_duty_int_part(&LEDC, LEDC_LOW_SPEED_MODE, channel, duty);
    ledc_ll_set_sig_out_en(&LEDC, LEDC_LOW_SPEED_MODE, channel, true);
    ledc_ll_set_duty_start(&LEDC, LEDC_LOW_SPEED_MODE, channel);
    ledc_ll_ls_channel_update(&LEDC, LEDC_LOW_SPEED_MODE, channel);
}

static bool IRAM_ATTR timer_alarm_cb(gptimer_handle_t timer,
    const gptimer_alarm_event_data_t *edata, void *internal_data_ptr) {
    audiopwmio_internal_data_t *internal_data =
        (audiopwmio_internal_data_t *)internal_data_ptr;
    (void)timer;
    (void)edata;

    uint8_t buf_index = internal_data->playing_buffer;
    if (!internal_data->buffer_ready[buf_index]) {
        // Underrun, or the sample ended. Hold the quiescent level rather than
        // leaving the last sample on the pin, which would be a DC offset.
        set_duty(internal_data->left_channel, internal_data->quiescent_value);
        if (internal_data->stereo) {
            set_duty(internal_data->right_channel, internal_data->quiescent_value);
        }
        return false;
    }

    size_t frame = internal_data->playing_frame;
    const uint16_t *sample =
        &internal_data->buffers[buf_index][frame * internal_data->out_channels];

    set_duty(internal_data->left_channel, sample[0]);
    if (internal_data->stereo) {
        set_duty(internal_data->right_channel, sample[1]);
    }

    frame++;
    if (frame >= internal_data->buffer_frames[buf_index]) {
        if (internal_data->single_buffer && internal_data->loop) {
            // The whole sample fits in one buffer and repeats forever, so wrap
            // in place. This keeps looping RawSamples glitch-free: no refill,
            // and no dependency on how promptly the background task runs.
            internal_data->playing_frame = 0;
            return false;
        }
        // This buffer is spent. Flag it; the background task does the refill.
        internal_data->buffer_ready[buf_index] = false;
        if (internal_data->last_buffer[buf_index]) {
            internal_data->stop_queued = true;
        }
        internal_data->playing_frame = 0;
        if (!internal_data->single_buffer) {
            internal_data->playing_buffer = buf_index ^ 1;
        }
    } else {
        internal_data->playing_frame = frame;
    }

    return false;
}

// Convert one source buffer into device buffer `buf_index`, scaling samples to
// PWM_BITS. Runs in task context, so it may touch the object freely.
static void fill_buffer(audiopwmio_pwmaudioout_obj_t *self, uint8_t buf_index) {
    audiopwmio_internal_data_t *internal_data = self->internal_data;
    uint8_t *source;
    uint32_t source_length;
    audioio_get_buffer_result_t result =
        audiosample_get_buffer(self->sample, false, 0, &source, &source_length);

    if (result == GET_BUFFER_ERROR) {
        internal_data->stop_queued = true;
        return;
    }

    uint8_t out_channels = internal_data->out_channels;
    size_t frames = source_length / self->bytes_per_sample / self->sample_channel_count;
    if (frames * out_channels * sizeof(uint16_t) > self->buffer_size[buf_index]) {
        // Should not happen: play() sized the buffers from max_buffer_length.
        frames = self->buffer_size[buf_index] / (out_channels * sizeof(uint16_t));
    }

    uint16_t *dest = internal_data->buffers[buf_index];
    if (self->bytes_per_sample == 1) {
        uint8_t xor_mask = self->signed_to_unsigned ? 0x80 : 0;
        for (size_t i = 0; i < frames; i++) {
            uint16_t left = (uint16_t)((source[i * self->sample_channel_count] ^ xor_mask) << (PWM_BITS - 8));
            *dest++ = left;
            if (out_channels == 2) {
                *dest++ = (self->sample_channel_count == 2)
                    ? (uint16_t)((source[i * 2 + 1] ^ xor_mask) << (PWM_BITS - 8))
                    : left;
            }
        }
    } else {
        uint16_t xor_mask = self->signed_to_unsigned ? 0x8000 : 0;
        // Cast via void * : the sample buffer is 16-bit aligned for 16-bit
        // samples, but the protocol hands it over as uint8_t *.
        const uint16_t *source16 = (const uint16_t *)(const void *)source;
        for (size_t i = 0; i < frames; i++) {
            uint16_t left = (uint16_t)((source16[i * self->sample_channel_count] ^ xor_mask) >> SAMPLE_BITS_TO_DISCARD);
            *dest++ = left;
            if (out_channels == 2) {
                *dest++ = (self->sample_channel_count == 2)
                    ? (uint16_t)((source16[i * 2 + 1] ^ xor_mask) >> SAMPLE_BITS_TO_DISCARD)
                    : left;
            }
        }
    }

    internal_data->buffer_frames[buf_index] = frames;
    internal_data->last_buffer[buf_index] = false;

    if (result == GET_BUFFER_DONE) {
        if (internal_data->loop) {
            audiosample_reset_buffer(self->sample, false, 0);
        } else {
            internal_data->last_buffer[buf_index] = true;
        }
    }

    // Publish last, so the ISR never sees a half-filled buffer.
    __asm__ volatile ("" ::: "memory");
    internal_data->buffer_ready[buf_index] = (frames > 0);
    if (frames == 0 && result == GET_BUFFER_DONE && !internal_data->loop) {
        internal_data->stop_queued = true;
    }
}

// Tear down the timer without touching the heap, so this is also safe to call
// while the VM is being reset.
static void stop_timer(audiopwmio_pwmaudioout_obj_t *self) {
    if (self->timer != NULL) {
        gptimer_stop(self->timer);
        gptimer_disable(self->timer);
        gptimer_del_timer(self->timer);
        self->timer = NULL;
    }
    for (size_t i = 0; i < MP_ARRAY_SIZE(active_audio); i++) {
        if (active_audio[i] == self) {
            active_audio[i] = NULL;
        }
    }
}

static void free_buffers(audiopwmio_pwmaudioout_obj_t *self) {
    for (uint8_t i = 0; i < 2; i++) {
        if (self->internal_data->buffers[i] != NULL) {
            heap_caps_free(self->internal_data->buffers[i]);
            self->internal_data->buffers[i] = NULL;
        }
        self->buffer_size[i] = 0;
        self->internal_data->buffer_frames[i] = 0;
        self->internal_data->buffer_ready[i] = false;
        self->internal_data->last_buffer[i] = false;
    }
}

void audiopwmout_background(void) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(active_audio); i++) {
        audiopwmio_pwmaudioout_obj_t *self = active_audio[i];
        if (self == NULL || !self->playing) {
            continue;
        }
        if (self->internal_data->stop_queued) {
            common_hal_audiopwmio_pwmaudioout_stop(self);
            continue;
        }
        if (self->paused) {
            continue;
        }
        for (uint8_t buf = 0; buf < 2; buf++) {
            if (!self->internal_data->buffer_ready[buf] && !self->internal_data->stop_queued) {
                fill_buffer(self, buf);
            }
            if (self->internal_data->single_buffer) {
                // A single-buffer sample hands back the same memory every
                // time, so there is nothing to gain from a second buffer.
                break;
            }
        }
    }
}

// Caller validates that pins are free.
void common_hal_audiopwmio_pwmaudioout_construct(audiopwmio_pwmaudioout_obj_t *self,
    const mcu_pin_obj_t *left_channel, const mcu_pin_obj_t *right_channel, uint16_t quiescent_value) {

    self->timer = NULL;
    self->sample = NULL;
    self->playing = false;
    self->paused = false;

    self->internal_data = heap_caps_calloc(1, sizeof(audiopwmio_internal_data_t),
        MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    if (self->internal_data == NULL) {
        raise_esp_error(ESP_ERR_NO_MEM);
    }
    self->internal_data->stereo = right_channel != NULL;
    self->internal_data->out_channels = right_channel != NULL ? 2 : 1;

    // variable_frequency is false so that the two channels of a stereo pair can
    // share one LEDC timer. At PWM_FREQUENCY pwmio picks a PWM_BITS-wide duty.
    pwmout_result_t result = common_hal_pwmio_pwmout_construct(
        &self->left_pwm, left_channel, 0, PWM_FREQUENCY, false);
    if (result == PWMOUT_OK && self->internal_data->stereo) {
        result = common_hal_pwmio_pwmout_construct(
            &self->right_pwm, right_channel, 0, PWM_FREQUENCY, false);
        if (result != PWMOUT_OK) {
            common_hal_pwmio_pwmout_deinit(&self->left_pwm);
        }
    }
    if (result != PWMOUT_OK) {
        heap_caps_free(self->internal_data);
        self->internal_data = NULL;
        common_hal_pwmio_pwmout_raise_error(result);
    }

    // Copy the channel numbers in: the ISR must not reach into the pwmio
    // objects, which live on the (possibly PSRAM) MicroPython heap.
    self->internal_data->left_channel = self->left_pwm.chan_handle.channel;
    if (self->internal_data->stereo) {
        self->internal_data->right_channel = self->right_pwm.chan_handle.channel;
    }

    self->internal_data->quiescent_value = quiescent_value >> SAMPLE_BITS_TO_DISCARD;
    common_hal_pwmio_pwmout_set_duty_cycle(&self->left_pwm, quiescent_value);
    if (self->internal_data->stereo) {
        common_hal_pwmio_pwmout_set_duty_cycle(&self->right_pwm, quiescent_value);
    }
}

bool common_hal_audiopwmio_pwmaudioout_deinited(audiopwmio_pwmaudioout_obj_t *self) {
    return self->internal_data == NULL;
}

void common_hal_audiopwmio_pwmaudioout_deinit(audiopwmio_pwmaudioout_obj_t *self) {
    if (common_hal_audiopwmio_pwmaudioout_deinited(self)) {
        return;
    }

    common_hal_audiopwmio_pwmaudioout_stop(self);

    common_hal_pwmio_pwmout_deinit(&self->left_pwm);
    if (self->internal_data->stereo) {
        common_hal_pwmio_pwmout_deinit(&self->right_pwm);
    }

    heap_caps_free(self->internal_data);
    self->internal_data = NULL;
}

void common_hal_audiopwmio_pwmaudioout_play(audiopwmio_pwmaudioout_obj_t *self,
    mp_obj_t sample, bool loop) {

    if (common_hal_audiopwmio_pwmaudioout_get_playing(self)) {
        common_hal_audiopwmio_pwmaudioout_stop(self);
    }

    audiopwmio_internal_data_t *internal_data = self->internal_data;
    audiosample_base_t *base = audiosample_check(sample);

    uint32_t sample_rate = audiosample_get_sample_rate(base);
    if (sample_rate == 0 || sample_rate > TIMER_RESOLUTION_HZ) {
        mp_arg_error_invalid(MP_QSTR_sample_rate);
    }

    self->sample = sample;
    internal_data->loop = loop;
    self->bytes_per_sample = audiosample_get_bits_per_sample(base) / 8;
    self->sample_channel_count = audiosample_get_channel_count(base);

    uint32_t max_buffer_length;
    uint8_t spacing;
    audiosample_get_buffer_structure(base, false, &internal_data->single_buffer,
        &self->signed_to_unsigned, &max_buffer_length, &spacing);

    size_t frames = max_buffer_length / self->bytes_per_sample / self->sample_channel_count;
    size_t buffer_size = frames * internal_data->out_channels * sizeof(uint16_t);
    if (buffer_size == 0) {
        mp_arg_error_invalid(MP_QSTR_sample);
    }
    if (buffer_size > MAX_BUFFER_BYTES) {
        // These come out of internal RAM, so refuse rather than exhaust it.
        raise_esp_error(ESP_ERR_NO_MEM);
    }

    // Claim a slot before allocating, so a failure here frees nothing.
    size_t slot = MP_ARRAY_SIZE(active_audio);
    for (size_t i = 0; i < MP_ARRAY_SIZE(active_audio); i++) {
        if (active_audio[i] == NULL) {
            slot = i;
            break;
        }
    }
    if (slot == MP_ARRAY_SIZE(active_audio)) {
        raise_esp_error(ESP_ERR_NOT_FOUND);
    }

    free_buffers(self);
    for (uint8_t i = 0; i < 2; i++) {
        // The ISR reads these with the cache possibly disabled, so they have
        // to be internal RAM too, not the MicroPython heap.
        internal_data->buffers[i] = heap_caps_malloc(buffer_size,
            MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
        if (internal_data->buffers[i] == NULL) {
            free_buffers(self);
            raise_esp_error(ESP_ERR_NO_MEM);
        }
        self->buffer_size[i] = buffer_size;
        if (internal_data->single_buffer) {
            break;
        }
    }

    audiosample_reset_buffer(sample, false, 0);

    internal_data->playing_buffer = 0;
    internal_data->playing_frame = 0;
    internal_data->stop_queued = false;
    self->paused = false;
    self->playing = true;

    // Prime both buffers before the timer starts.
    fill_buffer(self, 0);
    if (!internal_data->single_buffer && !internal_data->stop_queued) {
        fill_buffer(self, 1);
    }
    if (internal_data->stop_queued) {
        // Nothing to play (empty or broken sample). Don't start the timer, or
        // there would be nothing to shut it down again.
        common_hal_audiopwmio_pwmaudioout_stop(self);
        return;
    }

    // The GPTimer driver is built cache-safe here, so it requires the callback
    // in IRAM and its user context in internal RAM, and it only checks the
    // latter two. Check all of it, so a regression shows up as an error from
    // play() rather than as a fault at a random later moment.
    if (!esp_ptr_in_iram((const void *)timer_alarm_cb) ||
        !esp_ptr_internal(internal_data) ||
        !esp_ptr_internal(internal_data->buffers[0]) ||
        (internal_data->buffers[1] != NULL && !esp_ptr_internal(internal_data->buffers[1]))) {
        self->playing = false;
        free_buffers(self);
        raise_esp_error(ESP_ERR_INVALID_ARG);
    }

    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = TIMER_RESOLUTION_HZ,
        .intr_priority = 0,
    };
    gptimer_event_callbacks_t callbacks = {
        .on_alarm = timer_alarm_cb,
    };
    // CircuitPython builds with CONFIG_GPTIMER_CTRL_FUNC_IN_IRAM, under which
    // gptimer_set_alarm_action() rejects a config that is not in internal RAM.
    // The IDF docs call for a static instance here, so give it one.
    static gptimer_alarm_config_t alarm_config;
    alarm_config.alarm_count = TIMER_RESOLUTION_HZ / sample_rate;
    alarm_config.reload_count = 0;
    alarm_config.flags.auto_reload_on_alarm = true;

    active_audio[slot] = self;
    esp_err_t err = gptimer_new_timer(&timer_config, &self->timer);
    if (err == ESP_OK) {
        err = gptimer_register_event_callbacks(self->timer, &callbacks, internal_data);
    }
    if (err == ESP_OK) {
        err = gptimer_set_alarm_action(self->timer, &alarm_config);
    }
    if (err == ESP_OK) {
        err = gptimer_enable(self->timer);
    }
    if (err == ESP_OK) {
        err = gptimer_start(self->timer);
    }
    if (err != ESP_OK) {
        // Leave nothing half-started: a stuck `playing` would hang any
        // `while playing:` loop forever.
        self->playing = false;
        stop_timer(self);
        free_buffers(self);
        raise_esp_error(err);
    }
}

void common_hal_audiopwmio_pwmaudioout_stop(audiopwmio_pwmaudioout_obj_t *self) {
    stop_timer(self);

    self->playing = false;
    self->paused = false;
    self->internal_data->stop_queued = false;
    self->sample = NULL;

    // Park the output at the quiescent level so the speaker does not sit at a
    // DC offset between samples.
    uint16_t quiescent = self->internal_data->quiescent_value << SAMPLE_BITS_TO_DISCARD;
    if (!common_hal_pwmio_pwmout_deinited(&self->left_pwm)) {
        common_hal_pwmio_pwmout_set_duty_cycle(&self->left_pwm, quiescent);
    }
    if (self->internal_data->stereo && !common_hal_pwmio_pwmout_deinited(&self->right_pwm)) {
        common_hal_pwmio_pwmout_set_duty_cycle(&self->right_pwm, quiescent);
    }

    free_buffers(self);
}

bool common_hal_audiopwmio_pwmaudioout_get_playing(audiopwmio_pwmaudioout_obj_t *self) {
    return self->playing;
}

void common_hal_audiopwmio_pwmaudioout_pause(audiopwmio_pwmaudioout_obj_t *self) {
    if (!self->playing || self->paused || self->timer == NULL) {
        return;
    }
    gptimer_stop(self->timer);
    self->paused = true;
}

void common_hal_audiopwmio_pwmaudioout_resume(audiopwmio_pwmaudioout_obj_t *self) {
    if (!self->paused || self->timer == NULL) {
        return;
    }
    self->paused = false;
    gptimer_start(self->timer);
}

bool common_hal_audiopwmio_pwmaudioout_get_paused(audiopwmio_pwmaudioout_obj_t *self) {
    return self->paused;
}
