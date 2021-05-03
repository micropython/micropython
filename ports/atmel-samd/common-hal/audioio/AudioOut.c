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

#include "extmod/vfs_fat.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "common-hal/audioio/AudioOut.h"
#include "shared-bindings/audioio/AudioOut.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "supervisor/shared/translate.h"

#include "atmel_start_pins.h"
#include "hal/include/hal_gpio.h"
#include "hpl/gclk/hpl_gclk_base.h"
#include "peripheral_clk_config.h"

#ifdef SAMD21
#include "hpl/pm/hpl_pm_base.h"
#endif

#include "audio_dma.h"
#include "timer_handler.h"

#include "samd/dma.h"
#include "samd/events.h"
#include "samd/pins.h"
#include "samd/timers.h"

#ifdef SAMD21
static void ramp_value(uint16_t start, uint16_t end) {
    start = DAC->DATA.reg;
    int32_t diff = (int32_t)end - start;
    int32_t step = 49;
    int32_t steps = diff / step;
    if (diff < 0) {
        steps = -steps;
        step = -step;
    }
    for (int32_t i = 0; i < steps; i++) {
        uint32_t value = start + step * i;
        DAC->DATA.reg = value;
        DAC->DATABUF.reg = value;
        common_hal_mcu_delay_us(50);
        RUN_BACKGROUND_TASKS;
    }
}
#endif

#ifdef SAM_D5X_E5X
static void ramp_value(uint16_t start, uint16_t end) {
    int32_t diff = (int32_t)end - start;
    int32_t step = 49;
    int32_t steps = diff / step;
    if (diff < 0) {
        steps = -steps;
        step = -step;
    }

    for (int32_t i = 0; i < steps; i++) {
        uint16_t value = start + step * i;
        DAC->DATA[0].reg = value;
        DAC->DATABUF[0].reg = value;
        DAC->DATA[1].reg = value;
        DAC->DATABUF[1].reg = value;

        common_hal_mcu_delay_us(50);
        RUN_BACKGROUND_TASKS;
    }
}
#endif

void audioout_reset(void) {
    #if defined(SAMD21) && !defined(PIN_PA02)
    return;
    #endif
    #ifdef SAMD21
    while (DAC->STATUS.reg & DAC_STATUS_SYNCBUSY) {
    }
    #endif
    #ifdef SAM_D5X_E5X
    while (DAC->SYNCBUSY.reg & DAC_SYNCBUSY_SWRST) {
    }
    #endif
    if (DAC->CTRLA.bit.ENABLE) {
        ramp_value(0x8000, 0);
    }
    DAC->CTRLA.reg |= DAC_CTRLA_SWRST;

    // TODO(tannewt): Turn off the DAC clocks to save power.
}

