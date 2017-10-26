/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#include <stdint.h>
#include <string.h>

#include "extmod/vfs_fat_file.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "common-hal/audioio/AudioOut.h"
#include "shared-bindings/audioio/AudioOut.h"
#include "shared-bindings/microcontroller/Pin.h"

#include "asf/sam0/drivers/dac/dac.h"
#include "asf/sam0/drivers/dma/dma.h"
#include "asf/sam0/drivers/events/events.h"
#include "asf/sam0/drivers/port/port.h"
#include "asf/sam0/drivers/tc/tc.h"
#include "samd21_pins.h"
#include "shared_dma.h"

#undef ENABLE

// Shared with PWMOut
// TODO(tannewt): Factor these out so audioio can exist without PWMOut.
extern uint32_t target_timer_frequencies[TC_INST_NUM + TCC_INST_NUM];
extern uint8_t timer_refcount[TC_INST_NUM + TCC_INST_NUM];
extern const uint16_t prescaler[8];

// This timer is shared amongst all AudioOut objects under the assumption that
// the code is single threaded. The audioout_sample_timer, audioout_dac_instance,
// audioout_sample_event, and audioout_dac_event pointers live in
// MICROPY_PORT_ROOT_POINTERS so they don't get garbage collected.

// The AudioOut object is being currently played. Only it can pause the timer
// and change its frequency.
static audioio_audioout_obj_t* active_audioout;

static uint8_t refcount = 0;

struct wave_format_chunk {
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    uint16_t extra_params; // Assumed to be zero below.
};

void audioout_reset(void) {
    // Only reset DMA. PWMOut will reset the timer. Other code will reset the DAC.
    refcount = 0;
    MP_STATE_VM(audioout_sample_timer) = NULL;
    MP_STATE_VM(audiodma_block_counter) = NULL;
    MP_STATE_VM(audioout_dac_instance) = NULL;

    if (MP_STATE_VM(audioout_sample_event) != NULL) {
        events_detach_user(MP_STATE_VM(audioout_sample_event), EVSYS_ID_USER_DAC_START);
        events_release(MP_STATE_VM(audioout_sample_event));
    }
    MP_STATE_VM(audioout_sample_event) = NULL;

    if (MP_STATE_VM(audiodma_block_event) != NULL) {
        events_release(MP_STATE_VM(audiodma_block_event));
    }
    MP_STATE_VM(audiodma_block_event) = NULL;

    if (MP_STATE_VM(audioout_dac_event) != NULL) {
        events_detach_user(MP_STATE_VM(audioout_dac_event), EVSYS_ID_USER_DMAC_CH_0);
        events_release(MP_STATE_VM(audioout_dac_event));
    }
    MP_STATE_VM(audioout_dac_event) = NULL;

    dma_abort_job(&audio_dma);
}

// WARN(tannewt): DO NOT print from here. It calls background tasks and causes a
// stack overflow.
void audioout_background(void) {
    if (MP_STATE_VM(audiodma_block_counter) != NULL &&
            active_audioout != NULL &&
            active_audioout->second_buffer != NULL &&
            active_audioout->last_loaded_block < tc_get_count_value(MP_STATE_VM(audiodma_block_counter))) {
        uint8_t* buffer;
        if (tc_get_count_value(MP_STATE_VM(audiodma_block_counter)) % 2 == 1) {
            buffer = active_audioout->buffer;
        } else {
            buffer = active_audioout->second_buffer;
        }
        uint16_t num_bytes_to_load = active_audioout->len;
        if (num_bytes_to_load > active_audioout->bytes_remaining) {
            num_bytes_to_load = active_audioout->bytes_remaining;
        }
        UINT length_read;
        f_read(&active_audioout->file->fp, buffer, num_bytes_to_load, &length_read);
        active_audioout->bytes_remaining -= length_read;
        active_audioout->last_loaded_block += 1;

        if (active_audioout->bytes_remaining == 0) {
            if (active_audioout->loop) {
                // Loop back to the start of the file.
                f_lseek(&active_audioout->file->fp, active_audioout->data_start);
                active_audioout->bytes_remaining = active_audioout->file_length;
                f_read(&active_audioout->file->fp, buffer, active_audioout->len - num_bytes_to_load, &length_read);
                active_audioout->bytes_remaining -= length_read;
            } else {
                DmacDescriptor* descriptor = audio_dma.descriptor;
                if (buffer == active_audioout->second_buffer) {
                    descriptor = active_audioout->second_descriptor;
                }
                descriptor->BTCNT.reg = length_read / active_audioout->bytes_per_sample;
                descriptor->SRCADDR.reg = ((uint32_t) buffer) + length_read;
                descriptor->DESCADDR.reg = 0;
            }
        }

        if (active_audioout->bytes_per_sample == 2) {
            // Undo twos complement.
            for (uint16_t i = 0; i < length_read / 2; i++) {
                buffer[2 * i + 1] ^= 0x80;
            }
        }
    }
}

