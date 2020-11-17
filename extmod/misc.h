// Copyright (c) 2016 Paul Sokolovsky
// SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
// SPDX-FileCopyrightText: Copyright (c) 2014-2016 Damien P. George
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_EXTMOD_MISC_H
#define MICROPY_INCLUDED_EXTMOD_MISC_H

// This file contains cumulative declarations for extmod/ .

#include <stddef.h>
#include "py/runtime.h"

MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(mp_uos_dupterm_obj);

#if MICROPY_PY_OS_DUPTERM
int mp_uos_dupterm_rx_chr(void);
void mp_uos_dupterm_tx_strn(const char *str, size_t len);
void mp_uos_deactivate(size_t dupterm_idx, const char *msg, mp_obj_t exc);
#else
#define mp_uos_dupterm_tx_strn(s, l)
#endif

#endif // MICROPY_INCLUDED_EXTMOD_MISC_H
