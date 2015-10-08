/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014, 2015 Damien P. George
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

#ifndef MODMACHINE_H_
#define MODMACHINE_H_

#include "py/mpstate.h"
#include "py/nlr.h"
#include "py/obj.h"

MP_DECLARE_CONST_FUN_OBJ(machine_bootloader_obj);
MP_DECLARE_CONST_FUN_OBJ(machine_deepsleep_obj);
MP_DECLARE_CONST_FUN_OBJ(machine_freq_obj);
MP_DECLARE_CONST_FUN_OBJ(machine_info_obj);
MP_DECLARE_CONST_FUN_OBJ(machine_main_obj);         // defined in main.c
MP_DECLARE_CONST_FUN_OBJ(machine_reset_obj);
MP_DECLARE_CONST_FUN_OBJ(machine_sleep_obj);
MP_DECLARE_CONST_FUN_OBJ(machine_unique_id_obj);

#endif  // MODMACHINE_H_
