// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/_bleio/Attribute.h"

extern void bleio_attribute_gatts_set_security_mode(ble_gap_conn_sec_mode_t *perm, bleio_attribute_security_mode_t security_mode);
