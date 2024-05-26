// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "lib/quirc/lib/quirc.h"
#include "shared-bindings/qrio/PixelPolicy.h"

typedef struct qrio_qrdecoder_obj {
    mp_obj_base_t base;
    struct quirc *quirc;
    struct quirc_code code;
    struct quirc_data data;
} qrdecoder_qrdecoder_obj_t;

void shared_module_qrio_qrdecoder_construct(qrdecoder_qrdecoder_obj_t *, int width, int height);
int shared_module_qrio_qrdecoder_get_height(qrdecoder_qrdecoder_obj_t *);
int shared_module_qrio_qrdecoder_get_width(qrdecoder_qrdecoder_obj_t *);
void shared_module_qrio_qrdecoder_set_height(qrdecoder_qrdecoder_obj_t *, int height);
void shared_module_qrio_qrdecoder_set_width(qrdecoder_qrdecoder_obj_t *, int width);
mp_obj_t shared_module_qrio_qrdecoder_decode(qrdecoder_qrdecoder_obj_t *, const mp_buffer_info_t *bufinfo, qrio_pixel_policy_t policy);
mp_obj_t shared_module_qrio_qrdecoder_find(qrdecoder_qrdecoder_obj_t *, const mp_buffer_info_t *bufinfo, qrio_pixel_policy_t policy);
