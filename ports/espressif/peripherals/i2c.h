// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 microDev
//
// SPDX-License-Identifier: MIT

#pragma once

#include "driver/i2c.h"

extern void i2c_reset(void);
extern void never_reset_i2c(i2c_port_t num);
extern esp_err_t peripherals_i2c_init(i2c_port_t num, const i2c_config_t *i2c_conf);
extern void peripherals_i2c_deinit(i2c_port_t num);
extern i2c_port_t peripherals_i2c_get_free_num(void);
