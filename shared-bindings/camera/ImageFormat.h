// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright 2020 Sony Semiconductor Solutions Corporation
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

typedef enum {
    IMAGEFORMAT_NONE,
    IMAGEFORMAT_JPG,
    IMAGEFORMAT_RGB565,
} camera_imageformat_t;

extern const mp_obj_type_t camera_imageformat_type;

camera_imageformat_t camera_imageformat_obj_to_type(mp_obj_t obj);
mp_obj_t camera_imageformat_type_to_obj(camera_imageformat_t mode);

typedef struct {
    mp_obj_base_t base;
} camera_imageformat_obj_t;
extern const camera_imageformat_obj_t camera_imageformat_jpg_obj;
extern const camera_imageformat_obj_t camera_imageformat_rgb565_obj;
