/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Dan Halbert for Adafruit Industries
 * Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2021 Junji Sakai
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

#include "py/runtime.h"
#include <stdio.h>
#include <assert.h>

#include "shared-bindings/alarm/pin/PinAlarm.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "common-hal/alarm/__init__.h"

#include "nrfx.h"
#include "nrf_gpio.h"
#include "nrfx_gpiote.h"
#include "nrf_soc.h"
#include <string.h>

#include "supervisor/serial.h"  // dbg_print

#define WPIN_UNUSED 0xFF
volatile char _pinhandler_gpiote_count;
static bool pins_configured = false;

extern uint32_t reset_reason_saved;
extern void dbg_dump_GPIOregs(void);

void common_hal_alarm_pin_pinalarm_construct(alarm_pin_pinalarm_obj_t *self, const mcu_pin_obj_t *pin, bool value, bool edge, bool pull) {
    if (edge) {
        mp_raise_ValueError(translate("Cannot wake on pin edge. Only level."));
    }
    if (pin->number >= NUMBER_OF_PINS) {
        mp_raise_ValueError(translate("Invalid pin"));
    }
    self->pin = pin;
    self->value = value;
    self->pull = pull;
}

const mcu_pin_obj_t *common_hal_alarm_pin_pinalarm_get_pin(alarm_pin_pinalarm_obj_t *self) {
    return self->pin;
}

bool common_hal_alarm_pin_pinalarm_get_value(alarm_pin_pinalarm_obj_t *self) {
    return self->value;
}

bool common_hal_alarm_pin_pinalarm_get_edge(alarm_pin_pinalarm_obj_t *self) {
    return false;
}

bool common_hal_alarm_pin_pinalarm_get_pull(alarm_pin_pinalarm_obj_t *self) {
    return self->pull;
}


static void pinalarm_gpiote_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
    ++_pinhandler_gpiote_count;
    sleepmem_wakeup_event = SLEEPMEM_WAKEUP_BY_PIN;
    sleepmem_wakeup_pin = pin & 0xFF;
}

bool alarm_pin_pinalarm_woke_this_cycle(void) {
    return sleepmem_wakeup_event == SLEEPMEM_WAKEUP_BY_PIN &&
           sleepmem_wakeup_pin != WAKEUP_PIN_UNDEF;
}

mp_obj_t alarm_pin_pinalarm_find_triggered_alarm(size_t n_alarms, const mp_obj_t *alarms) {
    // First, check to see if we match any given alarms.
    for (size_t i = 0; i < n_alarms; i++) {
        if (!mp_obj_is_type(alarms[i], &alarm_pin_pinalarm_type)) {
            continue;
        }
        alarm_pin_pinalarm_obj_t *alarm = MP_OBJ_TO_PTR(alarms[i]);
        if (alarm->pin->number == sleepmem_wakeup_pin) {
            return alarms[i];
        }
    }
    return mp_const_none;
}

mp_obj_t alarm_pin_pinalarm_create_wakeup_alarm(void) {
    alarm_pin_pinalarm_obj_t *alarm = m_new_obj(alarm_pin_pinalarm_obj_t);
    alarm->base.type = &alarm_pin_pinalarm_type;
    alarm->pin = NULL;
    // Map the pin number back to a pin object.
    for (size_t i = 0; i < mcu_pin_globals.map.used; i++) {
        const mcu_pin_obj_t *pin_obj = MP_OBJ_TO_PTR(mcu_pin_globals.map.table[i].value);
        if ((size_t)pin_obj->number == sleepmem_wakeup_pin) {
            alarm->pin = mcu_pin_globals.map.table[i].value;
            break;
        }
    }
    return alarm;
}

// These must be static because we need to configure pulls later, right before
// deep sleep.
static uint64_t high_alarms = 0;
static uint64_t low_alarms = 0;
static uint64_t pull_pins = 0;

void alarm_pin_pinalarm_reset(void) {
    for (size_t i = 0; i < 64; i++) {
        uint64_t mask = 1ull << i;
        bool high = (high_alarms & mask) != 0;
        bool low = (low_alarms & mask) != 0;
        if (!(high || low)) {
            continue;
        }
        reset_pin_number(i);
        nrfx_gpiote_in_event_disable((nrfx_gpiote_pin_t)i);
        nrfx_gpiote_in_uninit((nrfx_gpiote_pin_t)i);
    }

    high_alarms = 0;
    low_alarms = 0;
    pull_pins = 0;
}

