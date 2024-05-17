// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include "py/gc.h"
#include "py/misc.h"
#include "supervisor/port.h"

#if defined(CONFIG_IDF_TARGET_ESP32S3) || defined(CONFIG_IDF_TARGET_ESP32S2)
#include "components/heap/include/esp_heap_caps.h"
#define _PM_allocate(x) heap_caps_malloc(x, MALLOC_CAP_DMA | MALLOC_CAP_8BIT)
#define _PM_free(x) heap_caps_free(x)
#else
#define _PM_allocate(x) port_malloc(x, true)
#define _PM_free(x) port_free(x)
#endif