static void shared_construct(audioio_audioout_obj_t* self, const mcu_pin_obj_t* pin) {
    assert_pin_free(pin);

    // Configure the DAC to output on input event and to output an empty event
    // that triggers the DMA to load the next sample.
    MP_STATE_VM(audioout_dac_instance) = gc_alloc(sizeof(struct dac_module), false);
    if (MP_STATE_VM(audioout_dac_instance) == NULL) {
        mp_raise_msg(&mp_type_MemoryError, "");
    }
    struct dac_config config_dac;
    dac_get_config_defaults(&config_dac);
    config_dac.left_adjust = true;
    config_dac.reference = DAC_REFERENCE_AVCC;
    config_dac.clock_source = GCLK_GENERATOR_0;
    enum status_code status = dac_init(MP_STATE_VM(audioout_dac_instance), DAC, &config_dac);
    if (status != STATUS_OK) {
        common_hal_audioio_audioout_deinit(self);
        mp_raise_OSError(MP_EIO);
        return;
    }

    struct dac_chan_config channel_config;
    dac_chan_get_config_defaults(&channel_config);
    dac_chan_set_config(MP_STATE_VM(audioout_dac_instance), DAC_CHANNEL_0, &channel_config);
    dac_chan_enable(MP_STATE_VM(audioout_dac_instance), DAC_CHANNEL_0);

    struct dac_events events_dac = { .generate_event_on_buffer_empty = true,
                                     .on_event_start_conversion = true };
    dac_enable_events(MP_STATE_VM(audioout_dac_instance), &events_dac);

    // Figure out which timer we are using.
    Tc *t = NULL;
    Tc *tcs[TC_INST_NUM] = TC_INSTS;
    for (uint8_t i = TC_INST_NUM; i > 0; i--) {
        if (tcs[i - 1]->COUNT16.CTRLA.bit.ENABLE == 0) {
            t = tcs[i - 1];
            break;
        }
    }
    if (t == NULL) {
        common_hal_audioio_audioout_deinit(self);
        mp_raise_RuntimeError("All timers in use");
        return;
    }
    MP_STATE_VM(audioout_sample_timer) = gc_alloc(sizeof(struct tc_module), false);
    if (MP_STATE_VM(audioout_sample_timer) == NULL) {
        common_hal_audioio_audioout_deinit(self);
        mp_raise_msg(&mp_type_MemoryError, "");
    }

    // Don't bother setting the period. We set it before you playback anything.
    struct tc_config config_tc;
    tc_get_config_defaults(&config_tc);
    config_tc.counter_size    = TC_COUNTER_SIZE_16BIT;
    config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV1;
    config_tc.wave_generation = TC_WAVE_GENERATION_MATCH_FREQ;
    if (tc_init(MP_STATE_VM(audioout_sample_timer), t, &config_tc) != STATUS_OK) {
        common_hal_audioio_audioout_deinit(self);
        mp_raise_OSError(MP_EIO);
        return;
    };

    struct tc_events events_tc;
    events_tc.generate_event_on_overflow = true;
    events_tc.on_event_perform_action = false;
    events_tc.event_action = TC_EVENT_ACTION_OFF;
    tc_enable_events(MP_STATE_VM(audioout_sample_timer), &events_tc);

    tc_enable(MP_STATE_VM(audioout_sample_timer));
    tc_stop_counter(MP_STATE_VM(audioout_sample_timer));

    // Connect the timer overflow event, which happens at the target frequency,
    // to the DAC conversion trigger.
    MP_STATE_VM(audioout_sample_event) = gc_alloc(sizeof(struct events_resource), false);
    if (MP_STATE_VM(audioout_sample_event) == NULL) {
        common_hal_audioio_audioout_deinit(self);
        mp_raise_msg(&mp_type_MemoryError, "");
    }
    struct events_config config;
    events_get_config_defaults(&config);

    uint8_t generator = EVSYS_ID_GEN_TC3_OVF;
    if (t == TC4) {
        generator = EVSYS_ID_GEN_TC4_OVF;
    } else if (t == TC5) {
        generator = EVSYS_ID_GEN_TC5_OVF;
#ifdef TC6
    } else if (t == TC6) {
        generator = EVSYS_ID_GEN_TC6_OVF;
#endif
#ifdef TC7
    } else if (t == TC7) {
        generator = EVSYS_ID_GEN_TC7_OVF;
#endif
    }

    config.generator    = generator;
    config.path         = EVENTS_PATH_ASYNCHRONOUS;
    if (events_allocate(MP_STATE_VM(audioout_sample_event), &config) != STATUS_OK ||
        events_attach_user(MP_STATE_VM(audioout_sample_event), EVSYS_ID_USER_DAC_START) != STATUS_OK) {
        common_hal_audioio_audioout_deinit(self);
        mp_raise_OSError(MP_EIO);
        return;
    }

    // Connect the DAC to DMA
    MP_STATE_VM(audioout_dac_event) = gc_alloc(sizeof(struct events_resource), false);
    if (MP_STATE_VM(audioout_dac_event) == NULL) {
        common_hal_audioio_audioout_deinit(self);
        mp_raise_msg(&mp_type_MemoryError, "");
    }
    events_get_config_defaults(&config);
    config.generator    = EVSYS_ID_GEN_DAC_EMPTY;
    config.path         = EVENTS_PATH_ASYNCHRONOUS;
    if (events_allocate(MP_STATE_VM(audioout_dac_event), &config) != STATUS_OK ||
        events_attach_user(MP_STATE_VM(audioout_dac_event), EVSYS_ID_USER_DMAC_CH_0) != STATUS_OK) {
        common_hal_audioio_audioout_deinit(self);
        mp_raise_OSError(MP_EIO);
        return;
    }

    // Leave the DMA setup to the specific constructor.
}