static void configure_pins_for_sleep(void) {
    nrfx_err_t err;
    if (nrfx_gpiote_is_init()) {
        nrfx_gpiote_uninit();
    }
    err = nrfx_gpiote_init(NRFX_GPIOTE_CONFIG_IRQ_PRIORITY);
    assert(err == NRFX_SUCCESS);
    (void)err; // to suppress unused warning

    _pinhandler_gpiote_count = 0;

    nrfx_gpiote_in_config_t cfg = {
        .sense = NRF_GPIOTE_POLARITY_TOGGLE,
        .pull = NRF_GPIO_PIN_PULLUP,
        .is_watcher = false,
        .hi_accuracy = true,
        .skip_gpio_setup = false
    };
    for (size_t i = 0; i < 64; ++i) {
        uint64_t mask = 1ull << i;
        if (((high_alarms & mask) == 0) && ((low_alarms & mask) == 0)) {
            continue;
        }
        if (((high_alarms & mask) != 0) && ((low_alarms & mask) == 0)) {
            cfg.sense = NRF_GPIOTE_POLARITY_LOTOHI;
            cfg.pull = ((pull_pins & mask) != 0) ?
                NRF_GPIO_PIN_PULLDOWN : NRF_GPIO_PIN_NOPULL;
        } else
        if (((high_alarms & mask) == 0) && ((low_alarms & mask) != 0)) {
            cfg.sense = NRF_GPIOTE_POLARITY_HITOLO;
            cfg.pull = ((pull_pins & mask) != 0) ?
                NRF_GPIO_PIN_PULLUP : NRF_GPIO_PIN_NOPULL;
        } else {
            cfg.sense = NRF_GPIOTE_POLARITY_TOGGLE;
            cfg.pull = NRF_GPIO_PIN_NOPULL;
        }
        err = nrfx_gpiote_in_init((nrfx_gpiote_pin_t)i, &cfg,
            pinalarm_gpiote_handler);
        assert(err == NRFX_SUCCESS);
        nrfx_gpiote_in_event_enable((nrfx_gpiote_pin_t)i, true);
        if (((high_alarms & mask) != 0) && ((low_alarms & mask) == 0)) {
            nrf_gpio_cfg_sense_set((uint32_t)i, NRF_GPIO_PIN_SENSE_HIGH);
        }
        if (((high_alarms & mask) == 0) && ((low_alarms & mask) != 0)) {
            nrf_gpio_cfg_sense_set((uint32_t)i, NRF_GPIO_PIN_SENSE_LOW);
        }
    }
}

void alarm_pin_pinalarm_set_alarms(bool deep_sleep, size_t n_alarms, const mp_obj_t *alarms) {
    // Bitmask of wake up settings.
    size_t high_count = 0;
    size_t low_count = 0;
    int pin_number = -1;

    for (size_t i = 0; i < n_alarms; i++) {
        if (!mp_obj_is_type(alarms[i], &alarm_pin_pinalarm_type)) {
            continue;
        }
        alarm_pin_pinalarm_obj_t *alarm = MP_OBJ_TO_PTR(alarms[i]);

        pin_number = alarm->pin->number;
        // dbg_printf("alarm_pin_pinalarm_set_alarms(pin#=%d, val=%d, pull=%d)\r\n", pin_number, alarm->value, alarm->pull);
        if (alarm->value) {
            high_alarms |= 1ull << pin_number;
            high_count++;
        } else {
            low_alarms |= 1ull << pin_number;
            low_count++;
        }
        if (alarm->pull) {
            pull_pins |= 1ull << pin_number;
        }
    }
    if (pin_number != -1) {
        if (!deep_sleep) {
            configure_pins_for_sleep();
        } else {
            // we don't setup gpio HW here but do them in
            // alarm_pin_pinalarm_prepare_for_deep_sleep() below
            reset_reason_saved = 0;
            pins_configured = false;
        }
    } else {
        // dbg_printf("alarm_pin_pinalarm_set_alarms() no valid pins\r\n");
    }
}

void alarm_pin_pinalarm_prepare_for_deep_sleep(void) {
    if (!pins_configured) {
        configure_pins_for_sleep();
        pins_configured = true;
        #ifdef NRF_DEBUG_PRINT
        // dbg_dump_GPIOregs();
        #endif
    }
}
