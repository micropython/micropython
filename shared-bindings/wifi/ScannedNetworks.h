// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
// SPDX-FileCopyrightText: Copyright (c) 2017 Glenn Ruben Bakke
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "common-hal/wifi/ScannedNetworks.h"

extern const mp_obj_type_t wifi_scannednetworks_type;

mp_obj_t common_hal_wifi_scannednetworks_next(wifi_scannednetworks_obj_t *self);
