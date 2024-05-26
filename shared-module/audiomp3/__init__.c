// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdlib.h>
#include "py/mpconfig.h"
#include "py/misc.h"
#include "shared-module/audiomp3/__init__.h"

MP_WEAK void *mp3_alloc(size_t sz) {
    return m_malloc_maybe(sz);
}

MP_WEAK void mp3_free(void *ptr) {
    m_free(ptr);
}
