// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Radomir Dopieralski
//
// SPDX-License-Identifier: MIT

#include <stdbool.h>

#include "py/mpstate.h"
#include "__init__.h"
#include "PewPew.h"

#include "shared-bindings/digitalio/DigitalInOut.h"


void pew_tick(void) {
    static uint8_t col = 0;
    static uint8_t turn = 0;
    static uint8_t pressed = 0;
    static uint8_t last_pressed = 0;
    digitalio_digitalinout_obj_t *pin;

    pew_obj_t *pew = MP_STATE_VM(pew_singleton);
    if (!pew) {
        return;
    }

    pin = MP_OBJ_TO_PTR(pew->cols[col]);
    ++col;
    if (col >= pew->cols_size) {
        pew->pressed |= last_pressed & pressed;
        last_pressed = pressed;
        pressed = 0;
        col = 0;
        ++turn;
        if (turn > 11) {
            turn = 0;
        }
    }
    if (!common_hal_digitalio_digitalinout_get_value(pew->buttons)) {
        pressed |= 1 << col;
    }
    common_hal_digitalio_digitalinout_set_value(pin, true);
    for (size_t x = 0; x < pew->rows_size; ++x) {
        pin = MP_OBJ_TO_PTR(pew->rows[x]);
        uint8_t color = pew->buffer[col * (pew->rows_size) + x];
        bool value = false;
        switch (color & 0x03) {
            case 3:
                value = true;
                break;
            case 2:
                if (turn == 2 || turn == 5 || turn == 8 || turn == 11) {
                    value = true;
                }
                break;
            case 1:
                if (turn == 0) {
                    value = true;
                }
                break;
            case 0:
                break;
        }
        common_hal_digitalio_digitalinout_set_value(pin, value);
    }
    pin = MP_OBJ_TO_PTR(pew->cols[col]);
    common_hal_digitalio_digitalinout_set_value(pin, false);
}
