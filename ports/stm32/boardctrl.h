/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Damien P. George
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
#ifndef MICROPY_INCLUDED_STM32_BOARDCTRL_H
#define MICROPY_INCLUDED_STM32_BOARDCTRL_H

#include "py/mpconfig.h"

#ifndef MICROPY_BOARD_EARLY_INIT
#define MICROPY_BOARD_EARLY_INIT()
#endif

#ifndef MICROPY_BOARD_BEFORE_SOFT_RESET_LOOP
#define MICROPY_BOARD_BEFORE_SOFT_RESET_LOOP boardctrl_before_soft_reset_loop
#endif

#ifndef MICROPY_BOARD_TOP_SOFT_RESET_LOOP
#define MICROPY_BOARD_TOP_SOFT_RESET_LOOP boardctrl_top_soft_reset_loop
#endif

#ifndef MICROPY_BOARD_BEFORE_BOOT_PY
#define MICROPY_BOARD_BEFORE_BOOT_PY boardctrl_before_boot_py
#endif

#ifndef MICROPY_BOARD_AFTER_BOOT_PY
#define MICROPY_BOARD_AFTER_BOOT_PY boardctrl_after_boot_py
#endif

#ifndef MICROPY_BOARD_BEFORE_MAIN_PY
#define MICROPY_BOARD_BEFORE_MAIN_PY boardctrl_before_main_py
#endif

#ifndef MICROPY_BOARD_AFTER_MAIN_PY
#define MICROPY_BOARD_AFTER_MAIN_PY boardctrl_after_main_py
#endif

#ifndef MICROPY_BOARD_START_SOFT_RESET
#define MICROPY_BOARD_START_SOFT_RESET boardctrl_start_soft_reset
#endif

#ifndef MICROPY_BOARD_END_SOFT_RESET
#define MICROPY_BOARD_END_SOFT_RESET boardctrl_end_soft_reset
#endif

typedef struct _boardctrl_state_t {
    uint8_t reset_mode;
    bool run_boot_py;
    bool run_main_py;
    bool log_soft_reset;
    int last_ret;
} boardctrl_state_t;

void boardctrl_before_soft_reset_loop(boardctrl_state_t *state);
void boardctrl_top_soft_reset_loop(boardctrl_state_t *state);
void boardctrl_before_boot_py(boardctrl_state_t *state);
void boardctrl_after_boot_py(boardctrl_state_t *state);
void boardctrl_before_main_py(boardctrl_state_t *state);
void boardctrl_after_main_py(boardctrl_state_t *state);
void boardctrl_start_soft_reset(boardctrl_state_t *state);
void boardctrl_end_soft_reset(boardctrl_state_t *state);

#endif // MICROPY_INCLUDED_STM32_BOARDCTRL_H
