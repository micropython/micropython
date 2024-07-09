// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

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


#if CIRCUITPY_NVM && CIRCUITPY_INTERNAL_NVM_SIZE > 0
#include "common-hal/nvm/ByteArray.h"
extern const nvm_bytearray_obj_t common_hal_mcu_nvm_obj;
#endif

#if CIRCUITPY_WATCHDOG
#include "common-hal/watchdog/WatchDogTimer.h"
extern watchdog_watchdogtimer_obj_t common_hal_mcu_watchdogtimer_obj;
#endif
