/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014-2015 Paul Sokolovsky
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

#if MICROPY_PY_TERMIOS

#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>

#include "py/objlist.h"
#include "py/runtime.h"
#include "py/mphal.h"

STATIC mp_obj_t mod_termios_tcgetattr(mp_obj_t fd_in) {
    struct termios term;
    int fd = mp_obj_get_int(fd_in);

    int res = tcgetattr(fd, &term);
    RAISE_ERRNO(res, errno);

    mp_obj_list_t *r = MP_OBJ_TO_PTR(mp_obj_new_list(7, NULL));
    r->items[0] = MP_OBJ_NEW_SMALL_INT(term.c_iflag);
    r->items[1] = MP_OBJ_NEW_SMALL_INT(term.c_oflag);
    r->items[2] = MP_OBJ_NEW_SMALL_INT(term.c_cflag);
    r->items[3] = MP_OBJ_NEW_SMALL_INT(term.c_lflag);
    r->items[4] = MP_OBJ_NEW_SMALL_INT(cfgetispeed(&term));
    r->items[5] = MP_OBJ_NEW_SMALL_INT(cfgetospeed(&term));

    mp_obj_list_t *cc = MP_OBJ_TO_PTR(mp_obj_new_list(NCCS, NULL));
    r->items[6] = MP_OBJ_FROM_PTR(cc);
    for (int i = 0; i < NCCS; i++) {
        if (i == VMIN || i == VTIME) {
            cc->items[i] = MP_OBJ_NEW_SMALL_INT(term.c_cc[i]);
        } else {
            // https://docs.python.org/3/library/termios.html says value is *string*,
            // but no way unicode chars could be there, if c_cc is defined to be a
            // a "char". But it's type is actually cc_t, which can be anything.
            // TODO: For now, we still deal with it like that.
            cc->items[i] = mp_obj_new_bytes((byte *)&term.c_cc[i], 1);
        }
    }
    return MP_OBJ_FROM_PTR(r);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_termios_tcgetattr_obj, mod_termios_tcgetattr);

STATIC mp_obj_t mod_termios_tcsetattr(mp_obj_t fd_in, mp_obj_t when_in, mp_obj_t attrs_in) {
    struct termios term;
    int fd = mp_obj_get_int(fd_in);
    int when = mp_obj_get_int(when_in);
    if (when == 0) {
        // We don't export TCSANOW and friends to save on code space. Then
        // common lazy sense says that passing 0 should be godo enough, and
        // it is e.g. for glibc. But for other libc's it's not, so set just
        // treat 0 as defaulting to TCSANOW.
        when = TCSANOW;
    }

    assert(mp_obj_is_type(attrs_in, &mp_type_list));
    mp_obj_list_t *attrs = MP_OBJ_TO_PTR(attrs_in);

    term.c_iflag = mp_obj_get_int(attrs->items[0]);
    term.c_oflag = mp_obj_get_int(attrs->items[1]);
    term.c_cflag = mp_obj_get_int(attrs->items[2]);
    term.c_lflag = mp_obj_get_int(attrs->items[3]);

    mp_obj_list_t *cc = MP_OBJ_TO_PTR(attrs->items[6]);
    for (int i = 0; i < NCCS; i++) {
        if (i == VMIN || i == VTIME) {
            term.c_cc[i] = mp_obj_get_int(cc->items[i]);
        } else {
            term.c_cc[i] = *mp_obj_str_get_str(cc->items[i]);
        }
    }

    int res = cfsetispeed(&term, mp_obj_get_int(attrs->items[4]));
    RAISE_ERRNO(res, errno);
    res = cfsetospeed(&term, mp_obj_get_int(attrs->items[5]));
    RAISE_ERRNO(res, errno);

    res = tcsetattr(fd, when, &term);
    RAISE_ERRNO(res, errno);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(mod_termios_tcsetattr_obj, mod_termios_tcsetattr);

STATIC mp_obj_t mod_termios_setraw(mp_obj_t fd_in) {
    struct termios term;
    int fd = mp_obj_get_int(fd_in);
    int res = tcgetattr(fd, &term);
    RAISE_ERRNO(res, errno);

    term.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    term.c_oflag = 0;
    term.c_cflag = (term.c_cflag & ~(CSIZE | PARENB)) | CS8;
    term.c_lflag = 0;
    term.c_cc[VMIN] = 1;
    term.c_cc[VTIME] = 0;
    res = tcsetattr(fd, TCSAFLUSH, &term);
    RAISE_ERRNO(res, errno);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_termios_setraw_obj, mod_termios_setraw);

STATIC const mp_rom_map_elem_t mp_module_termios_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_termios) },
    { MP_ROM_QSTR(MP_QSTR_tcgetattr), MP_ROM_PTR(&mod_termios_tcgetattr_obj) },
    { MP_ROM_QSTR(MP_QSTR_tcsetattr), MP_ROM_PTR(&mod_termios_tcsetattr_obj) },
    { MP_ROM_QSTR(MP_QSTR_setraw), MP_ROM_PTR(&mod_termios_setraw_obj) },

#define C(name) { MP_ROM_QSTR(MP_QSTR_##name), MP_ROM_INT(name) }
    C(TCSANOW),

    C(B9600),
    #ifdef B57600
    C(B57600),
    #endif
    #ifdef B115200
    C(B115200),
    #endif
#undef C
};

STATIC MP_DEFINE_CONST_DICT(mp_module_termios_globals, mp_module_termios_globals_table);

const mp_obj_module_t mp_module_termios = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_termios_globals,
};

MP_REGISTER_MODULE(MP_QSTR_termios, mp_module_termios);

#endif // MICROPY_PY_TERMIOS
