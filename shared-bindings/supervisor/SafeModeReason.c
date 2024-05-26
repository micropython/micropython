// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/enum.h"

#include "shared-bindings/supervisor/SafeModeReason.h"

// Reuse the non-Python safe_mode_t enum
#include "supervisor/shared/safe_mode.h"

MAKE_ENUM_VALUE(supervisor_safe_mode_reason_type, safe_mode_reason, NONE, SAFE_MODE_NONE);
MAKE_ENUM_VALUE(supervisor_safe_mode_reason_type, safe_mode_reason, BROWNOUT, SAFE_MODE_BROWNOUT);
// alphabetical from here down
MAKE_ENUM_VALUE(supervisor_safe_mode_reason_type, safe_mode_reason, FLASH_WRITE_FAIL, SAFE_MODE_FLASH_WRITE_FAIL);
MAKE_ENUM_VALUE(supervisor_safe_mode_reason_type, safe_mode_reason, GC_ALLOC_OUTSIDE_VM, SAFE_MODE_GC_ALLOC_OUTSIDE_VM);
MAKE_ENUM_VALUE(supervisor_safe_mode_reason_type, safe_mode_reason, HARD_FAULT, SAFE_MODE_HARD_FAULT);
MAKE_ENUM_VALUE(supervisor_safe_mode_reason_type, safe_mode_reason, INTERRUPT_ERROR, SAFE_MODE_INTERRUPT_ERROR);
MAKE_ENUM_VALUE(supervisor_safe_mode_reason_type, safe_mode_reason, NLR_JUMP_FAIL, SAFE_MODE_NLR_JUMP_FAIL);
MAKE_ENUM_VALUE(supervisor_safe_mode_reason_type, safe_mode_reason, NO_CIRCUITPY, SAFE_MODE_NO_CIRCUITPY);
MAKE_ENUM_VALUE(supervisor_safe_mode_reason_type, safe_mode_reason, NO_HEAP, SAFE_MODE_NO_HEAP);
MAKE_ENUM_VALUE(supervisor_safe_mode_reason_type, safe_mode_reason, PROGRAMMATIC, SAFE_MODE_PROGRAMMATIC);
MAKE_ENUM_VALUE(supervisor_safe_mode_reason_type, safe_mode_reason, SDK_FATAL_ERROR, SAFE_MODE_SDK_FATAL_ERROR);
MAKE_ENUM_VALUE(supervisor_safe_mode_reason_type, safe_mode_reason, STACK_OVERFLOW, SAFE_MODE_STACK_OVERFLOW);
MAKE_ENUM_VALUE(supervisor_safe_mode_reason_type, safe_mode_reason, USB_BOOT_DEVICE_NOT_INTERFACE_ZERO, SAFE_MODE_USB_BOOT_DEVICE_NOT_INTERFACE_ZERO);
MAKE_ENUM_VALUE(supervisor_safe_mode_reason_type, safe_mode_reason, USB_TOO_MANY_ENDPOINTS, SAFE_MODE_USB_TOO_MANY_ENDPOINTS);
MAKE_ENUM_VALUE(supervisor_safe_mode_reason_type, safe_mode_reason, USB_TOO_MANY_INTERFACE_NAMES, SAFE_MODE_USB_TOO_MANY_INTERFACE_NAMES);
MAKE_ENUM_VALUE(supervisor_safe_mode_reason_type, safe_mode_reason, USER, SAFE_MODE_USER);
MAKE_ENUM_VALUE(supervisor_safe_mode_reason_type, safe_mode_reason, WATCHDOG, SAFE_MODE_WATCHDOG);


