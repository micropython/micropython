// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Mark Komus, Ken Stillson, im-redactd
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/i2ctarget/I2CTarget.h"

#include "common-hal/busio/I2C.h"

#include "py/mperrno.h"
#include "py/mphal.h"
#include "shared-bindings/busio/I2C.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "py/runtime.h"

#include "src/rp2_common/hardware_gpio/include/hardware/gpio.h"

static i2c_inst_t *i2c[2] = {i2c0, i2c1};

#define NO_PIN 0xff

void common_hal_i2ctarget_i2c_target_construct(i2ctarget_i2c_target_obj_t *self, const mcu_pin_obj_t *scl, const mcu_pin_obj_t *sda,
    uint8_t *addresses, unsigned int num_addresses, bool smbus) {
    self->peripheral = NULL;

    // I2C pins have a regular pattern. SCL is always odd and SDA is even. They match up in pairs
    // so we can divide by two to get the instance. This pattern repeats.
    size_t scl_instance = (scl->number / 2) % 2;
    size_t sda_instance = (sda->number / 2) % 2;
    if (scl->number % 2 == 1 && sda->number % 2 == 0 && scl_instance == sda_instance) {
        self->peripheral = i2c[sda_instance];
    }

    if (self->peripheral == NULL) {
        raise_ValueError_invalid_pins();
    }

    if ((i2c_get_hw(self->peripheral)->enable & I2C_IC_ENABLE_ENABLE_BITS) != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("I2C peripheral in use"));
    }

    if (num_addresses > 1) {
        mp_raise_ValueError(MP_ERROR_TEXT("Only one address is allowed"));
    }

    self->addresses = addresses;
    self->num_addresses = num_addresses;
    self->scl_pin = scl->number;
    self->sda_pin = sda->number;

    // Have to specify a baudrate even if the i2c target does not use it
    const uint32_t frequency = 400000;
    i2c_init(self->peripheral, frequency);

    gpio_set_function(sda->number, GPIO_FUNC_I2C);
    gpio_set_function(scl->number, GPIO_FUNC_I2C);

    gpio_set_pulls(sda->number, true, false);
    gpio_set_pulls(scl->number, true, false);

    self->peripheral->hw->intr_mask |= I2C_IC_INTR_MASK_M_RESTART_DET_BITS;
    i2c_set_slave_mode(self->peripheral, true, self->addresses[0]);

    return;
}

bool common_hal_i2ctarget_i2c_target_deinited(i2ctarget_i2c_target_obj_t *self) {
    return self->sda_pin == NO_PIN;
}

void common_hal_i2ctarget_i2c_target_deinit(i2ctarget_i2c_target_obj_t *self) {
    if (common_hal_i2ctarget_i2c_target_deinited(self)) {
        return;
    }

    i2c_deinit(self->peripheral);

    reset_pin_number(self->sda_pin);
    reset_pin_number(self->scl_pin);
    self->sda_pin = NO_PIN;
    self->scl_pin = NO_PIN;

    return;
}

int common_hal_i2ctarget_i2c_target_is_addressed(i2ctarget_i2c_target_obj_t *self, uint8_t *address, bool *is_read, bool *is_restart) {
    if (!((self->peripheral->hw->raw_intr_stat & I2C_IC_INTR_STAT_R_RX_FULL_BITS) || (self->peripheral->hw->raw_intr_stat & I2C_IC_INTR_STAT_R_RD_REQ_BITS))) {
        return 0;
    }

    *address = self->peripheral->hw->sar;
    *is_read = !(self->peripheral->hw->raw_intr_stat & I2C_IC_INTR_STAT_R_RX_FULL_BITS);
    *is_restart = ((self->peripheral->hw->raw_intr_stat & I2C_IC_INTR_STAT_R_RESTART_DET_BITS) != 0);

    common_hal_i2ctarget_i2c_target_ack(self, true);
    return 1;
}

int common_hal_i2ctarget_i2c_target_read_byte(i2ctarget_i2c_target_obj_t *self, uint8_t *data) {
    if (self->peripheral->hw->status & I2C_IC_STATUS_RFNE_BITS) {
        *data = (uint8_t)self->peripheral->hw->data_cmd;
        return 1;
    } else {
        return 0;
    }
}

int common_hal_i2ctarget_i2c_target_write_byte(i2ctarget_i2c_target_obj_t *self, uint8_t data) {
    if (self->peripheral->hw->raw_intr_stat & I2C_IC_INTR_STAT_R_TX_ABRT_BITS) {
        self->peripheral->hw->clr_tx_abrt;
    }

    const size_t IC_TX_BUFFER_DEPTH = 16;
    size_t space = IC_TX_BUFFER_DEPTH - self->peripheral->hw->txflr;

    if (space > 0) {
        self->peripheral->hw->data_cmd = data;
        self->peripheral->hw->clr_rd_req;
        return 1;
    } else {
        return 0;
    }
}

void common_hal_i2ctarget_i2c_target_ack(i2ctarget_i2c_target_obj_t *self, bool ack) {
    return;
}

void common_hal_i2ctarget_i2c_target_close(i2ctarget_i2c_target_obj_t *self) {
    return;
}