void common_hal_audioio_audioout_construct_from_buffer(audioio_audioout_obj_t* self,
                                                       const mcu_pin_obj_t* pin,
                                                       uint16_t* buffer,
                                                       uint32_t len,
                                                       uint8_t bytes_per_sample) {
    self->pin = pin;
    if (pin != &pin_PA02) {
        mp_raise_ValueError("Invalid pin");
    }
    if (refcount == 0) {
        refcount++;
        shared_construct(self, pin);
    }

    self->buffer = (uint8_t*) buffer;
    self->second_buffer = NULL;
    self->bytes_per_sample = bytes_per_sample;
    self->len = len;
    self->frequency = 8000;
}

void common_hal_audioio_audioout_construct_from_file(audioio_audioout_obj_t* self,
                                                     const mcu_pin_obj_t* pin,
                                                     pyb_file_obj_t* file) {
    self->pin = pin;
    if (pin != &pin_PA02) {
        mp_raise_ValueError("Invalid pin");
    }
    if (refcount == 0) {
        refcount++;
        shared_construct(self, pin);
    }
    if (MP_STATE_VM(audiodma_block_counter) == NULL && !allocate_block_counter()) {
        mp_raise_RuntimeError("Unable to allocate audio DMA block counter.");
    }

    // Load the wave
    self->file = file;
    uint8_t chunk_header[16];
    f_rewind(&self->file->fp);
    UINT bytes_read;
    f_read(&self->file->fp, chunk_header, 16, &bytes_read);
    if (bytes_read != 16 ||
        memcmp(chunk_header, "RIFF", 4) != 0 ||
        memcmp(chunk_header + 8, "WAVEfmt ", 8) != 0) {
        mp_raise_ValueError("Invalid wave file");
    }
    uint32_t format_size;
    f_read(&self->file->fp, &format_size, 4, &bytes_read);
    if (bytes_read != 4 ||
        format_size > sizeof(struct wave_format_chunk)) {
        mp_raise_ValueError("Invalid format chunk size");
    }
    struct wave_format_chunk format;
    f_read(&self->file->fp, &format, format_size, &bytes_read);
    if (bytes_read != format_size) {
    }

    if (format.audio_format != 1 ||
        format.num_channels > 1 ||
        format.bits_per_sample > 16 ||
        (format_size == 18 &&
         format.extra_params != 0)) {
        mp_raise_ValueError("Unsupported format");
    }
    // Get the frequency
    self->frequency = format.sample_rate;
    self->len = 512;

    self->bytes_per_sample = format.bits_per_sample / 8;

    // TODO(tannewt): Skip any extra chunks that occur before the data section.

    uint8_t data_tag[4];
    f_read(&self->file->fp, &data_tag, 4, &bytes_read);
    if (bytes_read != 4 ||
        memcmp((uint8_t *) data_tag, "data", 4) != 0) {
        mp_raise_ValueError("Data chunk must follow fmt chunk");
    }

    uint32_t data_length;
    f_read(&self->file->fp, &data_length, 4, &bytes_read);
    if (bytes_read != 4) {
        mp_raise_ValueError("Invalid file");
    }
    self->file_length = data_length;
    self->data_start = self->file->fp.fptr;

    // Try to allocate two buffers, one will be loaded from file and the other
    // DMAed to DAC.
    self->buffer = gc_alloc(self->len, false);
    if (self->buffer == NULL) {
        common_hal_audioio_audioout_deinit(self);
        mp_raise_msg(&mp_type_MemoryError, "");
    }

    self->second_buffer = gc_alloc(self->len, false);
    if (self->second_buffer == NULL) {
        common_hal_audioio_audioout_deinit(self);
        mp_raise_msg(&mp_type_MemoryError, "");
    }

    self->second_descriptor = gc_alloc(sizeof(DmacDescriptor), false);
    if (self->second_descriptor == NULL) {
        common_hal_audioio_audioout_deinit(self);
        mp_raise_msg(&mp_type_MemoryError, "");
    }
}

