// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#include <stdio.h>

#include <stdint.h>
#include "py/mpconfig.h"
#include "py/obj.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "mphalport.h"

#include "peripherals/broadcom/defines.h"

void mp_hal_delay_us(mp_uint_t delay) {
    uint32_t end = SYSTMR->CLO + delay;
    // Wait if end is before current time because it must have wrapped.
    while (end < SYSTMR->CLO) {
    }
    while (SYSTMR->CLO < end) {
    }
}

void mp_hal_disable_all_interrupts(void) {
    common_hal_mcu_disable_interrupts();
}

void mp_hal_enable_all_interrupts(void) {
    common_hal_mcu_enable_interrupts();
}

mp_uint_t cpu_get_regs_and_sp(mp_uint_t *regs) {
    size_t sp = 0;
    #if defined(__ARM_ARCH) && (__ARM_ARCH >= 8)
    __asm__ ("mov %[out], sp" : [out] "=r" (sp));
    __asm__ ("mov %[out], x19" : [out] "=r" (regs[0]));
    __asm__ ("mov %[out], x20" : [out] "=r" (regs[1]));
    __asm__ ("mov %[out], x21" : [out] "=r" (regs[2]));
    __asm__ ("mov %[out], x22" : [out] "=r" (regs[3]));
    __asm__ ("mov %[out], x23" : [out] "=r" (regs[4]));
    __asm__ ("mov %[out], x24" : [out] "=r" (regs[5]));
    __asm__ ("mov %[out], x25" : [out] "=r" (regs[6]));
    __asm__ ("mov %[out], x26" : [out] "=r" (regs[7]));
    __asm__ ("mov %[out], x27" : [out] "=r" (regs[8]));
    __asm__ ("mov %[out], x28" : [out] "=r" (regs[9]));
    #else
    __asm__ ("mov %[out], sp" : [out] "=r" (sp));
    __asm__ ("mov %[out], r4" : [out] "=r" (regs[0]));
    __asm__ ("mov %[out], r5" : [out] "=r" (regs[1]));
    __asm__ ("mov %[out], r6" : [out] "=r" (regs[2]));
    __asm__ ("mov %[out], r7" : [out] "=r" (regs[3]));
    __asm__ ("mov %[out], r8" : [out] "=r" (regs[4]));
    __asm__ ("mov %[out], r9" : [out] "=r" (regs[5]));
    __asm__ ("mov %[out], r10" : [out] "=r" (regs[6]));
    __asm__ ("mov %[out], r11" : [out] "=r" (regs[7]));
    __asm__ ("mov %[out], r12" : [out] "=r" (regs[8]));
    __asm__ ("mov %[out], r13" : [out] "=r" (regs[9]));
    #endif

    return sp;
}
