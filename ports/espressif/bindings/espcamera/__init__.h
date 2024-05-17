// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/enum.h"
#include "py/obj.h"

#include "esp_camera.h"

extern const mp_obj_type_t espcamera_grab_mode_type;
extern const cp_enum_obj_t grab_mode_WHEN_EMPTY_obj;
extern const mp_obj_type_t espcamera_pixel_format_type;
extern const cp_enum_obj_t pixel_format_RGB565_obj;
extern const mp_obj_type_t espcamera_frame_size_type;
extern const cp_enum_obj_t frame_size_QQVGA_obj;
extern const mp_obj_type_t espcamera_gain_ceiling_type;

extern camera_grab_mode_t validate_grab_mode(mp_obj_t obj, qstr arg_name);
extern pixformat_t validate_pixel_format(mp_obj_t obj, qstr arg_name);
extern framesize_t validate_frame_size(mp_obj_t obj, qstr arg_name);
extern gainceiling_t validate_gain_ceiling(mp_obj_t obj, qstr arg_name);
