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
#ifndef MICROPY_INCLUDED_STMHAL_PORTMODULES_H
#define MICROPY_INCLUDED_STMHAL_PORTMODULES_H

extern const mp_obj_module_t pyb_module;
extern const mp_obj_module_t stm_module;
extern const mp_obj_module_t mp_module_uos;
extern const mp_obj_module_t mp_module_utime;
extern const mp_obj_module_t mp_module_usocket;

// additional helper functions exported by the modules

MP_DECLARE_CONST_FUN_OBJ_1(time_sleep_ms_obj);
MP_DECLARE_CONST_FUN_OBJ_1(time_sleep_us_obj);

MP_DECLARE_CONST_FUN_OBJ_0(mod_os_sync_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(mod_os_dupterm_obj);

#endif // MICROPY_INCLUDED_STMHAL_PORTMODULES_H
