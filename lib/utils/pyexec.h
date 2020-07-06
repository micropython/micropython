/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef MICROPY_INCLUDED_LIB_UTILS_PYEXEC_H
#define MICROPY_INCLUDED_LIB_UTILS_PYEXEC_H

#include "py/obj.h"

typedef enum {
    PYEXEC_MODE_FRIENDLY_REPL,
    PYEXEC_MODE_RAW_REPL,
} pyexec_mode_kind_t;

extern pyexec_mode_kind_t pyexec_mode_kind;

// Set this to the value (eg PYEXEC_FORCED_EXIT) that will be propagated through
// the pyexec functions if a SystemExit exception is raised by the running code.
// It will reset to 0 at the start of each execution (eg each REPL entry).
extern int pyexec_system_exit;

#define PYEXEC_FORCED_EXIT (0x100)
#define PYEXEC_SWITCH_MODE (0x200)

int pyexec_raw_repl(void);
int pyexec_friendly_repl(void);
int pyexec_file(const char *filename);
int pyexec_file_if_exists(const char *filename);
int pyexec_frozen_module(const char *name);
void pyexec_event_repl_init(void);
int pyexec_event_repl_process_char(int c);
extern uint8_t pyexec_repl_active;

#if MICROPY_REPL_INFO
mp_obj_t pyb_set_repl_info(mp_obj_t o_value);
MP_DECLARE_CONST_FUN_OBJ_1(pyb_set_repl_info_obj);
#endif

#endif // MICROPY_INCLUDED_LIB_UTILS_PYEXEC_H
