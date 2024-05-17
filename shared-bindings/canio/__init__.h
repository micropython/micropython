// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

typedef enum {
    BUS_STATE_ERROR_ACTIVE, BUS_STATE_ERROR_PASSIVE, BUS_STATE_ERROR_WARNING, BUS_STATE_OFF
} canio_bus_state_t;

extern const mp_obj_type_t canio_bus_state_type;
