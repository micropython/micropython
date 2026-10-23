/*
 * This file is part of the MicroPython project, http://micropython.org/
 * The MIT License (MIT)
 * Copyright (c) 2025 MicroPython contributors
 */

// The jspi variant is the pyscript variant plus JSPI: inherit its
// configuration (full features, split heap - which the suspension
// gate's depth counter relies on - and weakref) unchanged.
#include "../pyscript/mpconfigvariant.h"

// Enable the JSPI suspension gate and the real jsffi.run_sync().
#define MICROPY_JSPI (1)
