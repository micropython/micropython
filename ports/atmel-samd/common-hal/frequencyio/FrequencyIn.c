/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Michael Schroeder
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

#include "shared-bindings/frequencyio/FrequencyIn.h"

#include <stdint.h>

#include "hal/include/hal_gpio.h"
#include "atmel_start_pins.h"

#include "mpconfigport.h"
#include "py/runtime.h"
#include "timer_handler.h"
#include "background.h"

#include "samd/clocks.h"
#include "samd/timers.h"
#include "samd/events.h"
#include "samd/pins.h"
#include "samd/external_interrupts.h"

#include "peripheral_clk_config.h"
#include "hpl_gclk_config.h"

#include "shared-bindings/time/__init__.h"
#include "supervisor/shared/tick.h"
#include "supervisor/shared/translate.h"

#ifdef SAMD21
#include "hpl/gclk/hpl_gclk_base.h"
#endif

static frequencyio_frequencyin_obj_t *active_frequencyins[TC_INST_NUM];
volatile uint8_t reference_tc;
#ifdef SAM_D5X_E5X
static uint8_t dpll_gclk;

#if !BOARD_HAS_CRYSTAL
static uint8_t osculp32k_gclk;
#endif

#endif

void frequencyin_reset(void) {
    for (uint8_t i = 0; i < TC_INST_NUM; i++) {
        active_frequencyins[i] = NULL;
    }

    reference_tc = 0xff;
    #ifdef SAM_D5X_E5X
    dpll_gclk = 0xff;

    #if !BOARD_HAS_CRYSTAL
    osculp32k_gclk = 0xff;
    #endif

    #endif
}

static void frequencyin_emergency_cancel_capture(uint8_t index) {
    frequencyio_frequencyin_obj_t* self = active_frequencyins[index];

    NVIC_DisableIRQ(self->TC_IRQ);
    NVIC_ClearPendingIRQ(self->TC_IRQ);
    #ifdef SAMD21
    NVIC_DisableIRQ(EIC_IRQn);
    NVIC_ClearPendingIRQ(EIC_IRQn);
    #endif
    #ifdef SAM_D5X_E5X
    NVIC_DisableIRQ(EIC_0_IRQn + self->channel);
    NVIC_ClearPendingIRQ(EIC_0_IRQn + self->channel);
    #endif

    common_hal_frequencyio_frequencyin_pause(self); // pause any further captures

    NVIC_EnableIRQ(self->TC_IRQ);
    #ifdef SAMD21
    NVIC_EnableIRQ(EIC_IRQn);
    #endif
    #ifdef SAM_D5X_E5X
    NVIC_EnableIRQ(EIC_0_IRQn + self->channel);
    #endif
    // Frequency captured is above capability. Capture paused.
    // We can't raise an error here; we're in an interrupt handler.
}

