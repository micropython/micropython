// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 James Bowman for Excamera Labs
//
// SPDX-License-Identifier: MIT

#pragma once

typedef struct _common_hal__eve_t {
    mp_obj_t dest[3];           // Own 'write' method, plus argument
    int vscale;                 // fixed-point scaling used for Vertex2f
    size_t n;                   // Current size of command buffer
    uint8_t buf[512];           // Command buffer
} common_hal__eve_t;
