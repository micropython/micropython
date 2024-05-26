// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 MicroDev
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    mp_obj_t list;
} espnow_peers_obj_t;

extern const mp_obj_type_t espnow_peers_type;
extern espnow_peers_obj_t *espnow_peers_new(void);
