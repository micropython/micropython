// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"

void board_init(void) {
    // struct port_config pin_conf;
    // port_get_config_defaults(&pin_conf);
    //
    // pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
    // port_pin_set_config(MICROPY_HW_LED_TX, &pin_conf);
    // port_pin_set_output_level(MICROPY_HW_LED_TX, true);
    //
    // port_pin_set_config(MICROPY_HW_LED_RX, &pin_conf);
    // port_pin_set_output_level(MICROPY_HW_LED_RX, true);
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
