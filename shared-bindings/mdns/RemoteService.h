/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Dan Halbert for Adafruit Industries
 * Copyright (c) 2018 Artur Pacholec
 * Copyright (c) 2017 Glenn Ruben Bakke
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include "py/obj.h"
#include "common-hal/mdns/RemoteService.h"

extern const mp_obj_type_t mdns_remoteservice_type;

const char *common_hal_mdns_remoteservice_get_service_type(mdns_remoteservice_obj_t *self);
const char *common_hal_mdns_remoteservice_get_protocol(mdns_remoteservice_obj_t *self);
const char *common_hal_mdns_remoteservice_get_instance_name(mdns_remoteservice_obj_t *self);
const char *common_hal_mdns_remoteservice_get_hostname(mdns_remoteservice_obj_t *self);
mp_int_t common_hal_mdns_remoteservice_get_port(mdns_remoteservice_obj_t *self);
mp_obj_t common_hal_mdns_remoteservice_get_ipv4(mdns_remoteservice_obj_t *self);
void common_hal_mdns_remoteservice_deinit(mdns_remoteservice_obj_t *self);

// For internal use.
uint32_t mdns_remoteservice_get_ipv4(mdns_remoteservice_obj_t *self);
