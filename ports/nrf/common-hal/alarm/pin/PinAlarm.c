/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Dan Halbert for Adafruit Industries
 * Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
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

#include "shared-bindings/alarm/pin/PinAlarm.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"

#include "nrfx.h"
#include "nrf_gpio.h"
#include "nrfx_gpiote.h"
#include "nrf_soc.h"
#include <string.h>

#include "supervisor/serial.h"  // dbg_print

#define WPIN_UNUSED 0xFF
volatile char _pinhandler_gpiote_count;
volatile nrfx_gpiote_pin_t _pinhandler_ev_pin;
#define MYGPIOTE_EV_PIN_UNDEF 0xFF

void common_hal_alarm_pin_pinalarm_construct(alarm_pin_pinalarm_obj_t *self, mcu_pin_obj_t *pin, bool value, bool edge, bool pull) {
#if 0
    if (edge) {
        mp_raise_ValueError(translate("Cannot wake on pin edge. Only level."));
    }

    if (pull && !GPIO_IS_VALID_OUTPUT_GPIO(pin->number)) {
        mp_raise_ValueError(translate("Cannot pull on input-only pin."));
    }
#endif
    self->pin = pin;
    self->value = value;
    self->pull = pull;
}

mcu_pin_obj_t *common_hal_alarm_pin_pinalarm_get_pin(alarm_pin_pinalarm_obj_t *self) {
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
  _pinhandler_ev_pin = pin;
}

bool alarm_pin_pinalarm_woke_us_up(void) {
    return (_pinhandler_gpiote_count > 0 && _pinhandler_ev_pin != MYGPIOTE_EV_PIN_UNDEF);
}