void frequencyin_interrupt_handler(uint8_t index) {
    Tc* ref_tc = tc_insts[reference_tc];

    if (!ref_tc->COUNT16.INTFLAG.bit.OVF) return; // false trigger

    uint64_t current_ns = common_hal_time_monotonic_ns();

    for (uint8_t i = 0; i < TC_INST_NUM; i++) {
        if (active_frequencyins[i] != NULL) {
            frequencyio_frequencyin_obj_t* self = active_frequencyins[i];
            Tc* tc = tc_insts[self->tc_index];

            uint32_t mask = 1 << self->channel;
            if ((EIC->INTFLAG.reg & mask) == mask) {
                // Make sure capture_period has elapsed before we
                // record a new event count.
                if ((current_ns - self->last_ns) / 1000000 >= self->capture_period) {
                    // ms difference will not need 64 bits. If we use 64 bits,
                    // double-precision float routines are required, and we don't
                    // want to include them because they're very large.
                    self->factor = (uint32_t) (current_ns - self->last_ns) / 1000000.0;
                    self->last_ns = current_ns;

                    #ifdef SAM_D5X_E5X
                    tc->COUNT16.CTRLBSET.bit.CMD = TC_CTRLBSET_CMD_READSYNC_Val;
                    while ((tc->COUNT16.SYNCBUSY.bit.COUNT == 1) ||
                           (tc->COUNT16.CTRLBSET.bit.CMD == TC_CTRLBSET_CMD_READSYNC_Val)) {
                    }
                    #endif

                    uint16_t new_freq = tc->COUNT16.COUNT.reg;
                    if ((tc->COUNT16.INTFLAG.reg & TC_INTFLAG_OVF) == 1) {
                        new_freq += 65535;
                        tc->COUNT16.INTFLAG.reg |= TC_INTFLAG_OVF;
                    }
                    self->frequency = new_freq;

                    #ifdef SAM_D5X_E5X
                    tc->COUNT16.CTRLBSET.bit.CMD = TC_CTRLBSET_CMD_RETRIGGER_Val;
                    while ((tc->COUNT16.SYNCBUSY.bit.COUNT == 1) ||
                           (tc->COUNT16.CTRLBSET.bit.CMD == TC_CTRLBSET_CMD_RETRIGGER_Val)) {
                    }
                    #endif
                }
                EIC->INTFLAG.reg |= mask;
            }

            // Check if we've reached the upper limit of detection
            if (!supervisor_background_tasks_ok() || self->errored_too_fast) {
                self->errored_too_fast = true;
                frequencyin_emergency_cancel_capture(i);
            }
        }
    }
    ref_tc->COUNT16.INTFLAG.reg |= TC_INTFLAG_OVF;
}

static void frequencyin_reference_tc_init(void) {
    if (reference_tc == 0xff) {
        return;
    }
    #ifdef SAMD21
    set_timer_handler(true, reference_tc, TC_HANDLER_FREQUENCYIN);
    turn_on_clocks(true, reference_tc, 0);
    #endif
    // use the DPLL we setup so that the reference_tc and freqin_tc(s)
    // are using the same clock frequency.
    #ifdef SAM_D5X_E5X
    set_timer_handler(true, reference_tc, TC_HANDLER_FREQUENCYIN);
    turn_on_clocks(true, reference_tc, dpll_gclk);
    #endif

    Tc *tc = tc_insts[reference_tc];
    tc_set_enable(tc, false);
    tc_reset(tc);

    #ifdef SAMD21
    tc->COUNT16.CTRLA.reg = TC_CTRLA_MODE_COUNT16 | TC_CTRLA_PRESCALER_DIV1;
    tc->COUNT16.INTENSET.bit.OVF = 1;
    NVIC_EnableIRQ(TC3_IRQn + reference_tc);
    #endif
    #ifdef SAM_D5X_E5X
    tc->COUNT16.CTRLA.reg = TC_CTRLA_MODE_COUNT16 |
                            TC_CTRLA_PRESCALER_DIV1;
    tc->COUNT16.INTENSET.bit.OVF = 1;
    NVIC_EnableIRQ(TC0_IRQn + reference_tc);
    #endif
}

static bool frequencyin_reference_tc_enabled(void) {
    if (reference_tc == 0xff) {
        return false;
    }
    Tc *tc = tc_insts[reference_tc];
    return tc->COUNT16.CTRLA.bit.ENABLE;
}

static void frequencyin_reference_tc_enable(bool enable) {
    if (reference_tc == 0xff) {
        return;
    }
    Tc *tc = tc_insts[reference_tc];
    tc_set_enable(tc, enable);
}

