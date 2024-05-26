// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 MicroDev
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "esp_now.h"

typedef struct {
    mp_obj_base_t base;
    esp_now_peer_info_t peer_info;
    bool lmk_set;
} espnow_peer_obj_t;

extern const mp_obj_type_t espnow_peer_type;
