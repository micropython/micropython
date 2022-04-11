/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014-2018 Paul Sokolovsky
 * Copyright (c) 2017-2022 Damien P. George
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

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#include "py/runtime.h"
#include "py/mphal.h"

#if defined(__GLIBC__) && defined(__GLIBC_PREREQ)
#if __GLIBC_PREREQ(2, 25)
#include <sys/random.h>
#define _HAVE_GETRANDOM
#endif
#endif

STATIC mp_obj_t mp_uos_getenv(mp_obj_t var_in) {
    const char *s = getenv(mp_obj_str_get_str(var_in));
    if (s == NULL) {
        return mp_const_none;
    }
    return mp_obj_new_str(s, strlen(s));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_uos_getenv_obj, mp_uos_getenv);

STATIC mp_obj_t mp_uos_putenv(mp_obj_t key_in, mp_obj_t value_in) {
    const char *key = mp_obj_str_get_str(key_in);
    const char *value = mp_obj_str_get_str(value_in);
    int ret;

    #if _WIN32
    ret = _putenv_s(key, value);
    #else
    ret = setenv(key, value, 1);
    #endif

    if (ret == -1) {
        mp_raise_OSError(errno);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mp_uos_putenv_obj, mp_uos_putenv);

STATIC mp_obj_t mp_uos_unsetenv(mp_obj_t key_in) {
    const char *key = mp_obj_str_get_str(key_in);
    int ret;

    #if _WIN32
    ret = _putenv_s(key, "");
    #else
    ret = unsetenv(key);
    #endif

    if (ret == -1) {
        mp_raise_OSError(errno);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_uos_unsetenv_obj, mp_uos_unsetenv);

STATIC mp_obj_t mp_uos_system(mp_obj_t cmd_in) {
    const char *cmd = mp_obj_str_get_str(cmd_in);

    MP_THREAD_GIL_EXIT();
    int r = system(cmd);
    MP_THREAD_GIL_ENTER();

    RAISE_ERRNO(r, errno);

    return MP_OBJ_NEW_SMALL_INT(r);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_uos_system_obj, mp_uos_system);

STATIC mp_obj_t mp_uos_urandom(mp_obj_t num) {
    mp_int_t n = mp_obj_get_int(num);
    vstr_t vstr;
    vstr_init_len(&vstr, n);
    #ifdef _HAVE_GETRANDOM
    RAISE_ERRNO(getrandom(vstr.buf, n, 0), errno);
    #else
    int fd = open("/dev/urandom", O_RDONLY);
    RAISE_ERRNO(fd, errno);
    RAISE_ERRNO(read(fd, vstr.buf, n), errno);
    close(fd);
    #endif
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_uos_urandom_obj, mp_uos_urandom);

STATIC mp_obj_t mp_uos_errno(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        return MP_OBJ_NEW_SMALL_INT(errno);
    }

    errno = mp_obj_get_int(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_uos_errno_obj, 0, 1, mp_uos_errno);
