// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared/runtime/context_manager_helpers.h"

#include "py/obj.h"

static mp_obj_t default___enter__(mp_obj_t self_in) {
    return self_in;
}
MP_DEFINE_CONST_FUN_OBJ_1(default___enter___obj, default___enter__);
