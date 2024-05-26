// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "mdns.h"

typedef struct {
    mp_obj_base_t base;
    mdns_result_t *result;
} mdns_remoteservice_obj_t;
