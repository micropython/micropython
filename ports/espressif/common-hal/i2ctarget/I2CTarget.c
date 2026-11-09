// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 microDev
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/i2ctarget/I2CTarget.h"

#include "py/mperrno.h"
#include "py/runtime.h"

#include "common-hal/i2ctarget/I2CTarget.h"
#include "shared-bindings/microcontroller/Pin.h"

static bool i2c_slave_on_receive(i2c_slave_dev_handle_t i2c_slave, const i2c_slave_rx_done_event_data_t *evt_data, void *arg) {
    i2ctarget_i2c_target_obj_t *self = (i2ctarget_i2c_target_obj_t *)arg;
    for (uint32_t i = 0; i < evt_data->length; i++) {
        uint16_t next_head = (self->recv_head + 1) % I2CTARGET_RECV_BUF_SIZE;
        if (next_head == self->recv_tail) {
            break; // buffer full
        }
        self->recv_buf[self->recv_head] = evt_data->buffer[i];
        self->recv_head = next_head;
    }
    return false;
}

void common_hal_i2ctarget_i2c_target_construct(i2ctarget_i2c_target_obj_t *self,
    const mcu_pin_obj_t *scl, const mcu_pin_obj_t *sda,
    uint8_t *addresses, unsigned int num_addresses, bool smbus) {
    // Pins 45 and 46 are "strapping" pins that impact start up behavior. They usually need to
    // be pulled-down so pulling them up for I2C is a bad idea. To make this hard, we don't
    // support I2C on these pins.
    // Also 46 is input-only so it'll never work.
    if (scl->number == 45 || scl->number == 46 || sda->number == 45 || sda->number == 46) {
        raise_ValueError_invalid_pins();
    }

    if (num_addresses > 1) {
        mp_raise_ValueError(MP_ERROR_TEXT("Only one address is allowed"));
    }
    self->addresses = addresses;
    self->num_addresses = num_addresses;

    self->recv_head = 0;
    self->recv_tail = 0;

    i2c_slave_config_t slave_config = {
        .i2c_port = -1, // auto
        .sda_io_num = sda->number,
        .scl_io_num = scl->number,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .send_buf_depth = 256,
        .receive_buf_depth = 256,
        .slave_addr = addresses[0],
        .addr_bit_len = I2C_ADDR_BIT_LEN_7,
        .flags = {
            .enable_internal_pullup = true,
        },
    };

    esp_err_t err = i2c_new_slave_device(&slave_config, &self->handle);
    if (err != ESP_OK) {
        if (err == ESP_ERR_NOT_FOUND) {
            mp_raise_ValueError(MP_ERROR_TEXT("All I2C peripherals are in use"));
        } else if (err == ESP_FAIL) {
            mp_raise_OSError(MP_EIO);
        } else {
            mp_arg_error_invalid(MP_QSTR_I2CTarget);
        }
    }

    self->sda_pin = sda;
    self->scl_pin = scl;

    i2c_slave_event_callbacks_t cbs = {
        .on_receive = i2c_slave_on_receive,
    };
    i2c_slave_register_event_callbacks(self->handle, &cbs, self);

    claim_pin(sda);
    claim_pin(scl);
}

bool common_hal_i2ctarget_i2c_target_deinited(i2ctarget_i2c_target_obj_t *self) {
    return self->sda_pin == NULL;
}

void common_hal_i2ctarget_i2c_target_deinit(i2ctarget_i2c_target_obj_t *self) {
    if (common_hal_i2ctarget_i2c_target_deinited(self)) {
        return;
    }

    i2c_del_slave_device(self->handle);
    self->handle = NULL;

    common_hal_reset_pin(self->sda_pin);
    common_hal_reset_pin(self->scl_pin);
    self->sda_pin = NULL;
    self->scl_pin = NULL;
}

int common_hal_i2ctarget_i2c_target_is_addressed(i2ctarget_i2c_target_obj_t *self,
    uint8_t *address, bool *is_read, bool *is_restart) {
    *address = self->addresses[0];
    *is_read = true;
    *is_restart = false;
    // Check if we have received data
    if (self->recv_head != self->recv_tail) {
        *is_read = false;
        return 1;
    }
    return 0;
}

int common_hal_i2ctarget_i2c_target_read_byte(i2ctarget_i2c_target_obj_t *self, uint8_t *data) {
    if (self->recv_head == self->recv_tail) {
        return 0; // no data available
    }
    *data = self->recv_buf[self->recv_tail];
    self->recv_tail = (self->recv_tail + 1) % I2CTARGET_RECV_BUF_SIZE;
    return 1;
}

int common_hal_i2ctarget_i2c_target_write_byte(i2ctarget_i2c_target_obj_t *self, uint8_t data) {
    uint32_t write_len;
    esp_err_t err = i2c_slave_write(self->handle, &data, 1, &write_len, 100);
    if (err != ESP_OK || write_len == 0) {
        return 0;
    }
    return 1;
}

void common_hal_i2ctarget_i2c_target_ack(i2ctarget_i2c_target_obj_t *self, bool ack) {
}

void common_hal_i2ctarget_i2c_target_close(i2ctarget_i2c_target_obj_t *self) {
}