// Caller validates that pins are free.
void common_hal_audioio_audioout_construct(audioio_audioout_obj_t *self,
    const mcu_pin_obj_t *left_channel, const mcu_pin_obj_t *right_channel, uint16_t quiescent_value) {
    #ifdef SAM_D5X_E5X
    bool dac_clock_enabled = hri_mclk_get_APBDMASK_DAC_bit(MCLK);
    #endif

    #ifdef SAMD21
    bool dac_clock_enabled = PM->APBCMASK.bit.DAC_;
    #endif
    // Only support exclusive use of the DAC.
    if (dac_clock_enabled && DAC->CTRLA.bit.ENABLE == 1) {
        mp_raise_RuntimeError(translate("DAC already in use"));
    }
    #ifdef SAMD21
    if (right_channel != NULL) {
        mp_raise_ValueError(translate("Right channel unsupported"));
    }
    if (left_channel != &pin_PA02) {
        mp_raise_ValueError(translate("Invalid pin"));
    }
    claim_pin(left_channel);
    #endif
    #ifdef SAM_D5X_E5X
    self->right_channel = NULL;
    if (left_channel != &pin_PA02 && left_channel != &pin_PA05) {
        mp_raise_ValueError(translate("Invalid pin for left channel"));
    }
    if (right_channel != NULL && right_channel != &pin_PA02 && right_channel != &pin_PA05) {
        mp_raise_ValueError(translate("Invalid pin for right channel"));
    }
    if (right_channel == left_channel) {
        mp_raise_ValueError(translate("Cannot output both channels on the same pin"));
    }
    claim_pin(left_channel);
    if (right_channel != NULL) {
        claim_pin(right_channel);
        self->right_channel = right_channel;
        audio_dma_init(&self->right_dma);
    }
    #endif
    self->left_channel = left_channel;
    audio_dma_init(&self->left_dma);

    #ifdef SAM_D5X_E5X
    hri_mclk_set_APBDMASK_DAC_bit(MCLK);
    #endif

    #ifdef SAMD21
    _pm_enable_bus_clock(PM_BUS_APBC, DAC);
    #endif

    // SAMD51: This clock should be <= 12 MHz, per datasheet section 47.6.3.
    // SAMD21: This clock is 48mhz despite the datasheet saying it must only be <= 350kHz, per
    // datasheet table 37-6. It's incorrect because the max output rate is 350ksps and is only
    // achieved when the GCLK is more than 8mhz.
    _gclk_enable_channel(DAC_GCLK_ID, CONF_GCLK_DAC_SRC);

    DAC->CTRLA.bit.SWRST = 1;
    while (DAC->CTRLA.bit.SWRST == 1) {
    }
    // Make sure there are no outstanding access errors. (Reading DATA can cause this.)
    #ifdef SAM_D5X_E5X
    PAC->INTFLAGD.reg = PAC_INTFLAGD_DAC;
    #endif

    bool channel0_enabled = true;
    #ifdef SAM_D5X_E5X
    channel0_enabled = self->left_channel == &pin_PA02 || self->right_channel == &pin_PA02;
    bool channel1_enabled = self->left_channel == &pin_PA05 || self->right_channel == &pin_PA05;
    #endif

    if (channel0_enabled) {
        #ifdef SAMD21
        DAC->EVCTRL.reg |= DAC_EVCTRL_STARTEI;
        // We disable the voltage pump because we always run at 3.3v.
        DAC->CTRLB.reg = DAC_CTRLB_REFSEL_AVCC |
            DAC_CTRLB_LEFTADJ |
            DAC_CTRLB_EOEN |
            DAC_CTRLB_VPD;
        #endif
        #ifdef SAM_D5X_E5X
        DAC->EVCTRL.reg |= DAC_EVCTRL_STARTEI0;
        DAC->DACCTRL[0].reg = DAC_DACCTRL_CCTRL_CC12M |
            DAC_DACCTRL_ENABLE |
            DAC_DACCTRL_LEFTADJ;
        DAC->CTRLB.reg = DAC_CTRLB_REFSEL_VREFPU;
        #endif
    }
    #ifdef SAM_D5X_E5X
    if (channel1_enabled) {
        DAC->EVCTRL.reg |= DAC_EVCTRL_STARTEI1;
        DAC->DACCTRL[1].reg = DAC_DACCTRL_CCTRL_CC12M |
            DAC_DACCTRL_ENABLE |
            DAC_DACCTRL_LEFTADJ;
        DAC->CTRLB.reg = DAC_CTRLB_REFSEL_VREFPU;
    }
    #endif

    // Re-enable the DAC
    DAC->CTRLA.bit.ENABLE = 1;
    #ifdef SAMD21
    while (DAC->STATUS.bit.SYNCBUSY == 1) {
    }
    #endif
    #ifdef SAM_D5X_E5X
    while (DAC->SYNCBUSY.bit.ENABLE == 1) {
    }
    while (channel0_enabled && DAC->STATUS.bit.READY0 == 0) {
    }
    while (channel1_enabled && DAC->STATUS.bit.READY1 == 0) {
    }
    #endif

    // Use a timer to coordinate when DAC conversions occur.
    Tc *t = NULL;
    uint8_t tc_index = TC_INST_NUM;
    for (uint8_t i = TC_INST_NUM; i > 0; i--) {
        if (tc_insts[i - 1]->COUNT16.CTRLA.bit.ENABLE == 0) {
            t = tc_insts[i - 1];
            tc_index = i - 1;
            break;
        }
    }
    if (t == NULL) {
        common_hal_audioio_audioout_deinit(self);
        mp_raise_RuntimeError(translate("All timers in use"));
        return;
    }
    self->tc_index = tc_index;

    // Use the 48mhz clocks on both the SAMD21 and 51 because we will be going much slower.
    uint8_t tc_gclk = 0;
    #ifdef SAM_D5X_E5X
    tc_gclk = 1;
    #endif

    set_timer_handler(true, tc_index, TC_HANDLER_NO_INTERRUPT);
    turn_on_clocks(true, tc_index, tc_gclk);

    // Don't bother setting the period. We set it before you playback anything.
    tc_set_enable(t, false);
    tc_reset(t);
    #ifdef SAM_D5X_E5X
    t->COUNT16.WAVE.reg = TC_WAVE_WAVEGEN_MFRQ;
    #endif
    #ifdef SAMD21
    t->COUNT16.CTRLA.bit.WAVEGEN = TC_CTRLA_WAVEGEN_MFRQ_Val;
    #endif
    t->COUNT16.EVCTRL.reg = TC_EVCTRL_OVFEO;
    tc_set_enable(t, true);
    t->COUNT16.CTRLBSET.reg = TC_CTRLBSET_CMD_STOP;

    // Connect the timer overflow event, which happens at the target frequency,
    // to the DAC conversion trigger(s).
    #ifdef SAMD21
    #define FIRST_TC_GEN_ID EVSYS_ID_GEN_TC3_OVF
    #endif
    #ifdef SAM_D5X_E5X
    #define FIRST_TC_GEN_ID EVSYS_ID_GEN_TC0_OVF
    #endif
    uint8_t tc_gen_id = FIRST_TC_GEN_ID + 3 * tc_index;

    turn_on_event_system();

    // Find a free event channel. We start at the highest channels because we only need and async
    // path.
    uint8_t channel = find_async_event_channel();
    if (channel >= EVSYS_CHANNELS) {
        mp_raise_RuntimeError(translate("All event channels in use"));
    }

    #ifdef SAM_D5X_E5X
    connect_event_user_to_channel(EVSYS_ID_USER_DAC_START_1, channel);
    if (right_channel != NULL) {
        gpio_set_pin_function(self->right_channel->number, GPIO_PIN_FUNCTION_B);
    }
    #define EVSYS_ID_USER_DAC_START EVSYS_ID_USER_DAC_START_0
    #endif
    connect_event_user_to_channel(EVSYS_ID_USER_DAC_START, channel);
    gpio_set_pin_function(self->left_channel->number, GPIO_PIN_FUNCTION_B);
    init_async_event_channel(channel, tc_gen_id);

    self->tc_to_dac_event_channel = channel;

    // Ramp the DAC up.
    self->quiescent_value = quiescent_value;
    ramp_value(0, quiescent_value);

    // Leave the DMA setup to playback.
}

