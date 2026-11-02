// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"

#include "shared-bindings/alarm/__init__.h"
#include "shared-bindings/alarm/pin/PinAlarm.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "common-hal/microcontroller/__init__.h"

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/structs/iobank0.h"
#include "hardware/structs/watchdog.h"

#if PICO_RP2350
#include "hardware/powman.h"

// Deep sleep on RP2350 wakes through powman, which has four GPIO wakeup slots.
// The alarm objects are gone by the time we enter deep sleep, so copy what we need.
#define POWMAN_WAKEUP_SLOTS (count_of(powman_hw->pwrup))
typedef struct {
    uint8_t pin_number;
    bool edge;
    bool value;
} powman_wakeup_t;
static powman_wakeup_t powman_wakeups[POWMAN_WAKEUP_SLOTS];
static size_t powman_wakeup_count;
#endif

static bool woke_up;
static uint64_t alarm_triggered_pins; // One bit per GPIO. RP2350B has 48.
static uint64_t alarm_reserved_pins; // One bit per GPIO. RP2350B has 48.
static bool _not_yet_deep_sleeping = false;

#define GPIO_IRQ_ALL_EVENTS 0x15u

static void gpio_callback(uint gpio, uint32_t events) {
    alarm_triggered_pins |= (1ULL << gpio);
    woke_up = true;

    // gpio_acknowledge_irq(gpio, events) is called automatically, before this callback is called.

    if (_not_yet_deep_sleeping) {
        // Event went off prematurely, before we went to sleep, so set it again.
        gpio_set_irq_enabled(gpio, events, false);
    } else {
        // Went off during sleep.
        // Disable IRQ automatically.
        gpio_set_irq_enabled(gpio, events, false);
        gpio_set_dormant_irq_enabled(gpio, events, false);
    }
}

void alarm_pin_pinalarm_entering_deep_sleep(void) {
    _not_yet_deep_sleeping = false;
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
        if (alarm_triggered_pins & (1ULL << alarm->pin->number)) {
            return alarms[i];
        }
    }
    return mp_const_none;
}

// Returns the lowest-numbered pin in alarm_triggered_pins, or -1 if there is none.
static int lowest_triggered_pin_number(void) {
    return alarm_triggered_pins == 0 ? -1 : __builtin_ctzll(alarm_triggered_pins);
}

mp_obj_t alarm_pin_pinalarm_record_wake_alarm(void) {
    alarm_pin_pinalarm_obj_t *const alarm = &alarm_wake_alarm.pin_alarm;

    alarm->base.type = &alarm_pin_pinalarm_type;
    alarm->pin = NULL;
    if (woke_up) {
        // A fake deep sleep doesn't reset, so gpio_callback()'s record is still here.
        alarm->pin = mcu_get_pin_by_number(lowest_triggered_pin_number());
        return alarm;
    }
    #if PICO_RP2040
    // The reset that ended deep sleep cleared alarm_triggered_pins, so use the saved copy.
    uint32_t wake_pin = watchdog_hw->scratch[RP_WKUP_PIN_SCRATCH_REG];
    if (wake_pin != 0) {
        alarm->pin = mcu_get_pin_by_number(wake_pin - 1);
    }
    #endif
    #if PICO_RP2350
    // After a powman wake, the wakeup slot that fired still holds its pin number.
    if (alarm_woke_from_powman()) {
        uint32_t pwrup = powman_hw->last_swcore_pwrup & RP_POWMAN_PWRUP_GPIO_BITS;
        for (size_t i = 0; i < POWMAN_WAKEUP_SLOTS; i++) {
            if (pwrup & (1u << (i + RP_POWMAN_PWRUP_GPIO_LSB))) {
                alarm->pin = mcu_get_pin_by_number(powman_hw->pwrup[i] & POWMAN_PWRUP0_SOURCE_BITS);
                break;
            }
        }
    }
    #endif
    return alarm;
}

void alarm_pin_pinalarm_reset(void) {
    alarm_triggered_pins = 0;
    woke_up = false;
    #if PICO_RP2040
    watchdog_hw->scratch[RP_WKUP_PIN_SCRATCH_REG] = 0;
    #endif

    // Clear all GPIO interrupts
    for (uint8_t i = 0; i < 4; i++) {
        iobank0_hw->intr[i] = 0;
    }

    // Reset pins and pin IRQs
    for (size_t i = 0; i < NUM_BANK0_GPIOS; i++) {
        if (alarm_reserved_pins & (1ULL << i)) {
            gpio_set_irq_enabled(i, GPIO_IRQ_ALL_EVENTS, false);
            reset_pin_number(i);
        }
    }
    alarm_reserved_pins = 0;

    #if PICO_RP2350
    for (size_t i = 0; i < POWMAN_WAKEUP_SLOTS; i++) {
        powman_disable_gpio_wakeup(i);
    }
    powman_wakeup_count = 0;
    #endif
}

void alarm_pin_pinalarm_set_alarms(bool deep_sleep, size_t n_alarms, const mp_obj_t *alarms) {
    #if PICO_RP2350
    powman_wakeup_count = 0;
    #endif
    for (size_t i = 0; i < n_alarms; i++) {
        if (mp_obj_is_type(alarms[i], &alarm_pin_pinalarm_type)) {
            alarm_pin_pinalarm_obj_t *alarm = MP_OBJ_TO_PTR(alarms[i]);

            #if PICO_RP2350
            if (deep_sleep) {
                if (powman_wakeup_count >= POWMAN_WAKEUP_SLOTS) {
                    mp_raise_ValueError_varg(MP_ERROR_TEXT("Too many %q"), MP_QSTR_PinAlarm);
                }
                powman_wakeups[powman_wakeup_count].pin_number = alarm->pin->number;
                powman_wakeups[powman_wakeup_count].edge = alarm->edge;
                powman_wakeups[powman_wakeup_count].value = alarm->value;
                powman_wakeup_count++;
            }
            #endif

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
            alarm_reserved_pins |= (1ULL << alarm->pin->number);

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

            _not_yet_deep_sleeping = true;
        }
    }
}

#if PICO_RP2040
void alarm_pin_pinalarm_save_wake_pin(void) {
    // No pin gives -1 + 1 = 0.
    watchdog_hw->scratch[RP_WKUP_PIN_SCRATCH_REG] = lowest_triggered_pin_number() + 1;
}
#endif

#if PICO_RP2350
// Arm the powman GPIO wakeups recorded by alarm_pin_pinalarm_set_alarms().
// Call this just before powering down for deep sleep.
void alarm_pin_pinalarm_enable_powman_wakeups(void) {
    for (size_t i = 0; i < powman_wakeup_count; i++) {
        powman_enable_gpio_wakeup(i, powman_wakeups[i].pin_number,
            powman_wakeups[i].edge, powman_wakeups[i].value);
    }
}

bool alarm_pin_pinalarm_first_powman_wakeup(uint *pin_number, bool *edge, bool *value) {
    if (powman_wakeup_count == 0) {
        return false;
    }
    *pin_number = powman_wakeups[0].pin_number;
    *edge = powman_wakeups[0].edge;
    *value = powman_wakeups[0].value;
    return true;
}
#endif
