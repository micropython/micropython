// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_MCU_RESET_REASON__H
#define MICROPY_INCLUDED_SHARED_BINDINGS_MCU_RESET_REASON__H

#include "py/obj.h"
#include "py/enum.h"

typedef enum {
    RESET_REASON_POWER_ON,
    RESET_REASON_BROWNOUT,
    RESET_REASON_SOFTWARE,
    RESET_REASON_DEEP_SLEEP_ALARM,
    RESET_REASON_RESET_PIN,
    RESET_REASON_WATCHDOG,
    RESET_REASON_UNKNOWN,
    RESET_REASON_RESCUE_DEBUG,
} mcu_reset_reason_t;

extern const mp_obj_type_t mcu_reset_reason_type;

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_MCU_RESET_REASON__H
