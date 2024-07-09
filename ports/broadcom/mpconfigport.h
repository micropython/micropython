// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Definitions that control circuitpy_mpconfig.h:

////////////////////////////////////////////////////////////////////////////////////////////////////


#define CIRCUITPY_MCU_FAMILY                        broadcom
#define MICROPY_PY_SYS_PLATFORM                     "BROADCOM"
#define MICROPY_PY_BUILTINS_NOTIMPLEMENTED          (1)
#define MICROPY_PY_FUNCTION_ATTRS                   (1)
#if BCM_VERSION == 2837 || BCM_VERSION == 2711
#define MICROPY_OBJ_REPR            (MICROPY_OBJ_REPR_A)
#elif BCM_VERSION == 2835
#define MICROPY_OBJ_REPR            (MICROPY_OBJ_REPR_C)
#endif
#define CIRCUITPY_DEFAULT_STACK_SIZE            (0x10000)
#define CIRCUITPY_DISPLAY_AREA_BUFFER_SIZE (1920)
#define CIRCUITPY_PROCESSOR_COUNT (4)

#define MICROPY_FATFS_EXFAT    (1)
#define MICROPY_FATFS_MKFS_FAT32  (1)

////////////////////////////////////////////////////////////////////////////////////////////////////

// This also includes mpconfigboard.h.
#include "py/circuitpy_mpconfig.h"

// Definitions that can be overridden by mpconfigboard.h:

////////////////////////////////////////////////////////////////////////////////////////////////////
