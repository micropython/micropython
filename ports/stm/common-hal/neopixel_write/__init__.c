// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/neopixel_write/__init__.h"

#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "common-hal/microcontroller/Pin.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_ll_gpio.h"
#include "supervisor/port.h"

uint64_t next_start_raw_ticks = 0;

// sysclock divisors
#define MAGIC_800_INT  900000  // ~1.11 us  -> 1.2  field
#define MAGIC_800_T0H  3500000  // 300ns actual; 880 low
#define MAGIC_800_T1H  1350000  // 768ns actual; 412 low

#pragma GCC push_options
#pragma GCC optimize ("Os")

void common_hal_neopixel_write(const digitalio_digitalinout_obj_t *digitalinout, uint8_t *pixels,
    uint32_t numBytes) {
    uint8_t *p = pixels, *end = p + numBytes, pix = *p++, mask = 0x80;
    uint32_t start = 0;
    uint32_t cyc = 0;

    // assumes 800_000Hz frequency
    // Theoretical values here are 800_000 -> 1.25us, 2500000->0.4us, 1250000->0.8us
    // TODO: try to get dynamic weighting working again
    uint32_t sys_freq = HAL_RCC_GetSysClockFreq();
    uint32_t interval = sys_freq / MAGIC_800_INT;
    uint32_t t0 = (sys_freq / MAGIC_800_T0H);
    uint32_t t1 = (sys_freq / MAGIC_800_T1H);

    // Wait to make sure we don't append onto the last transmission. This should only be a tick or
    // two.
    while (port_get_raw_ticks(NULL) < next_start_raw_ticks) {
    }

    GPIO_TypeDef *p_port = pin_port(digitalinout->pin->port);
    uint32_t p_mask = pin_mask(digitalinout->pin->number);

    __disable_irq();
    // Enable DWT in debug core. Usable when interrupts disabled, as opposed to Systick->VAL
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    DWT->CYCCNT = 0;

    for (;;) {
        cyc = (pix & mask) ? t1 : t0;
        start = DWT->CYCCNT;
        LL_GPIO_SetOutputPin(p_port, p_mask);
        while ((DWT->CYCCNT - start) < cyc) {
            ;
        }
        LL_GPIO_ResetOutputPin(p_port, p_mask);
        while ((DWT->CYCCNT - start) < interval) {
            ;
        }
        if (!(mask >>= 1)) {
            if (p >= end) {
                break;
            }
            pix = *p++;
            mask = 0x80;
        }
    }

    // Enable interrupts again
    __enable_irq();

    // Update the next start.
    next_start_raw_ticks = port_get_raw_ticks(NULL) + 4;
}

#pragma GCC pop_options
