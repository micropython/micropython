// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

typedef enum {
    WARNINGS_DEFAULT,
    WARNINGS_ERROR,
    WARNINGS_IGNORE,
    WARNINGS_ALWAYS,
    WARNINGS_MODULE,
    WARNINGS_ONCE
} warnings_action_t;

void common_hal_warnings_warn(const char *message, const mp_obj_type_t *category);
void common_hal_warnings_simplefilter(warnings_action_t action);
