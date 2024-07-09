// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Matthias Breithaupt for Artisense GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

// Same setup as the Saola board but with no Neopixel on board

#define MICROPY_HW_BOARD_NAME       "Artisense Reference Design RD00"
#define MICROPY_HW_MCU_NAME         "ESP32S2"

#define MICROPY_HW_NEOPIXEL (&pin_GPIO45)

#define DEFAULT_UART_BUS_RX (&pin_GPIO17)
#define DEFAULT_UART_BUS_TX (&pin_GPIO18)