//| class SafeModeReason:
//|     """The reason that CircuitPython went into safe mode.
//|
//|     **Limitations**: Class not available on builds that do not implement ``safemode.py``.
//|     """
//|
MAKE_ENUM_MAP(supervisor_safe_mode_reason) {

//|     NONE: object
//|     """CircuitPython is not in safe mode."""
//|
    MAKE_ENUM_MAP_ENTRY(safe_mode_reason, NONE),

//|     BROWNOUT: object
//|     """The microcontroller voltage dropped too low."""
//|
    MAKE_ENUM_MAP_ENTRY(safe_mode_reason, BROWNOUT),

// alphabetical from here down

//|     FLASH_WRITE_FAIL: object
//|     """Could not write to flash memory."""
//|
    MAKE_ENUM_MAP_ENTRY(safe_mode_reason, FLASH_WRITE_FAIL),

//|     GC_ALLOC_OUTSIDE_VM: object
//|     """CircuitPython tried to allocate storage when its virtual machine was not running."""
//|
    MAKE_ENUM_MAP_ENTRY(safe_mode_reason, GC_ALLOC_OUTSIDE_VM),

//|     HARD_FAULT: object
//|     """The microcontroller detected a fault, such as an out-of-bounds memory write."""
//|
    MAKE_ENUM_MAP_ENTRY(safe_mode_reason, HARD_FAULT),

//|     INTERRUPT_ERROR: object
//|     """Internal error related to interrupts."""
//|
    MAKE_ENUM_MAP_ENTRY(safe_mode_reason, INTERRUPT_ERROR),

//|     NLR_JUMP_FAIL: object
//|     """An error occurred during exception handling, possibly due to memory corruption."""
//|
    MAKE_ENUM_MAP_ENTRY(safe_mode_reason, NLR_JUMP_FAIL),

//|     NO_CIRCUITPY: object
//|     """The CIRCUITPY drive was not available."""
//|
    MAKE_ENUM_MAP_ENTRY(safe_mode_reason, NO_CIRCUITPY),

//|     NO_HEAP: object
//|     """Heap storage was not present."""
//|
    MAKE_ENUM_MAP_ENTRY(safe_mode_reason, NO_HEAP),

//|     PROGRAMMATIC: object
//|     """The program entered safe mode using the `supervisor` module."""
//|
    MAKE_ENUM_MAP_ENTRY(safe_mode_reason, PROGRAMMATIC),

//|     SDK_FATAL_ERROR: object
//|     """Third party firmware reported a fatal error."""
//|
    MAKE_ENUM_MAP_ENTRY(safe_mode_reason, SDK_FATAL_ERROR),

//|     STACK_OVERFLOW: object
//|     """The CircuitPython heap was corrupted because the stack was too small."""
//|
    MAKE_ENUM_MAP_ENTRY(safe_mode_reason, STACK_OVERFLOW),

//|     USB_BOOT_DEVICE_NOT_INTERFACE_ZERO: object
//|     """The USB HID boot device was not set up to be the first device, on interface #0."""
//|
    MAKE_ENUM_MAP_ENTRY(safe_mode_reason, USB_BOOT_DEVICE_NOT_INTERFACE_ZERO),

//|     USB_TOO_MANY_ENDPOINTS: object
//|     """USB devices need more endpoints than are available."""
//|
    MAKE_ENUM_MAP_ENTRY(safe_mode_reason, USB_TOO_MANY_ENDPOINTS),

//|     USB_TOO_MANY_INTERFACE_NAMES: object
//|     """USB devices specify too many interface names."""
//|
    MAKE_ENUM_MAP_ENTRY(safe_mode_reason, USB_TOO_MANY_INTERFACE_NAMES),

//|     USER: object
//|     """The user pressed one or more buttons to enter safe mode.
//|     This safe mode does **not** cause ``safemode.py`` to be run, since its purpose
//|     is to prevent all user code from running.
//|     This allows errors in ``safemode.py`` to be corrected easily.
//|     """
//|
    MAKE_ENUM_MAP_ENTRY(safe_mode_reason, USER),

//|     WATCHDOG: object
//|     """An internal watchdog timer expired."""
//|
    MAKE_ENUM_MAP_ENTRY(safe_mode_reason, WATCHDOG),
};

static MP_DEFINE_CONST_DICT(supervisor_safe_mode_reason_locals_dict, supervisor_safe_mode_reason_locals_table);

MAKE_PRINTER(supervisor, supervisor_safe_mode_reason);

MAKE_ENUM_TYPE(supervisor, SafeModeReason, supervisor_safe_mode_reason);
