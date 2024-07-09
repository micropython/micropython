// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Nick Moore for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2023 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "common-hal/digitalio/DigitalInOut.h"
#include "common-hal/rotaryio/IncrementalEncoder.h"
#include "shared-module/rotaryio/IncrementalEncoder.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/rotaryio/IncrementalEncoder.h"

#include "py/runtime.h"

#include "sdk/drivers/igpio/fsl_gpio.h"
static void encoder_change(void *self_in) {
    rotaryio_incrementalencoder_obj_t *self = self_in;

    bool value_a = GPIO_PinRead(self->pin_a->gpio, self->pin_a->number);
    bool value_b = GPIO_PinRead(self->pin_b->gpio, self->pin_b->number);
    uint8_t new_state = (value_a << 1) | value_b;
    shared_module_softencoder_state_update(self, new_state);
}

void common_hal_rotaryio_incrementalencoder_construct(rotaryio_incrementalencoder_obj_t *self,
    const mcu_pin_obj_t *pin_a, const mcu_pin_obj_t *pin_b) {

    self->pin_a = pin_a;
    self->pin_b = pin_b;

    // GPIO is always IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_ALT5 until proven otherwise
#define IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_ALT5 5U
    IOMUXC_SetPinMux(pin_a->mux_reg, IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_ALT5, 0, 0, 0, 0);
    IOMUXC_SetPinMux(pin_b->mux_reg, IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_ALT5, 0, 0, 0, 0);

    const gpio_pin_config_t config = { kGPIO_DigitalInput, 0, kGPIO_IntRisingOrFallingEdge };
    GPIO_PinInit(pin_a->gpio, pin_a->number, &config);
    GPIO_PinInit(pin_b->gpio, pin_b->number, &config);

    enable_pin_change_interrupt(pin_a, encoder_change, self);
    enable_pin_change_interrupt(pin_b, encoder_change, self);

    pin_config(pin_a, false, PULL_UP);
    pin_config(pin_b, false, PULL_UP);

    claim_pin(pin_a);
    claim_pin(pin_b);
}

bool common_hal_rotaryio_incrementalencoder_deinited(rotaryio_incrementalencoder_obj_t *self) {
    return !self->pin_a;
}

void common_hal_rotaryio_incrementalencoder_deinit(rotaryio_incrementalencoder_obj_t *self) {
    if (common_hal_rotaryio_incrementalencoder_deinited(self)) {
        return;
    }
    disable_pin_change_interrupt(self->pin_a);
    disable_pin_change_interrupt(self->pin_b);

    common_hal_reset_pin(self->pin_a);
    common_hal_reset_pin(self->pin_b);

    self->pin_a = NULL;
    self->pin_b = NULL;
}
