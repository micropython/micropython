// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Provided by supervisor/usb.c that has a shared, non-port-specific header. So,
// just define it here.
void init_usb_instance(mp_int_t instance);
