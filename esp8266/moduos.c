/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Josef Gajdusek
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

#include <string.h>

#include "py/mpconfig.h"
#include "py/nlr.h"
#include "py/obj.h"
#include "py/objtuple.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "py/mperrno.h"
#include "extmod/misc.h"
#include "genhdr/mpversion.h"
#include "esp_mphal.h"
#include "user_interface.h"

extern const mp_obj_type_t mp_fat_vfs_type;

STATIC const qstr os_uname_info_fields[] = {
    MP_QSTR_sysname, MP_QSTR_nodename,
    MP_QSTR_release, MP_QSTR_version, MP_QSTR_machine
};
STATIC const MP_DEFINE_STR_OBJ(os_uname_info_sysname_obj, MICROPY_PY_SYS_PLATFORM);
STATIC const MP_DEFINE_STR_OBJ(os_uname_info_nodename_obj, MICROPY_PY_SYS_PLATFORM);
STATIC const MP_DEFINE_STR_OBJ(os_uname_info_version_obj, MICROPY_GIT_TAG " on " MICROPY_BUILD_DATE);
STATIC const MP_DEFINE_STR_OBJ(os_uname_info_machine_obj, MICROPY_HW_BOARD_NAME " with " MICROPY_HW_MCU_NAME);

STATIC mp_obj_tuple_t os_uname_info_obj = {
    .base = {&mp_type_attrtuple},
    .len = 5,
    .items = {
        (mp_obj_t)&os_uname_info_sysname_obj,
        (mp_obj_t)&os_uname_info_nodename_obj,
        NULL,
        (mp_obj_t)&os_uname_info_version_obj,
        (mp_obj_t)&os_uname_info_machine_obj,
        (void *)os_uname_info_fields,
    }
};

STATIC mp_obj_t os_uname(void) {
    if (os_uname_info_obj.items[2] == NULL) {
        const char *ver = system_get_sdk_version();
        os_uname_info_obj.items[2] = mp_obj_new_str(ver, strlen(ver), false);
    }
    return (mp_obj_t)&os_uname_info_obj;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(os_uname_obj, os_uname);

#if MICROPY_VFS_FAT
mp_obj_t vfs_proxy_call(qstr method_name, mp_uint_t n_args, const mp_obj_t *args) {
    if (MP_STATE_PORT(fs_user_mount)[0] == NULL) {
        nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(MP_ENODEV)));
    }

    mp_obj_t meth[n_args + 2];
    mp_load_method(MP_STATE_PORT(fs_user_mount)[0], method_name, meth);
    if (args != NULL) {
        memcpy(meth + 2, args, n_args * sizeof(*args));
    }
    return mp_call_method_n_kw(n_args, 0, meth);
}

STATIC mp_obj_t os_listdir(mp_uint_t n_args, const mp_obj_t *args) {
    return vfs_proxy_call(MP_QSTR_listdir, n_args, args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(os_listdir_obj, 0, 1, os_listdir);

STATIC mp_obj_t os_mkdir(mp_obj_t path_in) {
    return vfs_proxy_call(MP_QSTR_mkdir, 1, &path_in);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(os_mkdir_obj, os_mkdir);

STATIC mp_obj_t os_rmdir(mp_obj_t path_in) {
    return vfs_proxy_call(MP_QSTR_rmdir, 1, &path_in);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(os_rmdir_obj, os_rmdir);

STATIC mp_obj_t os_chdir(mp_obj_t path_in) {
    return vfs_proxy_call(MP_QSTR_chdir, 1, &path_in);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(os_chdir_obj, os_chdir);

STATIC mp_obj_t os_getcwd(void) {
    return vfs_proxy_call(MP_QSTR_getcwd, 0, NULL);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(os_getcwd_obj, os_getcwd);

STATIC mp_obj_t os_remove(mp_obj_t path_in) {
    return vfs_proxy_call(MP_QSTR_remove, 1, &path_in);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(os_remove_obj, os_remove);

STATIC mp_obj_t os_rename(mp_obj_t path_old, mp_obj_t path_new) {
    mp_obj_t args[2];
    args[0] = path_old;
    args[1] = path_new;
    return vfs_proxy_call(MP_QSTR_rename, 2, args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(os_rename_obj, os_rename);

STATIC mp_obj_t os_umount(void) {
    return vfs_proxy_call(MP_QSTR_umount, 0, NULL);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(os_umount_obj, os_umount);
#endif

STATIC mp_obj_t os_urandom(mp_obj_t num) {
    mp_int_t n = mp_obj_get_int(num);
    vstr_t vstr;
    vstr_init_len(&vstr, n);
    for (int i = 0; i < n; i++) {
        vstr.buf[i] = *WDEV_HWRNG;
    }
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(os_urandom_obj, os_urandom);

STATIC mp_obj_t os_dupterm_notify(mp_obj_t obj_in) {
    (void)obj_in;
    mp_hal_signal_dupterm_input();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(os_dupterm_notify_obj, os_dupterm_notify);

STATIC mp_obj_t os_stat(mp_obj_t path_in) {
    return vfs_proxy_call(MP_QSTR_stat, 1, &path_in);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(os_stat_obj, os_stat);

STATIC const mp_rom_map_elem_t os_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_uos) },
    { MP_ROM_QSTR(MP_QSTR_uname), MP_ROM_PTR(&os_uname_obj) },
    { MP_ROM_QSTR(MP_QSTR_urandom), MP_ROM_PTR(&os_urandom_obj) },
    #if MICROPY_PY_OS_DUPTERM
    { MP_ROM_QSTR(MP_QSTR_dupterm), MP_ROM_PTR(&mp_uos_dupterm_obj) },
    { MP_ROM_QSTR(MP_QSTR_dupterm_notify), MP_ROM_PTR(&os_dupterm_notify_obj) },
    #endif
    #if MICROPY_VFS_FAT
    { MP_ROM_QSTR(MP_QSTR_VfsFat), MP_ROM_PTR(&mp_fat_vfs_type) },
    { MP_ROM_QSTR(MP_QSTR_listdir), MP_ROM_PTR(&os_listdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_mkdir), MP_ROM_PTR(&os_mkdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_rmdir), MP_ROM_PTR(&os_rmdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_chdir), MP_ROM_PTR(&os_chdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_getcwd), MP_ROM_PTR(&os_getcwd_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove), MP_ROM_PTR(&os_remove_obj) },
    { MP_ROM_QSTR(MP_QSTR_rename), MP_ROM_PTR(&os_rename_obj) },
    { MP_ROM_QSTR(MP_QSTR_stat), MP_ROM_PTR(&os_stat_obj) },
    { MP_ROM_QSTR(MP_QSTR_umount), MP_ROM_PTR(&os_umount_obj) },
    #endif
};

STATIC MP_DEFINE_CONST_DICT(os_module_globals, os_module_globals_table);

const mp_obj_module_t uos_module = {
    .base = { &mp_type_module },
    .name = MP_QSTR_uos,
    .globals = (mp_obj_dict_t*)&os_module_globals,
};
