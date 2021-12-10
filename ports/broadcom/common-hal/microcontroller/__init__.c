/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
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

#include "shared-bindings/microcontroller/__init__.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/Processor.h"
#include "common-hal/microcontroller/__init__.h"
#include "peripherals/broadcom/interrupts.h"

#include "mphalport.h"

void common_hal_mcu_delay_us(uint32_t delay) {
    mp_hal_delay_us(delay);
}

volatile uint32_t nesting_count = 0;
void common_hal_mcu_disable_interrupts(void) {
    BP_DisableIRQs();
    nesting_count++;
}

void common_hal_mcu_enable_interrupts(void) {
    if (nesting_count == 0) {
        // reset_into_safe_mode(LOCKING_ERROR);
    }
    nesting_count--;
    if (nesting_count > 0) {
        return;
    }
    BP_EnableIRQs();
}

void common_hal_mcu_on_next_reset(mcu_runmode_t runmode) {
}

void common_hal_mcu_reset(void) {
}

// The singleton microcontroller.Processor object, bound to microcontroller.cpu
// It currently only has properties, and no state.
#if CIRCUITPY_PROCESSOR_COUNT > 1
static const mcu_processor_obj_t processor0 = {
    .base = {
        .type = &mcu_processor_type,
    },
};

static const mcu_processor_obj_t processor1 = {
    .base = {
        .type = &mcu_processor_type,
    },
};

static const mcu_processor_obj_t processor2 = {
    .base = {
        .type = &mcu_processor_type,
    },
};

static const mcu_processor_obj_t processor3 = {
    .base = {
        .type = &mcu_processor_type,
    },
};

const mp_rom_obj_tuple_t common_hal_multi_processor_obj = {
    {&mp_type_tuple},
    CIRCUITPY_PROCESSOR_COUNT,
    {
        MP_ROM_PTR(&processor0),
        MP_ROM_PTR(&processor1),
        MP_ROM_PTR(&processor2),
        MP_ROM_PTR(&processor3)
    }
};
#endif

const mcu_processor_obj_t common_hal_mcu_processor_obj = {
    .base = {
        .type = &mcu_processor_type,
    },
};

#if CIRCUITPY_NVM && CIRCUITPY_INTERNAL_NVM_SIZE > 0
// The singleton nvm.ByteArray object.
const nvm_bytearray_obj_t common_hal_mcu_nvm_obj = {
    .base = {
        .type = &nvm_bytearray_type,
    },
    .len = CIRCUITPY_INTERNAL_NVM_SIZE,
    .start_address = (uint8_t *)(CIRCUITPY_INTERNAL_NVM_START_ADDR)
};
#endif

#if CIRCUITPY_WATCHDOG
// The singleton watchdog.WatchDogTimer object.
watchdog_watchdogtimer_obj_t common_hal_mcu_watchdogtimer_obj = {
    .base = {
        .type = &watchdog_watchdogtimer_type,
    },
    .timeout = 0.0f,
    .mode = WATCHDOGMODE_NONE,
};
#endif

// This maps MCU pin names to pin objects.
#define PIN(num) { MP_ROM_QSTR(MP_QSTR_GPIO##num), MP_ROM_PTR(&pin_GPIO##num) },
const mp_rom_map_elem_t mcu_pin_global_dict_table[TOTAL_GPIO_COUNT] = {
    PIN(0)
    PIN(1)
    PIN(2)
    PIN(3)
    PIN(4)
    PIN(5)
    PIN(6)
    PIN(7)
    PIN(8)
    PIN(9)
    PIN(10)
    PIN(11)
    PIN(12)
    PIN(13)
    PIN(14)
    PIN(15)
    PIN(16)
    PIN(17)
    PIN(18)
    PIN(19)
    PIN(20)
    PIN(21)
    PIN(22)
    PIN(23)
    PIN(24)
    PIN(25)
    PIN(26)
    PIN(27)
    PIN(28)
    PIN(29)

    PIN(30)
    PIN(31)
    PIN(32)
    PIN(33)
    PIN(34)
    PIN(35)
    PIN(36)
    PIN(37)
    PIN(38)
    PIN(39)

    PIN(40)
    PIN(41)
    PIN(42)
    PIN(43)
    PIN(44)
    PIN(45)
    PIN(46)
    PIN(47)
    PIN(48)
    PIN(49)

    PIN(50)
    PIN(51)
    PIN(52)
    PIN(53)
    #if BCM_VERSION == 2711
    PIN(54)
    PIN(55)
    PIN(56)
    PIN(57)
    #endif
};
MP_DEFINE_CONST_DICT(mcu_pin_globals, mcu_pin_global_dict_table);