#ifdef SAM_D5X_E5X
static bool frequencyin_samd51_start_dpll(void) {
    if (clock_get_enabled(0, GCLK_SOURCE_DPLL1)) {
        return true;
    }

    dpll_gclk = find_free_gclk(1);
    if (dpll_gclk == 0xff) {
        return false;
    }

    // TC4-7 can only have a max of 100MHz source
    // DPLL1 frequency equation with [X]OSC32K as source: 98.304MHz = 32768(2999 + 1 + 0/32)
    // Will also enable the Lock Bypass due to low-frequency sources causing DPLL unlocks
    // as outlined in the Errata (1.12.1)
    OSCCTRL->Dpll[1].DPLLRATIO.reg = OSCCTRL_DPLLRATIO_LDRFRAC(0) | OSCCTRL_DPLLRATIO_LDR(2999);

#if BOARD_HAS_CRYSTAL
    // we can use XOSC32K directly as the source. It has already been initialized in clocks.c
    OSCCTRL->Dpll[1].DPLLCTRLB.reg =
        OSCCTRL_DPLLCTRLB_REFCLK(OSCCTRL_DPLLCTRLB_REFCLK_XOSC32_Val) | OSCCTRL_DPLLCTRLB_LBYPASS;
#else
    // We can't use OSCULP32K directly. Set up a GCLK controlled by it
    // Then use that GCLK as the reference oscillator for the DPLL.
    osculp32k_gclk = find_free_gclk(1);
    if (osculp32k_gclk == 0xff) {
        return false;
    }
    enable_clock_generator(osculp32k_gclk, GCLK_GENCTRL_SRC_OSCULP32K_Val, 1);
    GCLK->PCHCTRL[OSCCTRL_GCLK_ID_FDPLL1].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN(OSCCTRL_GCLK_ID_FDPLL1);
    OSCCTRL->Dpll[1].DPLLCTRLB.reg =
        OSCCTRL_DPLLCTRLB_REFCLK(OSCCTRL_DPLLCTRLB_REFCLK_GCLK_Val) | OSCCTRL_DPLLCTRLB_LBYPASS;
#endif

    OSCCTRL->Dpll[1].DPLLCTRLA.reg = OSCCTRL_DPLLCTRLA_ENABLE;
    while (!(OSCCTRL->Dpll[1].DPLLSTATUS.bit.LOCK || OSCCTRL->Dpll[1].DPLLSTATUS.bit.CLKRDY)) {}

    enable_clock_generator(dpll_gclk, GCLK_GENCTRL_SRC_DPLL1_Val, 1);
    return true;
}

static void frequencyin_samd51_stop_dpll(void) {
    if (!clock_get_enabled(0, GCLK_SOURCE_DPLL1)) {
        return;
    }

    if (dpll_gclk != 0xff) {
        disable_clock_generator(dpll_gclk);
        dpll_gclk = 0xff;
    }

    #if !BOARD_HAS_CRYSTAL
    if (osculp32k_gclk != 0xff) {
        disable_clock_generator(osculp32k_gclk);
        osculp32k_gclk = 0xff;
    }
    #endif

    GCLK->PCHCTRL[OSCCTRL_GCLK_ID_FDPLL1].reg = 0;
    OSCCTRL->Dpll[1].DPLLCTRLA.reg = 0;
    OSCCTRL->Dpll[1].DPLLRATIO.reg = 0;
    OSCCTRL->Dpll[1].DPLLCTRLB.reg = 0;
    while (OSCCTRL->Dpll[1].DPLLSYNCBUSY.bit.ENABLE) {
    }
}
#endif

