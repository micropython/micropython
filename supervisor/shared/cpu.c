// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "supervisor/shared/cpu.h"

bool cpu_interrupt_active(void) {
    #if defined(__ARM_ARCH) && (__ARM_ARCH >= 6) && defined(__ARM_ARCH_PROFILE) && (__ARM_ARCH_PROFILE == 'M')
    // Check VECTACTIVE in ICSR. We don't need to disable interrupts because if
    // one occurs after we read, we won't continue until it is resolved.
    return (*((volatile uint32_t *)0xE000ED04) & 0x1ff) != 0;
    #else
    // We don't know.
    return false;
    #endif
}