bool common_hal_audioio_audioout_deinited(audioio_audioout_obj_t* self) {
    return self->pin == mp_const_none;
}

void common_hal_audioio_audioout_deinit(audioio_audioout_obj_t* self) {
    if (common_hal_audioio_audioout_deinited(self)) {
        return;
    }
    refcount--;
    if (refcount == 0) {
        if (MP_STATE_VM(audioout_sample_timer) != NULL) {
            tc_reset(MP_STATE_VM(audioout_sample_timer));
            gc_free(MP_STATE_VM(audioout_sample_timer));
            MP_STATE_VM(audioout_sample_timer) = NULL;
        }
        if (MP_STATE_VM(audioout_dac_instance) != NULL) {
            dac_reset(MP_STATE_VM(audioout_dac_instance));
            gc_free(MP_STATE_VM(audioout_dac_instance));
            MP_STATE_VM(audioout_dac_instance) = NULL;
        }
        if (MP_STATE_VM(audioout_sample_event) != NULL) {
            events_detach_user(MP_STATE_VM(audioout_sample_event), EVSYS_ID_USER_DAC_START);
            events_release(MP_STATE_VM(audioout_sample_event));
            gc_free(MP_STATE_VM(audioout_sample_event));
            MP_STATE_VM(audioout_sample_event) = NULL;
        }
        if (MP_STATE_VM(audioout_dac_event) != NULL) {
            events_release(MP_STATE_VM(audioout_dac_event));
            gc_free(MP_STATE_VM(audioout_dac_event));
            MP_STATE_VM(audioout_dac_event) = NULL;
        }
        reset_pin(self->pin->pin);
    }

    self->pin = mp_const_none;
}

static void set_timer_frequency(uint32_t frequency) {
    uint32_t system_clock = system_cpu_clock_get_hz();
    uint32_t new_top;
    uint8_t new_divisor;
    for (new_divisor = 0; new_divisor < 8; new_divisor++) {
        new_top = (system_clock / prescaler[new_divisor] / frequency) - 1;
        if (new_top < (1u << 16)) {
            break;
        }
    }
    uint8_t old_divisor = MP_STATE_VM(audioout_sample_timer)->hw->COUNT16.CTRLA.bit.PRESCALER;
    if (new_divisor != old_divisor) {
        tc_disable(MP_STATE_VM(audioout_sample_timer));
        MP_STATE_VM(audioout_sample_timer)->hw->COUNT16.CTRLA.bit.PRESCALER = new_divisor;
        tc_enable(MP_STATE_VM(audioout_sample_timer));
    }
    while (tc_is_syncing(MP_STATE_VM(audioout_sample_timer))) {
        /* Wait for sync */
    }
    MP_STATE_VM(audioout_sample_timer)->hw->COUNT16.CC[0].reg = new_top;
    while (tc_is_syncing(MP_STATE_VM(audioout_sample_timer))) {
        /* Wait for sync */
    }
}

