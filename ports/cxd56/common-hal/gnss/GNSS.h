// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright 2020 Sony Semiconductor Solutions Corporation
//
// SPDX-License-Identifier: MIT

#pragma once

#include <arch/chip/gnss.h>

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    unsigned long satellite_system;
    uint8_t fix;
    double latitude;
    double longitude;
    double altitude;
    struct cxd56_gnss_date_s date;
    struct cxd56_gnss_time_s time;
} gnss_obj_t;