bool common_hal_audioio_audioout_deinited(audioio_audioout_obj_t *self) {
    return self->left_channel == NULL;
}

void common_hal_audioio_audioout_deinit(audioio_audioout_obj_t *self) {
    if (common_hal_audioio_audioout_deinited(self)) {
        return;
    }

    if (common_hal_audioio_audioout_get_playing(self)) {
        common_hal_audioio_audioout_stop(self);
    }

    // Ramp the DAC down.
    ramp_value(self->quiescent_value, 0);

    DAC->CTRLA.bit.ENABLE = 0;
    #ifdef SAMD21
    while (DAC->STATUS.bit.SYNCBUSY == 1) {
    }
    #endif
    #ifdef SAM_D5X_E5X
    while (DAC->SYNCBUSY.bit.ENABLE == 1) {
    }
    #endif

    disable_event_channel(self->tc_to_dac_event_channel);

    tc_set_enable(tc_insts[self->tc_index], false);

    reset_pin_number(self->left_channel->number);
    self->left_channel = NULL;
    #ifdef SAM_D5X_E5X
    reset_pin_number(self->right_channel->number);
    self->right_channel = NULL;
    #endif
}

static void set_timer_frequency(Tc *timer, uint32_t frequency) {
    uint32_t system_clock = 48000000;
    uint32_t new_top;
    uint8_t new_divisor;
    for (new_divisor = 0; new_divisor < 8; new_divisor++) {
        new_top = (system_clock / prescaler[new_divisor] / frequency) - 1;
        if (new_top < (1u << 16)) {
            break;
        }
    }
    uint8_t old_divisor = timer->COUNT16.CTRLA.bit.PRESCALER;
    if (new_divisor != old_divisor) {
        tc_set_enable(timer, false);
        timer->COUNT16.CTRLA.bit.PRESCALER = new_divisor;
        tc_set_enable(timer, true);
    }
    tc_wait_for_sync(timer);
    timer->COUNT16.CC[0].reg = new_top;
    tc_wait_for_sync(timer);
}