void common_hal_frequencyio_frequencyin_construct(frequencyio_frequencyin_obj_t* self, const mcu_pin_obj_t* pin, const uint16_t capture_period) {

    if (!pin->has_extint) {
        mp_raise_RuntimeError(translate("No hardware support on pin"));
    }
    if ((capture_period == 0) || (capture_period > 500)) {
        mp_raise_ValueError(translate("Invalid capture period. Valid range: 1 - 500"));
    }
    uint32_t mask = 1 << pin->extint_channel;
    if (eic_get_enable() == 1 &&
    #ifdef SAMD21
    ((EIC->INTENSET.vec.EXTINT & mask) != 0 || (EIC->EVCTRL.vec.EXTINTEO & mask) != 0)) {
    #endif
    #ifdef SAM_D5X_E5X
    ((EIC->INTENSET.bit.EXTINT & mask) != 0 || (EIC->EVCTRL.bit.EXTINTEO & mask) != 0)) {
    #endif
        mp_raise_RuntimeError(translate("EXTINT channel already in use"));
    }

    uint8_t timer_index = find_free_timer();
    if (timer_index == 0xff) {
        mp_raise_RuntimeError(translate("All timers in use"));
    }
    Tc *tc = tc_insts[timer_index];

    self->tc_index = timer_index;
    self->pin = pin->number;
    self->channel = pin->extint_channel;
    self->errored_too_fast = false;
    self->last_ns = 0;
    self->capture_period = capture_period;
    #ifdef SAMD21
    self->TC_IRQ = TC3_IRQn + timer_index;
    #endif
    #ifdef SAM_D5X_E5X
    self->TC_IRQ = TC0_IRQn + timer_index;
    #endif

    active_frequencyins[timer_index] = self;

    // SAMD21: We use GCLK0 generated from DFLL running at 48mhz
    // SAMD51: We use a GCLK generated from DPLL1 running at <100mhz
    #ifdef SAMD21
    set_timer_handler(timer_index, 0, TC_HANDLER_NO_INTERRUPT);
    turn_on_clocks(true, timer_index, 0);
    #endif
    #ifdef SAM_D5X_E5X
    frequencyin_samd51_start_dpll();
    if (dpll_gclk == 0xff && !clock_get_enabled(0, GCLK_SOURCE_DPLL1)) {
        common_hal_frequencyio_frequencyin_deinit(self);
        mp_raise_RuntimeError(translate("No available clocks"));
    }
    set_timer_handler(timer_index, dpll_gclk, TC_HANDLER_NO_INTERRUPT);
    turn_on_clocks(true, timer_index, dpll_gclk);
    #endif

    // Ensure EIC is on
    if (eic_get_enable() == 0) {
        turn_on_external_interrupt_controller(); // enables EIC, so disable it after
    }
    eic_set_enable(false);

    uint8_t sense_setting = EIC_CONFIG_SENSE0_HIGH_Val;
    uint8_t config_index = self->channel / 8;
    uint8_t position = (self->channel % 8) * 4;
    uint32_t masked_value = EIC->CONFIG[config_index].reg & ~(0xf << position);
    EIC->CONFIG[config_index].reg = masked_value | (sense_setting << position);

    #ifdef SAMD21
    masked_value = EIC->EVCTRL.vec.EXTINTEO;
    EIC->EVCTRL.vec.EXTINTEO = masked_value | (1 << self->channel);
    #endif
    #ifdef SAM_D5X_E5X
    masked_value = EIC->EVCTRL.bit.EXTINTEO;
    EIC->EVCTRL.bit.EXTINTEO = masked_value | (1 << self->channel);
    EIC->ASYNCH.bit.ASYNCH = 1;
    #endif

    turn_on_cpu_interrupt(self->channel);

    eic_set_enable(true);

    // Turn on EVSYS
    turn_on_event_system();
    uint8_t evsys_channel = find_async_event_channel();
    #ifdef SAMD21
    connect_event_user_to_channel((EVSYS_ID_USER_TC3_EVU + timer_index), evsys_channel);
    #endif
    #ifdef SAM_D5X_E5X
    connect_event_user_to_channel((EVSYS_ID_USER_TC0_EVU + timer_index), evsys_channel);
    #endif
    init_async_event_channel(evsys_channel, (EVSYS_ID_GEN_EIC_EXTINT_0 + self->channel));
    self->event_channel = evsys_channel;

    tc_set_enable(tc, false);
    tc_reset(tc);
    #ifdef SAMD21
    tc->COUNT16.CTRLA.reg = TC_CTRLA_MODE_COUNT16 |
                            TC_CTRLA_PRESCALER_DIV1;
    tc->COUNT16.EVCTRL.bit.TCEI = 1;
    tc->COUNT16.EVCTRL.bit.EVACT = TC_EVCTRL_EVACT_COUNT_Val;
    #endif

    #ifdef SAM_D5X_E5X
    tc->COUNT16.EVCTRL.reg = TC_EVCTRL_EVACT(TC_EVCTRL_EVACT_COUNT_Val) | TC_EVCTRL_TCEI;
    tc->COUNT16.CTRLA.reg = TC_CTRLA_MODE_COUNT16 |
                            TC_CTRLA_PRESCALER_DIV1;
    #endif

    NVIC_EnableIRQ(self->TC_IRQ);

    gpio_set_pin_function(pin->number, GPIO_PIN_FUNCTION_A);

    tc_set_enable(tc, true);

    // setup reference TC if not already
    if (reference_tc == 0xff) {
        reference_tc = find_free_timer();
        if (reference_tc == 0xff) {
            common_hal_frequencyio_frequencyin_deinit(self);
            mp_raise_RuntimeError(translate("All timers in use"));
        }
        frequencyin_reference_tc_init();
    }
    if (!frequencyin_reference_tc_enabled()) {
        frequencyin_reference_tc_enable(true);
    }
}

