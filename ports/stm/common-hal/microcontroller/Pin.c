// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/microcontroller/Pin.h"

#include "pins.h"

#if defined(TFBGA216)
GPIO_TypeDef *ports[] = {GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, GPIOH, GPIOI, GPIOJ, GPIOK};
#elif defined(LQFP144) || defined(WLCSP144)
GPIO_TypeDef *ports[] = {GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG};
#elif defined(LQFP100_f4) || (LQFP100_x7)
GPIO_TypeDef *ports[] = {GPIOA, GPIOB, GPIOC, GPIOD, GPIOE};
#elif defined(LQFP64)
GPIO_TypeDef *ports[] = {GPIOA, GPIOB, GPIOC, GPIOD};
#elif defined(UFQFPN48)
GPIO_TypeDef *ports[] = {GPIOA, GPIOB, GPIOC};
#else
    #error Unknown package type
#endif


#define GPIO_PORT_COUNT (MP_ARRAY_SIZE(ports))

static uint16_t claimed_pins[GPIO_PORT_COUNT];
static uint16_t __ALIGNED(4) never_reset_pins[GPIO_PORT_COUNT];

void reset_all_pins(void) {
    // Reset claimed pins
    for (uint8_t i = 0; i < GPIO_PORT_COUNT; i++) {
        claimed_pins[i] = never_reset_pins[i];
    }
    for (uint8_t i = 0; i < GPIO_PORT_COUNT; i++) {
        HAL_GPIO_DeInit(ports[i], ~never_reset_pins[i]);
    }
}

// Mark pin as free and return it to a quiescent state.
void reset_pin_number(uint8_t pin_port, uint8_t pin_number) {
    if (pin_number == NO_PIN) {
        return;
    }

    if (pin_port == 0x0F) {
        return;
    }
    // Clear claimed bit & reset
    claimed_pins[pin_port] &= ~(1 << pin_number);
    never_reset_pins[pin_port] &= ~(1 << pin_number);
    HAL_GPIO_DeInit(ports[pin_port], 1 << pin_number);
}

void never_reset_pin_number(uint8_t pin_port, uint8_t pin_number) {
    if (pin_number == NO_PIN) {
        return;
    }
    never_reset_pins[pin_port] |= 1 << pin_number;
    // Make sure never reset pins are also always claimed
    claimed_pins[pin_port] |= 1 << pin_number;
}

void common_hal_never_reset_pin(const mcu_pin_obj_t *pin) {
    never_reset_pin_number(pin->port, pin->number);
}

void common_hal_reset_pin(const mcu_pin_obj_t *pin) {
    if (pin == NULL) {
        return;
    }
    reset_pin_number(pin->port, pin->number);
}

void claim_pin(uint8_t pin_port, uint8_t pin_number) {
    // Set bit in claimed_pins bitmask.
    claimed_pins[pin_port] |= 1 << pin_number;
}

bool pin_number_is_free(uint8_t pin_port, uint8_t pin_number) {
    return !(claimed_pins[pin_port] & 1 << pin_number);
}

bool common_hal_mcu_pin_is_free(const mcu_pin_obj_t *pin) {
    return pin_number_is_free(pin->port, pin->number);
}

GPIO_TypeDef *pin_port(uint8_t pin_port) {
    return ports[pin_port];
}

uint16_t pin_mask(uint8_t pin_number) {
    return 1 << pin_number;
}

uint8_t common_hal_mcu_pin_number(const mcu_pin_obj_t *pin) {
    return pin->port * 16 + pin->number;
}

void common_hal_mcu_pin_claim(const mcu_pin_obj_t *pin) {
    claim_pin(pin->port, pin->number);
}

void common_hal_mcu_pin_reset_number(uint8_t pin_no) {
    reset_pin_number(pin_no / 16, pin_no % 16);
}
