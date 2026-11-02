// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "py/obj.h"
#include "shared-module/_airlift/transport.h"

void common_hal__airlift_init(void);
bool common_hal__airlift_active(void);
void common_hal__airlift_reset(void);
void common_hal__airlift_release(void);
// expected_replies < 0 takes the count from the reply. Returns a list of bytearrays.
mp_obj_t common_hal__airlift_transact(uint8_t cmd, const airlift_param_t *params, size_t n_params,
    bool param_len_16, int expected_replies, bool reply_len_16);
