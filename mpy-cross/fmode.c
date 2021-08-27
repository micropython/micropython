// SPDX-FileCopyrightText: Copyright (c) 2013-2016 Damien P. George
// SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
//
// SPDX-License-Identifier: MIT

#include "fmode.h"
#include "py/mpconfig.h"
#include <fcntl.h>
#include <stdlib.h>

// Workaround for setting file translation mode: we must distinguish toolsets
// since mingw has no _set_fmode, and altering msvc's _fmode directly has no effect
STATIC int set_fmode_impl(int mode) {
    #ifndef _MSC_VER
    _fmode = mode;
    return 0;
    #else
    return _set_fmode(mode);
    #endif
}

void set_fmode_binary(void) {
    set_fmode_impl(O_BINARY);
}

void set_fmode_text(void) {
    set_fmode_impl(O_TEXT);
}
