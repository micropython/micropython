/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Lucian Copeland for Adafruit Industries
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

#include "shared-bindings/alarm/pin/PinAlarm.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "common-hal/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/structs/iobank0.h"

STATIC bool woke_up;
STATIC uint64_t alarm_triggered_pins; // 36 actual pins
STATIC uint64_t alarm_reserved_pins; // 36 actual pins
STATIC bool _pinalarm_set = false;

#define GPIO_IRQ_ALL_EVENTS 0x15u

void gpio_callback(uint gpio, uint32_t events) {
    alarm_triggered_pins |= (1 << gpio);
    woke_up = true;

    // does this need to be called, to prevent IRQ from constantly going off?
    gpio_acknowledge_irq(gpio, events);

    // Disable IRQ automatically
    gpio_set_irq_enabled(gpio, events, false);
    gpio_set_dormant_irq_enabled(gpio, events, false);
}

void common_hal_alarm_pin_pinalarm_construct(alarm_pin_pinalarm_obj_t *self, const mcu_pin_obj_t *pin, bool value, bool edge, bool pull) {
    self->pin = pin;
    self->value = value;
    self->edge = edge;
    self->pull = pull;
}

const mcu_pin_obj_t *common_hal_alarm_pin_pinalarm_get_pin(alarm_pin_pinalarm_obj_t *self) {
    return self->pin;
}

bool common_hal_alarm_pin_pinalarm_get_value(alarm_pin_pinalarm_obj_t *self) {
    return self->value;
}

bool common_hal_alarm_pin_pinalarm_get_edge(alarm_pin_pinalarm_obj_t *self) {
    return self->edge;
}

bool common_hal_alarm_pin_pinalarm_get_pull(alarm_pin_pinalarm_obj_t *self) {
    return self->pull;
}

bool alarm_pin_pinalarm_woke_this_cycle(void) {
    return woke_up;
}

mp_obj_t alarm_pin_pinalarm_find_triggered_alarm(size_t n_alarms, const mp_obj_t *alarms) {
    for (size_t i = 0; i < n_alarms; i++) {
        if (!mp_obj_is_type(alarms[i], &alarm_pin_pinalarm_type)) {
            continue;
        }
        alarm_pin_pinalarm_obj_t *alarm = MP_OBJ_TO_PTR(alarms[i]);
        if (alarm_triggered_pins & (1 << alarm->pin->number)) {
            return alarms[i];
        }
    }
    return mp_const_none;
}

mp_obj_t alarm_pin_pinalarm_create_wakeup_alarm(void) {
    alarm_pin_pinalarm_obj_t *alarm = m_new_obj(alarm_pin_pinalarm_obj_t);
    alarm->base.type = &alarm_pin_pinalarm_type;
    // TODO: how to obtain the correct pin from memory?
    return alarm;
}

void alarm_pin_pinalarm_reset(void) {
    alarm_triggered_pins = 0;
    woke_up = false;

    // Clear all GPIO interrupts
    for (uint8_t i = 0; i < 4; i++) {
        iobank0_hw->intr[i] = 0;
    }

    // Reset pins and pin IRQs
    for (size_t i = 0; i < TOTAL_GPIO_COUNT; i++) {
        if (alarm_reserved_pins & (1 << i)) {
            gpio_set_irq_enabled(i, GPIO_IRQ_ALL_EVENTS, false);
            reset_pin_number(i);
        }
    }
    alarm_reserved_pins = 0;
}

void alarm_pin_pinalarm_set_alarms(bool deep_sleep, size_t n_alarms, const mp_obj_t *alarms) {
    for (size_t i = 0; i < n_alarms; i++) {
        if (mp_obj_is_type(alarms[i], &alarm_pin_pinalarm_type)) {
            alarm_pin_pinalarm_obj_t *alarm = MP_OBJ_TO_PTR(alarms[i]);

            gpio_init(alarm->pin->number);
            if (alarm->pull) {
                // If value is high, the pullup should be off, and vice versa
                gpio_set_pulls(alarm->pin->number, !alarm->value, alarm->value);
            } else {
                // Clear in case the pulls are already on
                gpio_set_pulls(alarm->pin->number, false, false);
            }
            gpio_set_dir(alarm->pin->number, GPIO_IN);
            // Don't reset at end of VM (instead, pinalarm_reset will reset before next VM)
            common_hal_never_reset_pin(alarm->pin);
            alarm_reserved_pins |= (1 << alarm->pin->number);

            uint32_t event;
            if (alarm->value == true && alarm->edge == true) {
                event = GPIO_IRQ_EDGE_RISE;
            } else if (alarm->value == false && alarm->edge == true) {
                event = GPIO_IRQ_EDGE_FALL;
            } else if (alarm->value == true && alarm->edge == false) {
                event = GPIO_IRQ_LEVEL_HIGH;
            } else { // both false
                event = GPIO_IRQ_LEVEL_LOW;
            }

            gpio_set_irq_enabled_with_callback((uint)alarm->pin->number, event, true, &gpio_callback);
            if (deep_sleep) {
                gpio_set_dormant_irq_enabled((uint)alarm->pin->number, event, true);
            }

            _pinalarm_set = true;
        }
    }
}

bool alarm_pin_pinalarm_is_set(void) {
    return _pinalarm_set;
}
