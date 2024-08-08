// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#ifdef PICO_RP2040
#include "Framebuffer_RP2040.h"
#else
#include "Framebuffer_RP2350.h"
#endif
