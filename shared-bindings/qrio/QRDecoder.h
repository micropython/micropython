// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

typedef struct qrio_qrdecoder_obj qrio_qrdecoder_obj_t;

extern const mp_obj_type_t qrio_qrdecoder_type_obj;

void common_hal_qrio_qrdecoder_construct(qrio_qrdecoder_obj_t *self);

mp_obj_t common_hal_qrio_qrdecoder_decode(qrio_qrdecoder_obj_t *self, int width, int height, mp_buffer_info_t *buf);
