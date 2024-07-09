// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"

#pragma once

typedef struct gifio_gifwriter gifio_gifwriter_t;
typedef enum displayio_colorspace displayio_colorspace_t;

extern const mp_obj_type_t gifio_gifwriter_type;

void shared_module_gifio_gifwriter_construct(gifio_gifwriter_t *self, mp_obj_t *file, int width, int height, displayio_colorspace_t colorspace, bool loop, bool dither, bool own_file);
void shared_module_gifio_gifwriter_check_for_deinit(gifio_gifwriter_t *self);
bool shared_module_gifio_gifwriter_deinited(gifio_gifwriter_t *self);
void shared_module_gifio_gifwriter_deinit(gifio_gifwriter_t *self);
void shared_module_gifio_gifwriter_add_frame(gifio_gifwriter_t *self, const mp_buffer_info_t *buf, int16_t delay);
void shared_module_gifio_gifwriter_close(gifio_gifwriter_t *self);
