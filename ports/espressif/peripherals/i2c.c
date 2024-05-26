// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 microDev
//
// SPDX-License-Identifier: MIT

#include "peripherals/i2c.h"

typedef enum {
    STATUS_FREE = 0,
    STATUS_IN_USE,
    STATUS_NEVER_RESET
} i2c_status_t;

static i2c_status_t i2c_status[I2C_NUM_MAX];

void i2c_reset(void) {
    for (i2c_port_t num = 0; num < (i2c_port_t)I2C_NUM_MAX; num++) {
        if (i2c_status[num] == STATUS_IN_USE) {
            i2c_driver_delete(num);
            i2c_status[num] = STATUS_FREE;
        }
    }
}

void never_reset_i2c(i2c_port_t num) {
    i2c_status[num] = STATUS_NEVER_RESET;
}

esp_err_t peripherals_i2c_init(i2c_port_t num, const i2c_config_t *i2c_conf) {
    esp_err_t err = i2c_param_config(num, i2c_conf);
    if (err != ESP_OK) {
        return err;
    }
    size_t rx_buf_len = 0;
    size_t tx_buf_len = 0;
    #ifdef SOC_I2C_SUPPORT_SLAVE
    if (i2c_conf->mode == I2C_MODE_SLAVE) {
        rx_buf_len = 256;
        tx_buf_len = 256;
    }
    #endif
    return i2c_driver_install(num, i2c_conf->mode, rx_buf_len, tx_buf_len, 0);
}

void peripherals_i2c_deinit(i2c_port_t num) {
    i2c_reset_rx_fifo(num);
    i2c_reset_tx_fifo(num);
    i2c_driver_delete(num);
    i2c_status[num] = STATUS_FREE;
}

i2c_port_t peripherals_i2c_get_free_num(void) {
    i2c_port_t i2c_num = I2C_NUM_MAX;
    for (i2c_port_t num = 0; num < (int)I2C_NUM_MAX; num++) {
        if (i2c_status[num] == STATUS_FREE) {
            i2c_num = num;
            break;
        }
    }
    if (i2c_num != I2C_NUM_MAX) {
        i2c_status[i2c_num] = STATUS_IN_USE;
    }
    return i2c_num;
}
