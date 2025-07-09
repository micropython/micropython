/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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

#include <stdio.h>
#include <string.h>

#include "py/objstr.h"
#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "extmod/misc.h"
#include "extmod/vfs.h"
#include "extmod/vfs_lfs.h"
#include "mpversion.h"
#include "helios_dev.h"
#if MICROPY_VFS_QUECFS
#include "vfs_quecfs.h"
#endif

#if MICROPY_QPY_MODULE_UOS

static char sysname[30] = {0};
static char nodename[20] = {0};
static char machine[30] = {0};

extern const mp_obj_type_t mp_fat_vfs_type;


static const qstr os_uname_info_fields[] = {
    MP_QSTR_sysname, MP_QSTR_nodename,MP_QSTR_release,
    MP_QSTR_version, MP_QSTR_machine, MP_QSTR_qpyver,
};

static  MP_DEFINE_STR_OBJ(os_uname_info_sysname_obj, sysname);
static  MP_DEFINE_STR_OBJ(os_uname_info_nodename_obj, nodename);
static  MP_DEFINE_STR_OBJ(os_uname_info_release_obj, MICROPY_VERSION_STRING);
static  MP_DEFINE_STR_OBJ(os_uname_info_version_obj, MICROPY_GIT_TAG " on " MICROPY_BUILD_DATE);
static  MP_DEFINE_STR_OBJ(os_uname_info_machine_obj, machine);
static  MP_DEFINE_STR_OBJ(os_uname_info_qpyver_obj, QUECPYTHON_VERSION_STRING);

static MP_DEFINE_ATTRTUPLE(
    os_uname_info_obj,
    os_uname_info_fields,
    6,
    (mp_obj_t)&os_uname_info_sysname_obj,
    (mp_obj_t)&os_uname_info_nodename_obj,
    (mp_obj_t)&os_uname_info_release_obj,
    (mp_obj_t)&os_uname_info_version_obj,
    (mp_obj_t)&os_uname_info_machine_obj,
    (mp_obj_t)&os_uname_info_qpyver_obj
    );

static mp_obj_t os_uname2(void) {
	Helios_Dev_GetProductName((void *)sysname, sizeof(sysname));
    Helios_Dev_GetModel((void *)nodename, sizeof(nodename));
	snprintf(machine, sizeof(machine), "%s with QUECTEL", nodename);
	
	os_uname_info_sysname_obj.len = strlen(sysname);
	os_uname_info_nodename_obj.len = strlen(nodename);
	os_uname_info_machine_obj.len = strlen(machine);
	os_uname_info_qpyver_obj.len = strlen(QUECPYTHON_VERSION_STRING);
	return (mp_obj_t)&os_uname_info_obj;
}
static MP_DEFINE_CONST_FUN_OBJ_0(os_uname2_obj, os_uname2);


static mp_obj_t os_uname(void) {
	char sysname[40] = {0};
	char nodname[20] = {0};
	char release[20] = {0};
	char machine[30] = {0};
	char version[128] = {0};
	//char qpy_ver[20] = {0};

    char mob_usb_product[64] = {0};
    char mob_model_id[64] = {0};
    //char _qpy_ver[20] = {0};

    Helios_Dev_GetProductName((void *)mob_usb_product, sizeof(mob_usb_product));
    Helios_Dev_GetModel((void *)mob_model_id, sizeof(mob_model_id));
    //Helios_Dev_GetQpyVersion((void *)_qpy_ver, sizeof(_qpy_ver));

	snprintf(sysname, sizeof(sysname), "sysname=%s", mob_usb_product);
	snprintf(nodname, sizeof(nodname), "nodename=%s", mob_model_id);
	snprintf(release, sizeof(release), "release=%s", MICROPY_VERSION_STRING);
	snprintf(version, sizeof(version), "version=%s on %s", MICROPY_GIT_TAG, MICROPY_BUILD_DATE);
	snprintf(machine, sizeof(machine), "machine=%s with QUECTEL", mob_model_id);
	//snprintf(qpy_ver, sizeof(qpy_ver), "qpyver=%s", _qpy_ver);

	mp_obj_t tuple[6] = {
		mp_obj_new_str(sysname, strlen(sysname)), 
		mp_obj_new_str(nodname, strlen(nodname)), 
		mp_obj_new_str(release, strlen(release)),
		mp_obj_new_str(version, strlen(version)), 
		mp_obj_new_str(machine, strlen(machine)), 
		mp_obj_new_str("qpyver="QUECPYTHON_VERSION_STRING, strlen("qpyver="QUECPYTHON_VERSION_STRING)),
	};
			
    return mp_obj_new_tuple(6, tuple);
}


static MP_DEFINE_CONST_FUN_OBJ_0(os_uname_obj, os_uname);

#include "quectel_version.h"
static mp_obj_t os_sdkver(void) {
#if defined(PLAT_Qualcomm)
    return mp_obj_new_str(mob_sw_rev, strlen(mob_sw_rev));
#else
    extern mp_obj_t queclib_dev_fw_version();
    return queclib_dev_fw_version();
#endif    
}

static MP_DEFINE_CONST_FUN_OBJ_0(os_sdkver_obj, os_sdkver);

// Work out if the seed will be set on import or not.
#if MICROPY_MODULE_BUILTIN_INIT && defined(MICROPY_PY_URANDOM_SEED_INIT_FUNC)
#define OS_URANDOM_SEED_ON_IMPORT (1)
#else
#define OS_URANDOM_SEED_ON_IMPORT (0)
#endif

