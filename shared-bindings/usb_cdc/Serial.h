// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/usb_cdc/Serial.h"

extern const mp_obj_type_t usb_cdc_serial_type;

extern size_t common_hal_usb_cdc_serial_read(usb_cdc_serial_obj_t *self, uint8_t *data, size_t len, int *errcode);
extern size_t common_hal_usb_cdc_serial_write(usb_cdc_serial_obj_t *self, const uint8_t *data, size_t len, int *errcode);

extern uint32_t common_hal_usb_cdc_serial_get_in_waiting(usb_cdc_serial_obj_t *self);
extern uint32_t common_hal_usb_cdc_serial_get_out_waiting(usb_cdc_serial_obj_t *self);

extern void common_hal_usb_cdc_serial_reset_input_buffer(usb_cdc_serial_obj_t *self);
extern uint32_t common_hal_usb_cdc_serial_reset_output_buffer(usb_cdc_serial_obj_t *self);

extern uint32_t common_hal_usb_cdc_serial_flush(usb_cdc_serial_obj_t *self);

extern bool common_hal_usb_cdc_serial_get_connected(usb_cdc_serial_obj_t *self);

extern mp_float_t common_hal_usb_cdc_serial_get_timeout(usb_cdc_serial_obj_t *self);
extern void common_hal_usb_cdc_serial_set_timeout(usb_cdc_serial_obj_t *self, mp_float_t timeout);

extern mp_float_t common_hal_usb_cdc_serial_get_write_timeout(usb_cdc_serial_obj_t *self);
extern void common_hal_usb_cdc_serial_set_write_timeout(usb_cdc_serial_obj_t *self, mp_float_t write_timeout);
