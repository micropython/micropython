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

#include "atmel_start_pins.h"
#include "hal/include/hal_gpio.h"
#include "hpl/gclk/hpl_gclk_base.h"
#include "peripheral_clk_config.h"

#ifdef SAMD21
#include "hpl/pm/hpl_pm_base.h"
#endif

#include "audio_dma.h"
#include "events.h"
#include "samd21_pins.h"
#include "shared_dma.h"
#include "timers.h"

void audioout_reset(void) {
}

void common_hal_audioio_audioout_construct(audioio_audioout_obj_t* self,
        const mcu_pin_obj_t* left_channel, const mcu_pin_obj_t* right_channel) {
    #ifdef SAMD51
    bool dac_clock_enabled = hri_mclk_get_APBDMASK_DAC_bit(MCLK);
    #endif

    #ifdef SAMD21
    bool dac_clock_enabled = PM->APBCMASK.bit.DAC_;
    #endif
    // Only support exclusive use of the DAC.
    if (dac_clock_enabled && DAC->CTRLA.bit.ENABLE == 1) {
        mp_raise_RuntimeError("DAC already in use");
    }
    #ifdef SAMD21
    if (right_channel != NULL) {
        mp_raise_ValueError("Right channel unsupported");
    }
    if (left_channel != &pin_PA02) {
        mp_raise_ValueError("Invalid pin");
    }
    assert_pin_free(left_channel);
    claim_pin(left_channel);
    #endif
    #ifdef SAMD51
    self->right_channel = NULL;
    if (left_channel != &pin_PA02 && left_channel != &pin_PA05) {
        mp_raise_ValueError("Invalid pin for left channel");
    }
    assert_pin_free(left_channel);
    if (right_channel != NULL && right_channel != &pin_PA02 && right_channel != &pin_PA05) {
        mp_raise_ValueError("Invalid pin for right channel");
    }
    if (right_channel == left_channel) {
        mp_raise_ValueError("Cannot output both channels on the same pin");
    }
    claim_pin(left_channel);
    if (right_channel != NULL) {
        claim_pin(right_channel);
        self->right_channel = right_channel;
        gpio_set_pin_function(self->right_channel->pin, GPIO_PIN_FUNCTION_B);
        audio_dma_init(&self->right_dma);
    }
    #endif
    self->left_channel = left_channel;
    gpio_set_pin_function(self->left_channel->pin, GPIO_PIN_FUNCTION_B);
    audio_dma_init(&self->left_dma);

    #ifdef SAMD51
    hri_mclk_set_APBDMASK_DAC_bit(MCLK);
    #endif

    #ifdef SAMD21
    _pm_enable_bus_clock(PM_BUS_APBC, DAC);
    #endif

    // SAMD21: This clock should be <= 12 MHz, per datasheet section 47.6.3.
    // SAMD51: This clock should be <= 350kHz, per datasheet table 37-6.
    _gclk_enable_channel(DAC_GCLK_ID, CONF_GCLK_DAC_SRC);


        DAC->CTRLA.bit.SWRST = 1;
        while (DAC->CTRLA.bit.SWRST == 1) {}

    bool channel0_enabled = true;
    #ifdef SAMD51
    channel0_enabled = self->left_channel == &pin_PA02 || self->right_channel == &pin_PA02;
    bool channel1_enabled = self->left_channel == &pin_PA05 || self->right_channel == &pin_PA05;
    #endif

    if (channel0_enabled) {
        #ifdef SAMD21
        DAC->EVCTRL.reg |= DAC_EVCTRL_STARTEI;
        DAC->CTRLB.reg = DAC_CTRLB_REFSEL_AVCC |
                         DAC_CTRLB_LEFTADJ |
                         DAC_CTRLB_EOEN;
        #endif
        #ifdef SAMD51
        DAC->EVCTRL.reg |= DAC_EVCTRL_STARTEI0;
        DAC->DACCTRL[0].reg = DAC_DACCTRL_CCTRL_CC1M |
                              DAC_DACCTRL_ENABLE |
                              DAC_DACCTRL_LEFTADJ;
        DAC->CTRLB.reg = DAC_CTRLB_REFSEL_VREFPU;
        #endif
    }
    #ifdef SAMD51
    if (channel1_enabled) {
        DAC->EVCTRL.reg |= DAC_EVCTRL_STARTEI1;
        DAC->DACCTRL[1].reg = DAC_DACCTRL_CCTRL_CC1M |
                              DAC_DACCTRL_ENABLE |
                              DAC_DACCTRL_LEFTADJ;
        DAC->CTRLB.reg = DAC_CTRLB_REFSEL_VREFPU;
    }
    #endif

    // Re-enable the DAC
    DAC->CTRLA.bit.ENABLE = 1;
    #ifdef SAMD21
    while (DAC->STATUS.bit.SYNCBUSY == 1) {}
    #endif
    #ifdef SAMD51
    while (DAC->SYNCBUSY.bit.ENABLE == 1) {}
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
        mp_raise_RuntimeError("All timers in use");
        return;
    }
    self->tc_index = tc_index;

    // Use the 48mhz clocks on both the SAMD21 and 51 because we will be going much slower.
    uint8_t tc_gclk = 0;
    #ifdef SAMD51
    tc_gclk = 1;
    #endif

    turn_on_clocks(true, tc_index, tc_gclk);

    // Don't bother setting the period. We set it before you playback anything.
    tc_set_enable(t, false);
    tc_reset(t);
    #ifdef SAMD51
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
    #ifdef SAMD51
    #define FIRST_TC_GEN_ID EVSYS_ID_GEN_TC0_OVF
    #endif
    uint8_t tc_gen_id = FIRST_TC_GEN_ID + 3 * tc_index;

    turn_on_event_system();

    // Find a free event channel. We start at the highest channels because we only need and async
    // path.
    uint8_t channel = find_async_event_channel();
    #ifdef SAMD51
    connect_event_user_to_channel(EVSYS_ID_USER_DAC_START_1, channel);
    #define EVSYS_ID_USER_DAC_START EVSYS_ID_USER_DAC_START_0
    #endif
    connect_event_user_to_channel(EVSYS_ID_USER_DAC_START, channel);
    init_async_event_channel(channel, tc_gen_id);

    self->tc_to_dac_event_channel = channel;

    // Leave the DMA setup to playback.
}

