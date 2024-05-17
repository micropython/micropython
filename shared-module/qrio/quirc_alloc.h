// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 by Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/gc.h"

#if !MICROPY_GC_CONSERVATIVE_CLEAR
// so that we can implement calloc as m_malloc
#error Requires MICROPY_GC_CONSERVATIVE_CLEAR
#endif

#define QUIRC_MALLOC(x) m_malloc((x))
#define QUIRC_CALLOC(x, y) m_malloc((x) * (y))
#define QUIRC_FREE(x) gc_free((x))

#define QUIRC_SMALL_STACK (1)
