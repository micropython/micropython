// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT
#pragma once

#include "py/stream.h"

void common_hal_msgpack_pack(mp_obj_t obj, mp_obj_t stream_obj, mp_obj_t default_handler);
mp_obj_t common_hal_msgpack_unpack(mp_obj_t stream_obj, mp_obj_t ext_hook, bool use_list);
