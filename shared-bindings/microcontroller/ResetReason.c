// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/enum.h"

#include "shared-bindings/microcontroller/ResetReason.h"

MAKE_ENUM_VALUE(mcu_reset_reason_type, reset_reason, POWER_ON, RESET_REASON_POWER_ON);
MAKE_ENUM_VALUE(mcu_reset_reason_type, reset_reason, BROWNOUT, RESET_REASON_BROWNOUT);
MAKE_ENUM_VALUE(mcu_reset_reason_type, reset_reason, SOFTWARE, RESET_REASON_SOFTWARE);
MAKE_ENUM_VALUE(mcu_reset_reason_type, reset_reason, DEEP_SLEEP_ALARM, RESET_REASON_DEEP_SLEEP_ALARM);
MAKE_ENUM_VALUE(mcu_reset_reason_type, reset_reason, RESET_PIN, RESET_REASON_RESET_PIN);
MAKE_ENUM_VALUE(mcu_reset_reason_type, reset_reason, WATCHDOG, RESET_REASON_WATCHDOG);
MAKE_ENUM_VALUE(mcu_reset_reason_type, reset_reason, UNKNOWN, RESET_REASON_UNKNOWN);
MAKE_ENUM_VALUE(mcu_reset_reason_type, reset_reason, RESCUE_DEBUG, RESET_REASON_RESCUE_DEBUG);

//| class ResetReason:
//|     """The reason the microcontroller was last reset"""
//|
//|     POWER_ON: object
//|     """The microcontroller was started from power off."""
//|
//|     BROWNOUT: object
//|     """The microcontroller was reset due to too low a voltage."""
//|
//|     SOFTWARE: object
//|     """The microcontroller was reset from software."""
//|
//|     DEEP_SLEEP_ALARM: object
//|     """The microcontroller was reset for deep sleep and restarted by an alarm."""
//|
//|     RESET_PIN: object
//|     """The microcontroller was reset by a signal on its reset pin. The pin might be connected to a reset button."""
//|
//|     WATCHDOG: object
//|     """The microcontroller was reset by its watchdog timer."""
//|
//|     UNKNOWN: object
//|     """The microcontroller restarted for an unknown reason."""
//|
//|     RESCUE_DEBUG: object
//|     """The microcontroller was reset by the rescue debug port."""
//|
MAKE_ENUM_MAP(mcu_reset_reason) {
    MAKE_ENUM_MAP_ENTRY(reset_reason, POWER_ON),
    MAKE_ENUM_MAP_ENTRY(reset_reason, BROWNOUT),
    MAKE_ENUM_MAP_ENTRY(reset_reason, SOFTWARE),
    MAKE_ENUM_MAP_ENTRY(reset_reason, DEEP_SLEEP_ALARM),
    MAKE_ENUM_MAP_ENTRY(reset_reason, RESET_PIN),
    MAKE_ENUM_MAP_ENTRY(reset_reason, WATCHDOG),
    MAKE_ENUM_MAP_ENTRY(reset_reason, UNKNOWN),
    MAKE_ENUM_MAP_ENTRY(reset_reason, RESCUE_DEBUG),
};
static MP_DEFINE_CONST_DICT(mcu_reset_reason_locals_dict, mcu_reset_reason_locals_table);

MAKE_PRINTER(microcontroller, mcu_reset_reason);

MAKE_ENUM_TYPE(microcontroller, ResetReason, mcu_reset_reason);
