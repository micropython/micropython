/* This file is part of the MicroPython project, http://micropython.org/
 * The MIT License (MIT)
 * Copyright (c) 2022-2023 Damien P. George
 */

// Include common MicroPython embed configuration.
#include <port/mpconfigport_common.h>

// Use the same starting configuration as on most bare-metal targets.
#define MICROPY_CONFIG_ROM_LEVEL                (MICROPY_CONFIG_ROM_LEVEL_EXTRA_FEATURES)

// MicroPython configuration.
#define MICROPY_ENABLE_COMPILER                 (1)
#define MICROPY_ENABLE_GC                       (1)
#define MICROPY_PY_GC                           (1)

#define MICROPY_PY_SYS_PLATFORM                 "embedded"

// Requires shared/readline/readline.h, don't bother as we have no input.
#define MICROPY_PY_BUILTINS_INPUT               (0)

// Can be enabled once extmod/moductypes.c is included in the build.
#define MICROPY_PY_UCTYPES                      (0)

// Can be enabled once either shared/runtime/sys_stdio_mphal.c or
// extmod/vfs_posix_file.c is included in the build.
#define MICROPY_PY_SYS_STDFILES                 (0)

// Can be enabled if you provide an implementation of
// mp_hal_set_interrupt_char() in mphal.c or include
// shared/runtime/interrupt_char.c in the build.
#define MICROPY_KBD_EXCEPTION                   (0)
