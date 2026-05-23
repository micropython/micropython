#pragma once

/* Reduce Flash size */
#define MICROPY_EMIT_RV32                (0)
#define MICROPY_EMIT_INLINE_RV32         (0)

#include "mpconfigport.h"

/* Needed for bluetooth */
#define MICROPY_PY_RANDOM                (1)
#define MICROPY_PY_BUILTINS_FLOAT        (1)
#define MICROPY_PY_RANDOM_EXTRA_FUNCS    (1)

/* Nice extras */
#define MICROPY_PY_FRAMEBUF              (1)