bool common_hal_frequencyio_frequencyin_deinited(frequencyio_frequencyin_obj_t* self) {
    return self->pin == NO_PIN;
}

void common_hal_frequencyio_frequencyin_deinit(frequencyio_frequencyin_obj_t* self) {
    if (common_hal_frequencyio_frequencyin_deinited(self)) {
        return;
    }
    reset_pin_number(self->pin);

    // turn off EIC & EVSYS utilized by this TC
    disable_event_channel(self->event_channel);
    eic_set_enable(false);
    #ifdef SAMD21
    disable_event_user(EVSYS_ID_USER_TC3_EVU + self->tc_index);
    uint32_t masked_value = EIC->EVCTRL.vec.EXTINTEO;
    EIC->EVCTRL.vec.EXTINTEO = masked_value ^ (1 << self->channel);
    #endif
    #ifdef SAM_D5X_E5X
    disable_event_user(EVSYS_ID_USER_TC0_EVU + self->tc_index);
    uint32_t masked_value = EIC->EVCTRL.bit.EXTINTEO;
    EIC->EVCTRL.bit.EXTINTEO = masked_value ^ (1 << self->channel);
    NVIC_DisableIRQ(EIC_0_IRQn + self->channel);
    NVIC_ClearPendingIRQ(EIC_0_IRQn + self->channel);
    #endif
    eic_set_enable(true);
    // check if any other objects are using the EIC; if not, turn it off
    if (EIC->EVCTRL.reg == 0 && EIC->INTENSET.reg == 0) {
        eic_reset();
        turn_off_external_interrupt_controller();
    }

    // turn off the TC we were using
    Tc *tc = tc_insts[self->tc_index];
    tc_set_enable(tc, false);
    tc_reset(tc);
    NVIC_DisableIRQ(self->TC_IRQ);
    NVIC_ClearPendingIRQ(self->TC_IRQ);

    active_frequencyins[self->tc_index] = NULL;
    self->tc_index = 0xff;
    self->pin = NO_PIN;

    bool check_active = false;
    for (uint8_t i = 0; i < TC_INST_NUM; i++) {
        if (active_frequencyins[i] != NULL) {
            check_active = true;
        }
    }
    if (!check_active) {
        frequencyin_reference_tc_enable(false);
        reference_tc = 0xff;
        #ifdef SAM_D5X_E5X
        frequencyin_samd51_stop_dpll();
        #endif
    }
}

