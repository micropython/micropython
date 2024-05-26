// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdlib.h>
#include <string.h>
#include "py/mpprint.h"
#include "esp_heap_caps.h"
#include "shared-module/audiomp3/__init__.h"
#include "supervisor/port_heap.h"

void *mp3_alloc(size_t sz) {
    void *ptr = heap_caps_malloc(sz, MALLOC_CAP_8BIT);
    if (ptr) {
        memset(ptr, 0, sz);
    }
    return ptr;
}

void mp3_free(void *ptr) {
    heap_caps_free(ptr);
}
