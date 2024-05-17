// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#include "supervisor/port.h"

#include "supervisor/linker.h"

#include "fsl_common.h"

void PLACE_IN_ITCM(port_background_task)(void) {
}

void port_background_tick(void) {
}

void port_start_background_tick(void) {
}

void port_finish_background_tick(void) {
}
