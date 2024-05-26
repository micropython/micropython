// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdbool.h>
#include "py/mpconfig.h"
#include "py/misc.h"

#include "peripherals/exti.h"

#if !(CPY_STM32H7)

static bool stm_exti_reserved[STM32_GPIO_PORT_SIZE];
static bool stm_exti_never_reset[STM32_GPIO_PORT_SIZE];
static void (*stm_exti_callback[STM32_GPIO_PORT_SIZE])(uint8_t num);

void exti_reset(void) {
    for (size_t i = 0; i < STM32_GPIO_PORT_SIZE; i++) {
        if (!stm_exti_never_reset[i]) {
            stm_exti_reserved[i] = false;
            stm_exti_callback[i] = NULL;
            stm_peripherals_exti_disable(i);
        }
    }
}

void stm_peripherals_exti_never_reset(uint8_t num) {
    stm_exti_never_reset[num] = true;
}

void stm_peripherals_exti_reset_exti(uint8_t num) {
    stm_peripherals_exti_disable(num);
    stm_exti_never_reset[num] = false;
    stm_exti_reserved[num] = false;
    stm_exti_callback[num] = NULL;
}

bool stm_peripherals_exti_is_free(uint8_t num) {
    return !stm_exti_reserved[num];
}

bool stm_peripherals_exti_reserve(uint8_t num) {
    if (!stm_peripherals_exti_is_free(num)) {
        return false;
    }
    stm_exti_reserved[num] = true;
    return true;
}

void stm_peripherals_exti_enable(uint8_t num) {
    HAL_NVIC_EnableIRQ(stm_peripherals_exti_get_irq(num));
}

void stm_peripherals_exti_disable(uint8_t num) {
    HAL_NVIC_DisableIRQ(stm_peripherals_exti_get_irq(num));
}

void stm_peripherals_exti_set_callback(void (*callback)(uint8_t num), uint8_t number) {
    stm_exti_callback[number] = callback;
}

void stm_peripherals_exti_free(uint8_t num) {
    stm_exti_reserved[num] = true;
}

IRQn_Type stm_peripherals_exti_get_irq(uint8_t num) {
    if (num == 0) {
        return EXTI0_IRQn;
    } else if (num == 1) {
        return EXTI1_IRQn;
    } else if (num == 2) {
        return EXTI2_IRQn;
    } else if (num == 3) {
        return EXTI3_IRQn;
    } else if (num == 4) {
        return EXTI4_IRQn;
    } else if (num >= 5 && num <= 9) {
        return EXTI9_5_IRQn;
    } else if (num >= 10 && num <= 15) {
        return EXTI15_10_IRQn;
    } else {
        return 0;
    }
}

void EXTI0_IRQHandler(void) {
    stm_exti_callback[0](0);
}
void EXTI1_IRQHandler(void) {
    stm_exti_callback[1](1);
}
void EXTI2_IRQHandler(void) {
    stm_exti_callback[2](2);
}
void EXTI3_IRQHandler(void) {
    stm_exti_callback[3](3);
}
void EXTI4_IRQHandler(void) {
    stm_exti_callback[4](4);
}

#ifdef STM32L4
#define PR PR1
#endif
void EXTI9_5_IRQHandler(void) {
    uint32_t pending = EXTI->PR;
    for (uint i = 5; i <= 9; i++) {
        if (pending & (1 << i)) {
            stm_exti_callback[i](i);
        }
    }
}

void EXTI15_10_IRQHandler(void) {
    uint32_t pending = EXTI->PR;
    for (uint i = 10; i <= 15; i++) {
        if (pending & (1 << i)) {
            stm_exti_callback[i](i);
        }
    }
}
#ifdef STM32L4
#undef PR
#endif


#endif
