/* This file is part of the MicroPython project, http://micropython.org/
 * The MIT License (MIT)
 * Copyright (c) 2022-2023 Damien P. George
 */

// Include common MicroPython embed configuration.
#include <port/mpconfigport_common.h>

// Use the minimal starting configuration (disables all optional features).
#define MICROPY_CONFIG_ROM_LEVEL                (MICROPY_CONFIG_ROM_LEVEL_MINIMUM)

// MicroPython configuration.
#define MICROPY_ENABLE_COMPILER                 (1)
#define MICROPY_PERSISTENT_CODE_LOAD            (1)
#define MICROPY_ENABLE_GC                       (1)
#define MICROPY_PY_GC                           (1)

#define MICROPY_PY_SYS                          (0)

#define MICROPY_PY_IO                           (1)
#define MICROPY_PY_JSON                         (1)
#define MICROPY_PY_MATH                         (1)

#define MICROPY_FLOAT_IMPL                      (MICROPY_FLOAT_IMPL_FLOAT)

// Enable C++ function call API.
#define MICROPY_CALLABLE_STRING_FUNCTION        (1)

// Disable NDEBUG, also defined in ESP-IDF.
#define NDEBUG                                  (1)