uint32_t common_hal_frequencyio_frequencyin_get_item(frequencyio_frequencyin_obj_t* self) {
    NVIC_DisableIRQ(self->TC_IRQ);
    #ifdef SAMD21
    NVIC_DisableIRQ(EIC_IRQn);
    #endif
    #ifdef SAM_D5X_E5X
    NVIC_DisableIRQ(EIC_0_IRQn + self->channel);
    #endif

    // adjust for actual capture period vs base `capture_period`
    float frequency_adjustment = 0.0;
    if (self->factor > self->capture_period) {
        float time_each_event = self->factor / self->frequency; // get the time for each event during actual period
        float capture_diff = self->factor - self->capture_period; // get the difference of actual and base periods
        // we only need to adjust if the capture_diff can contain 1 or more events
        // if so, we add how many events could have occured during the diff time
        if (time_each_event > capture_diff) {
            frequency_adjustment = capture_diff / time_each_event;
        }
    }

    float value = 1000 / (self->capture_period / (self->frequency + frequency_adjustment));

    NVIC_ClearPendingIRQ(self->TC_IRQ);
    NVIC_EnableIRQ(self->TC_IRQ);
    #ifdef SAMD21
    NVIC_ClearPendingIRQ(EIC_IRQn);
    NVIC_EnableIRQ(EIC_IRQn);
    #endif
    #ifdef SAM_D5X_E5X
    NVIC_ClearPendingIRQ(EIC_0_IRQn + self->channel);
    NVIC_EnableIRQ(EIC_0_IRQn + self->channel);
    #endif

    return value;
}

void common_hal_frequencyio_frequencyin_pause(frequencyio_frequencyin_obj_t* self) {
    Tc *tc = tc_insts[self->tc_index];
    if (!tc->COUNT16.EVCTRL.bit.TCEI) {
        return;
    }
    tc->COUNT16.EVCTRL.bit.TCEI = 0;

    #ifdef SAMD21
    uint32_t masked_value = EIC->EVCTRL.vec.EXTINTEO;
    EIC->EVCTRL.vec.EXTINTEO = masked_value ^ (1 << self->channel);
    #endif
    #ifdef SAM_D5X_E5X
    uint32_t masked_value = EIC->EVCTRL.bit.EXTINTEO;
    EIC->EVCTRL.bit.EXTINTEO = masked_value ^ (1 << self->channel);
    #endif
    return;
}

void common_hal_frequencyio_frequencyin_resume(frequencyio_frequencyin_obj_t* self) {
    Tc *tc = tc_insts[self->tc_index];
    if (tc->COUNT16.EVCTRL.bit.TCEI) {
        return;
    }
    tc->COUNT16.EVCTRL.bit.TCEI = 1;

    #ifdef SAMD21
    uint32_t masked_value = EIC->EVCTRL.vec.EXTINTEO;
    EIC->EVCTRL.vec.EXTINTEO = masked_value | (1 << self->channel);
    #endif
    #ifdef SAM_D5X_E5X
    uint32_t masked_value = EIC->EVCTRL.bit.EXTINTEO;
    EIC->EVCTRL.bit.EXTINTEO = masked_value | (1 << self->channel);
    #endif
    self->errored_too_fast = false;
    return;
}

void common_hal_frequencyio_frequencyin_clear(frequencyio_frequencyin_obj_t* self) {
    NVIC_DisableIRQ(self->TC_IRQ);
    #ifdef SAMD21
    NVIC_DisableIRQ(EIC_IRQn);
    #endif
    #ifdef SAM_D5X_E5X
    NVIC_DisableIRQ(EIC_0_IRQn + self->channel);
    #endif

    self->frequency = 0;

    NVIC_ClearPendingIRQ(self->TC_IRQ);
    NVIC_EnableIRQ(self->TC_IRQ);
    #ifdef SAMD21
    NVIC_ClearPendingIRQ(EIC_IRQn);
    NVIC_EnableIRQ(EIC_IRQn);
    #endif
    #ifdef SAM_D5X_E5X
    NVIC_ClearPendingIRQ(EIC_0_IRQn + self->channel);
    NVIC_EnableIRQ(EIC_0_IRQn + self->channel);
    #endif
    return;
}

uint16_t common_hal_frequencyio_frequencyin_get_capture_period(frequencyio_frequencyin_obj_t *self) {
    return self->capture_period;
}

void common_hal_frequencyio_frequencyin_set_capture_period(frequencyio_frequencyin_obj_t *self, uint16_t capture_period) {
    if ((capture_period == 0) || (capture_period > 500)) {
        mp_raise_ValueError(translate("Invalid capture period. Valid range: 1 - 500"));
    }

    self->capture_period = capture_period;

    common_hal_frequencyio_frequencyin_clear(self);
}
