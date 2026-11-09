// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/paralleldisplaybus/ParallelBus.h"

#include <stdint.h>

#include "common-hal/microcontroller/Pin.h"
#include "py/runtime.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/microcontroller/__init__.h"

void common_hal_paralleldisplaybus_parallelbus_construct(paralleldisplaybus_parallelbus_obj_t *self,
    const mcu_pin_obj_t *data0, const mcu_pin_obj_t *command, const mcu_pin_obj_t *chip_select,
    const mcu_pin_obj_t *write, const mcu_pin_obj_t *read, const mcu_pin_obj_t *reset, uint32_t frequency) {
    // Ensure the bus starts in its deinit state, so deinit() does nothing if
    // this raises. The bus lives in a display bus slot, which
    // release_displays() deinits even if construct never finished.
    self->bus = NULL;

    uint8_t data_pin = data0->number;
    if (data_pin % 8 != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("Data 0 pin must be byte aligned"));
    }
    for (uint8_t i = 0; i < 8; i++) {
        if (!pin_number_is_free(data_pin + i)) {
            mp_raise_ValueError_varg(MP_ERROR_TEXT("Bus pin %d is already in use"), i);
        }
    }
    NRF_GPIO_Type *g;
    uint8_t num_pins_in_port;
    if (data0->number < P0_PIN_NUM) {
        g = NRF_P0;
        num_pins_in_port = P0_PIN_NUM;
    } else {
        g = NRF_P1;
        num_pins_in_port = P1_PIN_NUM;
    }
    for (uint8_t i = 0; i < 8; i++) {
        claim_pin_number(data_pin + i);
    }
    g->DIRSET = 0xff << (data_pin % num_pins_in_port);
    for (uint8_t i = 0; i < 8; i++) {
        g->PIN_CNF[data_pin + i] |= NRF_GPIO_PIN_S0S1 << GPIO_PIN_CNF_DRIVE_Pos;
    }
    uint8_t *bus = ((uint8_t *)&g->OUT) + (data0->number % num_pins_in_port / 8);

    self->command.base.type = &digitalio_digitalinout_type;
    common_hal_digitalio_digitalinout_construct(&self->command, command);
    common_hal_digitalio_digitalinout_switch_to_output(&self->command, true, DRIVE_MODE_PUSH_PULL);

    self->chip_select.base.type = &digitalio_digitalinout_type;
    common_hal_digitalio_digitalinout_construct(&self->chip_select, chip_select);
    common_hal_digitalio_digitalinout_switch_to_output(&self->chip_select, true, DRIVE_MODE_PUSH_PULL);

    self->write.base.type = &digitalio_digitalinout_type;
    common_hal_digitalio_digitalinout_construct(&self->write, write);
    common_hal_digitalio_digitalinout_switch_to_output(&self->write, true, DRIVE_MODE_PUSH_PULL);

    // read is optional.
    self->read.base.type = &mp_type_NoneType;
    if (read != NULL) {
        self->read.base.type = &digitalio_digitalinout_type;
        common_hal_digitalio_digitalinout_construct(&self->read, read);
        common_hal_digitalio_digitalinout_switch_to_output(&self->read, true, DRIVE_MODE_PUSH_PULL);
    }

    self->data0_pin = data_pin;
    uint8_t num_pins_in_write_port;
    // The write pin's own port, which may not be data0's.
    if (write->number < P0_PIN_NUM) {
        self->write_group = NRF_P0;
        num_pins_in_write_port = P0_PIN_NUM;
    } else {
        self->write_group = NRF_P1;
        num_pins_in_write_port = P1_PIN_NUM;
    }
    self->write_mask = 1 << (write->number % num_pins_in_write_port);

    self->reset.base.type = &mp_type_NoneType;
    if (reset != NULL) {
        self->reset.base.type = &digitalio_digitalinout_type;
        common_hal_digitalio_digitalinout_construct(&self->reset, reset);
        common_hal_digitalio_digitalinout_switch_to_output(&self->reset, true, DRIVE_MODE_PUSH_PULL);
        common_hal_paralleldisplaybus_parallelbus_reset(self);
    }

    // The bus is live from here.
    self->bus = bus;
}

void common_hal_paralleldisplaybus_parallelbus_deinit(paralleldisplaybus_parallelbus_obj_t *self) {
    if (self->bus == NULL) {
        return;
    }
    for (uint8_t i = 0; i < 8; i++) {
        reset_pin_number(self->data0_pin + i);
    }

    // read and reset are optional. One that wasn't given was never constructed,
    // so its pin is NULL, and digitalinout_deinit() returns without touching a
    // DigitalInOut whose pin is NULL.
    common_hal_digitalio_digitalinout_deinit(&self->command);
    common_hal_digitalio_digitalinout_deinit(&self->chip_select);
    common_hal_digitalio_digitalinout_deinit(&self->write);
    common_hal_digitalio_digitalinout_deinit(&self->read);
    common_hal_digitalio_digitalinout_deinit(&self->reset);
    self->bus = NULL;
}

bool common_hal_paralleldisplaybus_parallelbus_reset(mp_obj_t obj) {
    paralleldisplaybus_parallelbus_obj_t *self = MP_OBJ_TO_PTR(obj);
    if (self->reset.base.type == &mp_type_NoneType) {
        return false;
    }

    common_hal_digitalio_digitalinout_set_value(&self->reset, false);
    common_hal_mcu_delay_us(4);
    common_hal_digitalio_digitalinout_set_value(&self->reset, true);
    return true;
}

bool common_hal_paralleldisplaybus_parallelbus_bus_free(mp_obj_t obj) {
    return true;
}

bool common_hal_paralleldisplaybus_parallelbus_begin_transaction(mp_obj_t obj) {
    paralleldisplaybus_parallelbus_obj_t *self = MP_OBJ_TO_PTR(obj);
    common_hal_digitalio_digitalinout_set_value(&self->chip_select, false);
    return true;
}

// This ignores chip_select behaviour because data is clocked in by the write line toggling.
void common_hal_paralleldisplaybus_parallelbus_send(mp_obj_t obj, display_byte_type_t byte_type,
    display_chip_select_behavior_t chip_select, const uint8_t *data, uint32_t data_length) {
    paralleldisplaybus_parallelbus_obj_t *self = MP_OBJ_TO_PTR(obj);
    common_hal_digitalio_digitalinout_set_value(&self->command, byte_type == DISPLAY_DATA);
    uint32_t *clear_write = (uint32_t *)&self->write_group->OUTCLR;
    uint32_t *set_write = (uint32_t *)&self->write_group->OUTSET;
    uint32_t mask = self->write_mask;
    for (uint32_t i = 0; i < data_length; i++) {
        *clear_write = mask;
        *self->bus = data[i];
        *set_write = mask;
    }
}

void common_hal_paralleldisplaybus_parallelbus_end_transaction(mp_obj_t obj) {
    paralleldisplaybus_parallelbus_obj_t *self = MP_OBJ_TO_PTR(obj);
    common_hal_digitalio_digitalinout_set_value(&self->chip_select, true);
}