bool common_hal_audioio_audioout_deinited(audioio_audioout_obj_t* self) {
    return self->left_channel == mp_const_none;
}

void common_hal_audioio_audioout_deinit(audioio_audioout_obj_t* self) {
    if (common_hal_audioio_audioout_deinited(self)) {
        return;
    }

    DAC->CTRLA.bit.ENABLE = 0;
    #ifdef SAMD21
    while (DAC->STATUS.bit.SYNCBUSY == 1) {}
    #endif
    #ifdef SAMD51
    while (DAC->SYNCBUSY.bit.ENABLE == 1) {}
    #endif

    disable_event_channel(self->tc_to_dac_event_channel);

    reset_pin(self->left_channel->pin);
    self->left_channel = mp_const_none;
    #ifdef SAMD51
    reset_pin(self->right_channel->pin);
    self->right_channel = mp_const_none;
    #endif
}

static void set_timer_frequency(Tc* timer, uint32_t frequency) {
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

void common_hal_audioio_audioout_play(audioio_audioout_obj_t* self,
                                      mp_obj_t sample, bool loop) {
    if (common_hal_audioio_audioout_get_playing(self)) {
        common_hal_audioio_audioout_stop(self);
    }
    audio_dma_result result = AUDIO_DMA_OK;
    #ifdef SAMD21
    result = audio_dma_setup_playback(&self->left_dma, sample, loop, true, 0,
                                      false /* output unsigned */,
                                      (uint32_t) &DAC->DATABUF.reg,
                                      DAC_DMAC_ID_EMPTY);
    #endif

    #ifdef SAMD51
    uint32_t left_channel_reg = (uint32_t) &DAC->DATABUF[0].reg;
    uint8_t left_channel_trigger = DAC_DMAC_ID_EMPTY_0;
    uint32_t right_channel_reg = 0;
    uint8_t right_channel_trigger = 0;
    if (self->left_channel == &pin_PA05) {
        left_channel_reg = (uint32_t) &DAC->DATABUF[1].reg;
        left_channel_trigger = DAC_DMAC_ID_EMPTY_1;
    } else if (self->right_channel == &pin_PA05) {
        right_channel_reg = (uint32_t) &DAC->DATABUF[1].reg;
        right_channel_trigger = DAC_DMAC_ID_EMPTY_1;
    }
    if (self->right_channel == &pin_PA02) {
        right_channel_reg = (uint32_t) &DAC->DATABUF[0].reg;
        right_channel_trigger = DAC_DMAC_ID_EMPTY_0;
    }
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
    #endif
    if (result != AUDIO_DMA_OK) {
        audio_dma_stop(&self->left_dma);
        #ifdef SAMD51
        audio_dma_stop(&self->right_dma);
        #endif
        if (result == AUDIO_DMA_DMA_BUSY) {
            mp_raise_RuntimeError("No DMA channel found");
        } else if (result == AUDIO_DMA_MEMORY_ERROR) {
            mp_raise_RuntimeError("Unable to allocate buffers for signed conversion");
        }
    }
    Tc* timer = tc_insts[self->tc_index];
    set_timer_frequency(timer, audiosample_sample_rate(sample));
    timer->COUNT16.CTRLBSET.reg = TC_CTRLBSET_CMD_RETRIGGER;
    while (timer->COUNT16.STATUS.bit.STOP == 1) {}
    self->playing = true;
}

void common_hal_audioio_audioout_stop(audioio_audioout_obj_t* self) {
    Tc* timer = tc_insts[self->tc_index];
    timer->COUNT16.CTRLBSET.reg = TC_CTRLBSET_CMD_STOP;
    audio_dma_stop(&self->left_dma);
    #ifdef SAMD51
    audio_dma_stop(&self->right_dma);
    #endif
}

bool common_hal_audioio_audioout_get_playing(audioio_audioout_obj_t* self) {
    bool now_playing = audio_dma_get_playing(&self->left_dma);
    if (self->playing && !now_playing) {
        common_hal_audioio_audioout_stop(self);
    }
    return now_playing;
}