#if OS_URANDOM_SEED_ON_IMPORT
static mp_obj_t os_urandom___init__(void) {
    // This module may be imported by more than one name so need to ensure
    // that it's only ever seeded once.
    static bool seeded = false;
    if (!seeded) {
        seeded = true;
        srand(MICROPY_PY_URANDOM_SEED_INIT_FUNC);
    }
    return mp_const_none;

}
static MP_DEFINE_CONST_FUN_OBJ_0(os_urandom___init___obj, os_urandom___init__);
#endif

#if MICROPY_PY_OS_DUPTERM
static mp_obj_t os_dupterm_notify(mp_obj_t obj_in) {
    (void)obj_in;
    for (;;) {
        int c = mp_uos_dupterm_rx_chr();
        if (c < 0) {
            break;
        }
        ringbuf_put(&stdin_ringbuf, c);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(os_dupterm_notify_obj, os_dupterm_notify);
#endif

extern const mp_obj_type_t helios_flash_device_type;
static const mp_rom_map_elem_t os_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_uos) },
    #if OS_URANDOM_SEED_ON_IMPORT
    { MP_ROM_QSTR(MP_QSTR___init__), MP_ROM_PTR(&os_urandom___init___obj) },
    #endif
	{ MP_ROM_QSTR(MP_QSTR_uname), MP_ROM_PTR(&os_uname_obj) },
	{ MP_ROM_QSTR(MP_QSTR_uname2), MP_ROM_PTR(&os_uname2_obj) },
    { MP_ROM_QSTR(MP_QSTR_sdkver), MP_ROM_PTR(&os_sdkver_obj) },
    #if MICROPY_PY_OS_DUPTERM
    { MP_ROM_QSTR(MP_QSTR_dupterm), MP_ROM_PTR(&mp_uos_dupterm_obj) },
    { MP_ROM_QSTR(MP_QSTR_dupterm_notify), MP_ROM_PTR(&os_dupterm_notify_obj) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_FlashDevice), MP_ROM_PTR(&helios_flash_device_type) },
    #if MICROPY_VFS
    { MP_ROM_QSTR(MP_QSTR_ilistdir), MP_ROM_PTR(&mp_vfs_ilistdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_listdir), MP_ROM_PTR(&mp_vfs_listdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_mkdir), MP_ROM_PTR(&mp_vfs_mkdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_rmdir), MP_ROM_PTR(&mp_vfs_rmdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_chdir), MP_ROM_PTR(&mp_vfs_chdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_getcwd), MP_ROM_PTR(&mp_vfs_getcwd_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove), MP_ROM_PTR(&mp_vfs_remove_obj) },
    { MP_ROM_QSTR(MP_QSTR_rename), MP_ROM_PTR(&mp_vfs_rename_obj) },
    { MP_ROM_QSTR(MP_QSTR_stat), MP_ROM_PTR(&mp_vfs_stat_obj) },
    { MP_ROM_QSTR(MP_QSTR_statvfs), MP_ROM_PTR(&mp_vfs_statvfs_obj) },
    { MP_ROM_QSTR(MP_QSTR_mount), MP_ROM_PTR(&mp_vfs_mount_obj) },
    { MP_ROM_QSTR(MP_QSTR_umount), MP_ROM_PTR(&mp_vfs_umount_obj) },
    #if MICROPY_VFS_FAT
    { MP_ROM_QSTR(MP_QSTR_VfsFat), MP_ROM_PTR(&mp_fat_vfs_type) },
    #endif
    #if MICROPY_VFS_LFS1
    { MP_ROM_QSTR(MP_QSTR_VfsLfs1), MP_ROM_PTR(&mp_type_vfs_lfs1) },
    #endif
    #if MICROPY_VFS_LFS2
    { MP_ROM_QSTR(MP_QSTR_VfsLfs2), MP_ROM_PTR(&mp_type_vfs_lfs2) },
    #endif
	#if defined(PLAT_Qualcomm)
	{ MP_ROM_QSTR(MP_QSTR_VfsEfs), MP_ROM_PTR(&mp_type_vfs_efs) },
	#endif
    #if defined(PLAT_Unisoc_8910_R05) || defined(PLAT_Unisoc_8910_R06)
        { MP_ROM_QSTR(MP_QSTR_VfsTemp), MP_ROM_PTR(&mp_type_vfs_temp) },
    #endif
    #if defined(PLAT_Unisoc_8910_R05) || defined(PLAT_Unisoc_8910_R06) || (defined(PLAT_Unisoc_8850_R02) && !defined(BOARD_EC800GCN_LD_XBND))
        { MP_ROM_QSTR(MP_QSTR_VfsSd), MP_ROM_PTR(&mp_type_vfs_temp) },
    #endif
#if MICROPY_VFS_FAT_SDIO
    { MP_ROM_QSTR(MP_QSTR_VfsEmmc), MP_ROM_PTR(&mp_type_vfs_sdio_emmc) },
    { MP_ROM_QSTR(MP_QSTR_VfsSd), MP_ROM_PTR(&mp_type_vfs_sdio_sd) },
#endif
#if MICROPY_VFS_QUECFS
    { MP_ROM_QSTR(MP_QSTR_VfsQuecfs), MP_ROM_PTR(&mp_type_vfs_quecfs) },
#endif
    #endif
};

static MP_DEFINE_CONST_DICT(os_module_globals, os_module_globals_table);

const mp_obj_module_t uos_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&os_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_uos, uos_module);
#endif
