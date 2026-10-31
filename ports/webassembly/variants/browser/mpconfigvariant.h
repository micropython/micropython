/*
 * This file is part of the MicroPython project, http://micropython.org/
 * The MIT License (MIT)
 * Copyright (c) 2025 MicroPython contributors
 */

// The browser variant: full features, the split heap (whose call depth
// counter the suspension gate relies on), weakref and JSPI.
#define MICROPY_CONFIG_ROM_LEVEL                (MICROPY_CONFIG_ROM_LEVEL_FULL_FEATURES)
#define MICROPY_GC_SPLIT_HEAP                   (1)
#define MICROPY_GC_SPLIT_HEAP_AUTO              (1)
#define MICROPY_PY_WEAKREF                      (1)

// Enable the JSPI suspension gate and the real jsffi.run_sync().
#define MICROPY_JSPI (1)
