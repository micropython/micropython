// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"
#include "supervisor/filesystem.h"
#include "supervisor/port.h"
#include "supervisor/shared/stack.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void port_background_tick(void) {
    // Zero delay in case FreeRTOS wants to switch to something else.
    vTaskDelay(0);
}

void port_background_task(void) {
}

void port_start_background_tick(void) {
}

void port_finish_background_tick(void) {
}
