/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 MicroPython contributors
 */

#ifndef MICROPY_INCLUDED_PIC32_MPHAL_H
#define MICROPY_INCLUDED_PIC32_MPHAL_H

#include "py/mpconfig.h"

// HAL functions
void mp_hal_stdout_tx_strn(const char *str, size_t len);
void mp_hal_stdout_tx_strn_cooked(const char *str, size_t len);

static inline void mp_hal_set_interrupt_char(int c) {
    // TODO: Implement interrupt character handling
}

static inline void mp_hal_delay_ms(mp_uint_t ms) {
    // TODO: Implement ms delay
    for (volatile uint32_t i = 0; i < ms * 10000; i++) {
        __asm__ volatile ("nop");
    }
}

static inline void mp_hal_delay_us(mp_uint_t us) {
    // TODO: Implement us delay
    for (volatile uint32_t i = 0; i < us * 10; i++) {
        __asm__ volatile ("nop");
    }
}

// IRQ disable/enable
#define MACHINE_DISABLE_IRQ() __extension__({ \
    uint32_t __status; \
    __asm__ volatile ("di %0" : "=r"(__status)); \
    __status; \
})

#define MACHINE_ENABLE_IRQ(state) __asm__ volatile ("mtc0 %0, $12" : : "r"(state))

#endif // MICROPY_INCLUDED_PIC32_MPHAL_H
