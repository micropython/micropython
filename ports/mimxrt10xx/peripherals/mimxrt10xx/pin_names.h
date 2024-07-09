// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#pragma once

// OK to include more than once because FORMAT_PIN may be different.

#ifdef MIMXRT1011_SERIES
#include "MIMXRT1011/pin_names.h"
#elif defined(MIMXRT1021_SERIES)
#include "MIMXRT1021/pin_names.h"
#elif defined(MIMXRT1042_SERIES)
#include "MIMXRT1042/pin_names.h"
#elif defined(MIMXRT1052_SERIES)
#include "MIMXRT1052/pin_names.h"
#elif defined(MIMXRT1062_SERIES)
#include "MIMXRT1062/pin_names.h"
#elif defined(MIMXRT1176_cm7_SERIES)
#include "MIMXRT1176/pin_names.h"
#endif
