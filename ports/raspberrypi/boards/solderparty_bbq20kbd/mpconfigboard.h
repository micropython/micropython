// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME "BBQ20KBD"
#define MICROPY_HW_MCU_NAME "rp2040"

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO23)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO18)

#define DEFAULT_UART_BUS_TX (&pin_GPIO20)
