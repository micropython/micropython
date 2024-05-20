// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright 2020 Sony Semiconductor Solutions Corporation
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/camera/Camera.h"
#include "shared-bindings/camera/ImageFormat.h"

extern const mp_obj_type_t camera_type;

void common_hal_camera_construct(camera_obj_t *self);
void common_hal_camera_deinit(camera_obj_t *self);
bool common_hal_camera_deinited(camera_obj_t *self);
size_t common_hal_camera_take_picture(camera_obj_t *self, uint8_t *buffer, size_t len, uint16_t width, uint16_t height, camera_imageformat_t format);
