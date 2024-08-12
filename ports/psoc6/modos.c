/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014-2018 Paul Sokolovsky
 * Copyright (c) 2017-2022 Damien P. George
 * Copyright (c) 2022-2024 Infineon Technologies AG
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

// std includes
#include <errno.h>
#include <stdlib.h>
#include <string.h>


// micropython includes
#include "py/runtime.h"
#include "py/mphal.h"


#define RAISE_ERRNO(err_flag, error_val) \
    { if (err_flag == -1) \
      { mp_raise_OSError(error_val); } }


void os_init(void) {
}


void os_deinit(void) {

}


#if MICROPY_PY_OS_GETENV_PUTENV_UNSETENV

static mp_obj_t mp_os_getenv(mp_obj_t var_in) {
    const char *s = getenv(mp_obj_str_get_str(var_in));
    if (s == NULL) {
        return mp_const_none;
    }
    return mp_obj_new_str(s, strlen(s));
}
static MP_DEFINE_CONST_FUN_OBJ_1(mp_os_getenv_obj, mp_os_getenv);


static mp_obj_t mp_os_putenv(mp_obj_t var, mp_obj_t value) {
    setenv(mp_obj_str_get_str(var), mp_obj_str_get_str(value), 1);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(mp_os_putenv_obj, mp_os_putenv);


static mp_obj_t mp_os_unsetenv(mp_obj_t var) {
    unsetenv(mp_obj_str_get_str(var));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mp_os_unsetenv_obj, mp_os_unsetenv);

#endif


static mp_obj_t mp_os_system(mp_obj_t cmd_in) {
    const char *cmd = mp_obj_str_get_str(cmd_in);

    MP_THREAD_GIL_EXIT();
    int r = system(cmd);
    MP_THREAD_GIL_ENTER();

    RAISE_ERRNO(r, errno);

    return MP_OBJ_NEW_SMALL_INT(r);
}
static MP_DEFINE_CONST_FUN_OBJ_1(mp_os_system_obj, mp_os_system);


static mp_obj_t mp_os_errno(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        return MP_OBJ_NEW_SMALL_INT(errno);
    }

    errno = mp_obj_get_int(args[0]);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_os_errno_obj, 0, 1, mp_os_errno);
