// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/socketpool/__init__.h"

#include "common-hal/socketpool/Socket.h"

void socketpool_user_reset(void) {
    socket_user_reset();
}
