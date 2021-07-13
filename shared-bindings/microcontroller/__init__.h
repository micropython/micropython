/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_MICROCONTROLLER___INIT___H
#define MICROPY_INCLUDED_SHARED_BINDINGS_MICROCONTROLLER___INIT___H

#include "py/obj.h"
#include "py/mpconfig.h"
#include "py/objtuple.h"

#include "common-hal/microcontroller/Processor.h"
#include "shared-bindings/microcontroller/ResetReason.h"
#include "shared-bindings/microcontroller/RunMode.h"

extern void common_hal_mcu_delay_us(uint32_t);

extern void common_hal_mcu_disable_interrupts(void);
extern void common_hal_mcu_enable_interrupts(void);

extern void common_hal_mcu_on_next_reset(mcu_runmode_t runmode);
extern void common_hal_mcu_reset(void);

extern const mp_obj_dict_t mcu_pin_globals;

#if CIRCUITPY_PROCESSOR_COUNT == 1
extern const mcu_processor_obj_t common_hal_mcu_processor_obj;
#elif CIRCUITPY_PROCESSOR_COUNT > 1
extern const mcu_processor_obj_t common_hal_mcu_processor_obj;
extern const mp_rom_obj_tuple_t common_hal_multi_processor_obj;
#else
#error "Invalid processor count"
#endif


#if CIRCUITPY_INTERNAL_NVM_SIZE > 0
#include "common-hal/nvm/ByteArray.h"
extern const nvm_bytearray_obj_t common_hal_mcu_nvm_obj;
#endif

#if CIRCUITPY_WATCHDOG
#include "common-hal/watchdog/WatchDogTimer.h"
extern watchdog_watchdogtimer_obj_t common_hal_mcu_watchdogtimer_obj;
#endif

#endif  // MICROPY_INCLUDED_SHARED_BINDINGS_MICROCONTROLLER___INIT___H