void common_hal_audioio_audioout_play(audioio_audioout_obj_t *self,
    mp_obj_t sample, bool loop) {
    if (common_hal_audioio_audioout_get_playing(self)) {
        common_hal_audioio_audioout_stop(self);
    }
    audio_dma_result result = AUDIO_DMA_OK;
    uint32_t sample_rate = audiosample_sample_rate(sample);
    #ifdef SAMD21
    uint32_t max_sample_rate = 350000;
    #endif
    #ifdef SAM_D5X_E5X
    uint32_t max_sample_rate = 1000000;
    #endif
    if (sample_rate > max_sample_rate) {
        mp_raise_ValueError_varg(translate("Sample rate too high. It must be less than %d"), max_sample_rate);
    }
    #ifdef SAMD21
    result = audio_dma_setup_playback(&self->left_dma, sample, loop, true, 0,
        false /* output unsigned */,
        (uint32_t)&DAC->DATABUF.reg,
        DAC_DMAC_ID_EMPTY);
    #endif

    #ifdef SAM_D5X_E5X
    uint32_t left_channel_reg = (uint32_t)&DAC->DATABUF[0].reg;
    uint8_t tc_trig_id = TC0_DMAC_ID_OVF + 3 * self->tc_index;
    uint8_t left_channel_trigger = tc_trig_id;
    uint32_t right_channel_reg = 0;
    uint8_t right_channel_trigger = tc_trig_id;
    if (self->left_channel == &pin_PA05) {
        left_channel_reg = (uint32_t)&DAC->DATABUF[1].reg;
    } else if (self->right_channel == &pin_PA05) {
        right_channel_reg = (uint32_t)&DAC->DATABUF[1].reg;
    }
    if (self->right_channel == &pin_PA02) {
        right_channel_reg = (uint32_t)&DAC->DATABUF[0].reg;
    }
    if (right_channel_reg == left_channel_reg + 2 && audiosample_bits_per_sample(sample) == 16) {
        result = audio_dma_setup_playback(&self->left_dma, sample, loop, false, 0,
            false /* output unsigned */,
            left_channel_reg,
            left_channel_trigger);
    } else {
        result = audio_dma_setup_playback(&self->left_dma, sample, loop, true, 0,
            false /* output unsigned */,
            left_channel_reg,
            left_channel_trigger);
        if (right_channel_reg != 0 && result == AUDIO_DMA_OK) {
            result = audio_dma_setup_playback(&self->right_dma, sample, loop, true, 1,
                false /* output unsigned */,
                right_channel_reg,
                right_channel_trigger);
        }
    }
    #endif
    if (result != AUDIO_DMA_OK) {
        audio_dma_stop(&self->left_dma);
        #ifdef SAM_D5X_E5X
        audio_dma_stop(&self->right_dma);
        #endif
        if (result == AUDIO_DMA_DMA_BUSY) {
            mp_raise_RuntimeError(translate("No DMA channel found"));
        } else if (result == AUDIO_DMA_MEMORY_ERROR) {
            mp_raise_RuntimeError(translate("Unable to allocate buffers for signed conversion"));
        }
    }
    Tc *timer = tc_insts[self->tc_index];
    set_timer_frequency(timer, audiosample_sample_rate(sample));
    timer->COUNT16.CTRLBSET.reg = TC_CTRLBSET_CMD_RETRIGGER;
    while (timer->COUNT16.STATUS.bit.STOP == 1) {
    }
    self->playing = true;
}

void common_hal_audioio_audioout_pause(audioio_audioout_obj_t *self) {
    audio_dma_pause(&self->left_dma);
    #ifdef SAM_D5X_E5X
    audio_dma_pause(&self->right_dma);
    #endif
}

void common_hal_audioio_audioout_resume(audioio_audioout_obj_t *self) {
    // Clear any overrun/underrun errors
    #ifdef SAMD21
    DAC->INTFLAG.reg = DAC_INTFLAG_UNDERRUN;
    #endif
    #ifdef SAM_D5X_E5X
    DAC->INTFLAG.reg = DAC_INTFLAG_UNDERRUN0 | DAC_INTFLAG_UNDERRUN1;
    #endif

    audio_dma_resume(&self->left_dma);
    #ifdef SAM_D5X_E5X
    audio_dma_resume(&self->right_dma);
    #endif
}

bool common_hal_audioio_audioout_get_paused(audioio_audioout_obj_t *self) {
    return audio_dma_get_paused(&self->left_dma);
}

void common_hal_audioio_audioout_stop(audioio_audioout_obj_t *self) {
    Tc *timer = tc_insts[self->tc_index];
    timer->COUNT16.CTRLBSET.reg = TC_CTRLBSET_CMD_STOP;
    audio_dma_stop(&self->left_dma);
    #ifdef SAM_D5X_E5X
    audio_dma_stop(&self->right_dma);
    #endif
    // Ramp the DAC to default. The start is ignored when the current value can be readback.
    // Otherwise, we just set it immediately.
    ramp_value(self->quiescent_value, self->quiescent_value);
}

bool common_hal_audioio_audioout_get_playing(audioio_audioout_obj_t *self) {
    bool now_playing = audio_dma_get_playing(&self->left_dma);
    if (self->playing && !now_playing) {
        common_hal_audioio_audioout_stop(self);
    }
    return now_playing;
}