void common_hal_audioio_audioout_play(audioio_audioout_obj_t* self, bool loop) {
    common_hal_audioio_audioout_get_playing(self);
    // Shut down any active playback.
    if (active_audioout != NULL) {
        tc_stop_counter(MP_STATE_VM(audioout_sample_timer));
        dma_abort_job(&audio_dma);
    } else {
        dac_enable(MP_STATE_VM(audioout_dac_instance));
    }
    switch_audiodma_trigger(DAC_DMAC_ID_EMPTY);
    struct dma_descriptor_config descriptor_config;
    dma_descriptor_get_config_defaults(&descriptor_config);
    if (self->bytes_per_sample == 2) {
        descriptor_config.beat_size = DMA_BEAT_SIZE_HWORD;
    } else {
        descriptor_config.beat_size = DMA_BEAT_SIZE_BYTE;
    }

    descriptor_config.dst_increment_enable = false;
    // Block transfer count is the number of beats per block (aka descriptor).
    // In this case there are two bytes per beat so divide the length by two.
    descriptor_config.block_transfer_count = self->len / self->bytes_per_sample;
    descriptor_config.source_address = ((uint32_t)self->buffer + self->len);
    descriptor_config.destination_address = ((uint32_t)&DAC->DATABUF.reg + 1);
    descriptor_config.event_output_selection = DMA_EVENT_OUTPUT_BLOCK;
    self->loop = loop;
    if (self->second_buffer == NULL) {
        if (loop) {
            descriptor_config.next_descriptor_address = ((uint32_t)audio_dma.descriptor);
        } else {
            descriptor_config.next_descriptor_address = 0;
        }
    } else {
        descriptor_config.next_descriptor_address = ((uint32_t)self->second_descriptor);
    }
    dma_descriptor_create(audio_dma.descriptor, &descriptor_config);

    if (self->second_buffer != NULL) {
        // TODO(tannewt): Correctly set the end of this.
        descriptor_config.block_transfer_count = self->len / self->bytes_per_sample;
        descriptor_config.source_address = ((uint32_t)self->second_buffer + self->len);
        descriptor_config.next_descriptor_address = ((uint32_t)audio_dma.descriptor);
        dma_descriptor_create(self->second_descriptor, &descriptor_config);

        self->last_loaded_block = 0;
        self->bytes_remaining = self->file_length;

        f_lseek(&self->file->fp, self->data_start);
        // Seek to the start of the PCM.
        UINT length_read;
        f_read(&self->file->fp, self->buffer, self->len, &length_read);
        self->bytes_remaining -= length_read;
        if (self->bytes_per_sample == 2) {
            // Undo twos complement.
            for (uint16_t i = 0; i < length_read / 2; i++) {
                self->buffer[2 * i + 1] ^= 0x80;
            }
        }

        f_read(&self->file->fp, self->second_buffer, self->len, &length_read);
        self->bytes_remaining -= length_read;
        if (self->bytes_per_sample == 2) {
            // Undo twos complement.
            for (uint16_t i = 0; i < length_read / 2; i++) {
                self->second_buffer[2 * i + 1] ^= 0x80;
            }
        }
    }
    active_audioout = self;
    dma_start_transfer_job(&audio_dma);

    if (MP_STATE_VM(audiodma_block_counter) != NULL) {
        tc_start_counter(MP_STATE_VM(audiodma_block_counter));
    }
    set_timer_frequency(self->frequency);
    tc_start_counter(MP_STATE_VM(audioout_sample_timer));
}

void common_hal_audioio_audioout_stop(audioio_audioout_obj_t* self) {
    if (active_audioout == self) {
        if (MP_STATE_VM(audiodma_block_counter) != NULL) {
            tc_stop_counter(MP_STATE_VM(audiodma_block_counter));
        }
        tc_stop_counter(MP_STATE_VM(audioout_sample_timer));
        dma_abort_job(&audio_dma);
        active_audioout = NULL;
        dac_disable(MP_STATE_VM(audioout_dac_instance));
    }
}

bool common_hal_audioio_audioout_get_playing(audioio_audioout_obj_t* self) {
    if (!dma_is_busy(&audio_dma)) {
        if (active_audioout != NULL) {
            common_hal_audioio_audioout_stop(active_audioout);
        }
        active_audioout = NULL;
    }
    return active_audioout == self;
}

void common_hal_audioio_audioout_set_frequency(audioio_audioout_obj_t* self,
                                              uint32_t frequency) {
    if (frequency == 0 || frequency > 350000) {
        mp_raise_ValueError("Unsupported playback frequency");
    }
    self->frequency = frequency;

    if (common_hal_audioio_audioout_get_playing(self)) {
        set_timer_frequency(frequency);
    }
}

uint32_t common_hal_audioio_audioout_get_frequency(audioio_audioout_obj_t* self) {
    return self->frequency;
}
