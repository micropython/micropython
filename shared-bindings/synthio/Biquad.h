// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "py/objnamedtuple.h"

extern const mp_obj_namedtuple_type_t synthio_biquad_type_obj;
mp_obj_t common_hal_synthio_new_lpf(mp_float_t w0, mp_float_t Q);
mp_obj_t common_hal_synthio_new_hpf(mp_float_t w0, mp_float_t Q);
mp_obj_t common_hal_synthio_new_bpf(mp_float_t w0, mp_float_t Q);