mp_obj_t alarm_pin_pinalarm_get_wakeup_alarm(size_t n_alarms, const mp_obj_t *alarms) {
    // First, check to see if we match any given alarms.
    for (size_t i = 0; i < n_alarms; i++) {
        if (!MP_OBJ_IS_TYPE(alarms[i], &alarm_pin_pinalarm_type)) {
            continue;
        }
        alarm_pin_pinalarm_obj_t *alarm  = MP_OBJ_TO_PTR(alarms[i]);
	if (alarm->pin->number == _pinhandler_ev_pin) {
            return alarms[i];
        }
    }

    alarm_pin_pinalarm_obj_t *alarm = m_new_obj(alarm_pin_pinalarm_obj_t);
    alarm->base.type = &alarm_pin_pinalarm_type;
    alarm->pin = NULL;
    // Map the pin number back to a pin object.
    for (size_t i = 0; i < mcu_pin_globals.map.used; i++) {
        const mcu_pin_obj_t* pin_obj = MP_OBJ_TO_PTR(mcu_pin_globals.map.table[i].value);
        if ((size_t) pin_obj->number == _pinhandler_ev_pin) {
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

void _setup2(void) {
  nrfx_gpiote_in_config_t cfg = {
    .sense = NRF_GPIOTE_POLARITY_TOGGLE,
    .pull = NRF_GPIO_PIN_PULLUP,
    .is_watcher = false,
    .hi_accuracy = true,
    .skip_gpio_setup = false
  };
  for(size_t i = 0; i < 64; ++i) {
    uint64_t mask = 1ull << i;
    int pull = 0;
    int sense = 0;
    if (((high_alarms & mask) == 0) && ((low_alarms & mask) == 0)) {
      continue;
    }
    if (((high_alarms & mask) != 0) && ((low_alarms & mask) == 0)) {
      cfg.sense = NRF_GPIOTE_POLARITY_LOTOHI;
      cfg.pull = ((pull_pins & mask) != 0) ? NRF_GPIO_PIN_PULLDOWN : NRF_GPIO_PIN_NOPULL;
      pull = -1; sense = 1;
    }
    else
    if (((high_alarms & mask) == 0) && ((low_alarms & mask) != 0)) {
      cfg.sense = NRF_GPIOTE_POLARITY_HITOLO;
      cfg.pull = ((pull_pins & mask) != 0) ? NRF_GPIO_PIN_PULLUP : NRF_GPIO_PIN_NOPULL;
      pull = 1; sense = -1;
    }
    else {
      cfg.sense = NRF_GPIOTE_POLARITY_TOGGLE;
      cfg.pull = NRF_GPIO_PIN_NOPULL;
      sense = 9;
    }
    nrfx_gpiote_in_init((nrfx_gpiote_pin_t)i, &cfg, pinalarm_gpiote_handler);
    nrfx_gpiote_in_event_enable((nrfx_gpiote_pin_t)i, true);
    printf("pin=%d, sense=%d, pull=%d\r\n", i, sense, pull);
  }
}

void _setup_pin1_for_lightsleep(void) {
  if ( nrfx_gpiote_is_init() ) {
    nrfx_gpiote_uninit();
  }
  nrfx_gpiote_init(NRFX_GPIOTE_CONFIG_IRQ_PRIORITY);

  _pinhandler_gpiote_count = 0;
  _pinhandler_ev_pin = MYGPIOTE_EV_PIN_UNDEF;
  _setup2();
}

void _setup_pin1_for_deepsleep(void) {
    for(size_t i = 0; i < 64; ++i) {
      uint64_t mask = 1ull << i;
      int pull = 0;
      int sense = 0;
      if (((high_alarms & mask) == 0) && ((low_alarms & mask) == 0)) {
	continue;
      }
      if (((high_alarms & mask) != 0) && ((low_alarms & mask) == 0)) {
	pull = ((pull_pins & mask) != 0) ? NRF_GPIO_PIN_PULLDOWN : NRF_GPIO_PIN_NOPULL;
	nrf_gpio_cfg_input((uint32_t)i, (nrf_gpio_pin_pull_t)pull);
	nrf_gpio_cfg_sense_set((uint32_t)i, NRF_GPIO_PIN_SENSE_HIGH);
	sense = NRF_GPIO_PIN_SENSE_HIGH;
      }
      else
      if (((high_alarms & mask) == 0) && ((low_alarms & mask) != 0)) {
	pull = ((pull_pins & mask) != 0) ? NRF_GPIO_PIN_PULLUP : NRF_GPIO_PIN_NOPULL;
	nrf_gpio_cfg_input((uint32_t)i, (nrf_gpio_pin_pull_t)pull);
	nrf_gpio_cfg_sense_set((uint32_t)i, NRF_GPIO_PIN_SENSE_LOW);
        sense = NRF_GPIO_PIN_SENSE_LOW;
      }
      printf("pin=%d, sense=%d, pull=%d\r\n", i, sense, pull);
    }
#if 0
    uint32_t pin_number = 2;
    NRF_GPIO_Type * reg = nrf_gpio_pin_port_decode(&pin_number);
    dbg_printf(" 2 PIN_CNF=0x%08X\r\n", (unsigned int)(reg->PIN_CNF[pin_number]));
    pin_number = 28;
    reg = nrf_gpio_pin_port_decode(&pin_number);
    dbg_printf("28 PIN_CNF=0x%08X\r\n", (unsigned int)(reg->PIN_CNF[pin_number]));
#endif
}

void alarm_pin_pinalarm_set_alarms(bool deep_sleep, size_t n_alarms, const mp_obj_t *alarms) {
    // Bitmask of wake up settings.
    size_t high_count = 0;
    size_t low_count = 0;
    int pin_number = -1;

    for (size_t i = 0; i < n_alarms; i++) {
        if (!MP_OBJ_IS_TYPE(alarms[i], &alarm_pin_pinalarm_type)) {
            continue;
        }
        alarm_pin_pinalarm_obj_t *alarm  = MP_OBJ_TO_PTR(alarms[i]);

        pin_number = alarm->pin->number;
	dbg_printf("alarm_pin_pinalarm_set_alarms(pin#=%d, val=%d, pull=%d)\r\n", pin_number, alarm->value, alarm->pull);
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
	_setup_pin1_for_lightsleep();
      }
      else {
	//_setup_pin1_for_deepsleep(pin_number);
      }
    }
    else {
      dbg_printf("alarm_pin_pinalarm_set_alarms() no valid pins\r\n");
    }
}

void alarm_pin_pinalarm_prepare_for_deep_sleep(void) {
  _setup_pin1_for_deepsleep();
}
