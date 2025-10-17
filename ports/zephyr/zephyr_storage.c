/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 NXP
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

#include "modzephyr.h"
#include "py/runtime.h"

#if MICROPY_VFS
#include "extmod/vfs.h"
#endif

#ifdef CONFIG_DISK_ACCESS
#include <zephyr/storage/disk_access.h>
#endif

#ifdef CONFIG_FLASH_MAP
#include <zephyr/storage/flash_map.h>
#endif

#ifdef CONFIG_DISK_ACCESS

#define DISK_DEFINE_NAME(part) CONCAT(MP_QSTR_, DT_STRING_TOKEN(part, disk_name))

#define FOREACH_DISK(n) MP_ROM_QSTR(DISK_DEFINE_NAME(n)),

typedef struct _zephyr_disk_access_obj_t {
    mp_obj_base_t base;
    const char *pdrv;
    int block_size;
    int block_count;
} zephyr_disk_access_obj_t;

static void zephyr_disk_access_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    zephyr_disk_access_obj_t *self = self_in;
    mp_printf(print, "DiskAccess(%s)", self->pdrv);
}

static mp_obj_t zephyr_disk_access_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    zephyr_disk_access_obj_t *self = mp_obj_malloc(zephyr_disk_access_obj_t, type);
    self->pdrv = mp_obj_str_get_str(args[0]);

    if (disk_access_init(self->pdrv) != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("disk not found"));
    }

    if (disk_access_ioctl(self->pdrv, DISK_IOCTL_GET_SECTOR_SIZE, &self->block_size)) {
        mp_raise_ValueError(MP_ERROR_TEXT("unable to get sector size"));
    }

    if (disk_access_ioctl(self->pdrv, DISK_IOCTL_GET_SECTOR_COUNT, &self->block_count)) {
        mp_raise_ValueError(MP_ERROR_TEXT("unable to get block count"));
    }

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t zephyr_disk_access_readblocks(mp_obj_t self_in, mp_obj_t block_num, mp_obj_t buf) {
    zephyr_disk_access_obj_t *self = self_in;
    mp_buffer_info_t bufinfo;
    int ret;

    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_WRITE);
    ret = disk_access_read(self->pdrv, bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / self->block_size);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_3(zephyr_disk_access_readblocks_obj, zephyr_disk_access_readblocks);

static mp_obj_t zephyr_disk_access_writeblocks(mp_obj_t self_in, mp_obj_t block_num, mp_obj_t buf) {
    zephyr_disk_access_obj_t *self = self_in;
    mp_buffer_info_t bufinfo;
    int ret;

    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_READ);
    ret = disk_access_write(self->pdrv, bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / self->block_size);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_3(zephyr_disk_access_writeblocks_obj, zephyr_disk_access_writeblocks);

static mp_obj_t zephyr_disk_access_ioctl(mp_obj_t self_in, mp_obj_t cmd_in, mp_obj_t arg_in) {
    zephyr_disk_access_obj_t *self = self_in;
    mp_int_t cmd = mp_obj_get_int(cmd_in);
    int buf;
    int ret;

    switch (cmd) {
        case MP_BLOCKDEV_IOCTL_INIT:
        case MP_BLOCKDEV_IOCTL_DEINIT:
            return MP_OBJ_NEW_SMALL_INT(0);

        case MP_BLOCKDEV_IOCTL_SYNC:
            ret = disk_access_ioctl(self->pdrv, DISK_IOCTL_CTRL_SYNC, &buf);
            return MP_OBJ_NEW_SMALL_INT(ret);

        case MP_BLOCKDEV_IOCTL_BLOCK_COUNT:
            return MP_OBJ_NEW_SMALL_INT(self->block_count);

        case MP_BLOCKDEV_IOCTL_BLOCK_SIZE:
            return MP_OBJ_NEW_SMALL_INT(self->block_size);

        default:
            return MP_OBJ_NEW_SMALL_INT(-1);
    }
}
static MP_DEFINE_CONST_FUN_OBJ_3(zephyr_disk_access_ioctl_obj, zephyr_disk_access_ioctl);

static const mp_rom_obj_tuple_t zephyr_disks_tuple = {
    .base = {.type = &mp_type_tuple},
    .items = {
        #ifdef CONFIG_DISK_DRIVER_SDMMC
        DT_FOREACH_STATUS_OKAY(zephyr_sdmmc_disk, FOREACH_DISK)
        #endif
        #ifdef CONFIG_DISK_DRIVER_MMC
        DT_FOREACH_STATUS_OKAY(zephyr_mmc_disk, FOREACH_DISK)
        #endif
        #ifdef CONFIG_DISK_DRIVER_FLASH
        DT_FOREACH_STATUS_OKAY(zephyr_flash_disk, FOREACH_DISK)
        #endif
        #ifdef CONFIG_DISK_DRIVER_RAM
        DT_FOREACH_STATUS_OKAY(zephyr_ram_disk, FOREACH_DISK)
        #endif
    },
    .len = MP_ARRAY_SIZE((mp_rom_obj_t []) {
        #ifdef CONFIG_DISK_DRIVER_SDMMC
        DT_FOREACH_STATUS_OKAY(zephyr_sdmmc_disk, FOREACH_DISK)
        #endif
        #ifdef CONFIG_DISK_DRIVER_MMC
        DT_FOREACH_STATUS_OKAY(zephyr_mmc_disk, FOREACH_DISK)
        #endif
        #ifdef CONFIG_DISK_DRIVER_FLASH
        DT_FOREACH_STATUS_OKAY(zephyr_flash_disk, FOREACH_DISK)
        #endif
        #ifdef CONFIG_DISK_DRIVER_RAM
        DT_FOREACH_STATUS_OKAY(zephyr_ram_disk, FOREACH_DISK)
        #endif
    })
};

static const mp_rom_map_elem_t zephyr_disk_access_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&zephyr_disk_access_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&zephyr_disk_access_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&zephyr_disk_access_ioctl_obj) },
    { MP_ROM_QSTR(MP_QSTR_disks), MP_ROM_PTR(&zephyr_disks_tuple) },
};
static MP_DEFINE_CONST_DICT(zephyr_disk_access_locals_dict, zephyr_disk_access_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    zephyr_disk_access_type,
    MP_QSTR_DiskAccess,
    MP_TYPE_FLAG_NONE,
    make_new, zephyr_disk_access_make_new,
    print, zephyr_disk_access_print,
    locals_dict, &zephyr_disk_access_locals_dict
    );
#endif // CONFIG_DISK_ACCESS

#ifdef CONFIG_FLASH_MAP

#define FLASH_AREA_DEFINE_LABEL(part) CONCAT(MP_QSTR_, DT_STRING_TOKEN(part, label))
#define FLASH_AREA_DEFINE_NB(part) CONCAT(MP_QSTR_, DT_FIXED_PARTITION_ID(part))

#define FLASH_AREA_DEFINE_GETNAME(part) COND_CODE_1(DT_NODE_HAS_PROP(part, label), \
    (FLASH_AREA_DEFINE_LABEL(part)), (FLASH_AREA_DEFINE_NB(part)))

// Helper macro to get erase block size from the flash device
// Note: Some flash controllers have erase-block-size, others (like QSPI NOR) don't
// For devices without this property, use 4096 as a common default for NOR flash
#define FLASH_AREA_GET_ERASE_SIZE(part) \
    DT_PROP_OR(DT_MTD_FROM_FIXED_PARTITION(part), erase_block_size, 4096)

// Create a static tuple for each partition containing (id, erase_block_size)
#define FLASH_AREA_DEFINE_TUPLE(part) \
    static const mp_rom_obj_tuple_t flash_area_tuple_##part = { \
        {&mp_type_tuple}, \
        2, \
        { \
            MP_ROM_INT(DT_FIXED_PARTITION_ID(part)), \
            MP_ROM_INT(FLASH_AREA_GET_ERASE_SIZE(part)), \
        } \
    };

#define FLASH_AREA_TUPLE(part) COND_CODE_1(DT_NODE_HAS_STATUS_OKAY(DT_MTD_FROM_FIXED_PARTITION(part)), \
    (FLASH_AREA_DEFINE_TUPLE(part)), ())

#define FLASH_AREA_DEFINE_DEFINE(part) { MP_ROM_QSTR(FLASH_AREA_DEFINE_GETNAME(part)), MP_ROM_PTR(&flash_area_tuple_##part) },

#define FLASH_AREA_DEFINE(part) COND_CODE_1(DT_NODE_HAS_STATUS_OKAY(DT_MTD_FROM_FIXED_PARTITION(part)), \
    (FLASH_AREA_DEFINE_DEFINE(part)), ())

#define FOREACH_PARTITION(n) DT_FOREACH_CHILD(n, FLASH_AREA_DEFINE)
#define FOREACH_PARTITION_TUPLE(n) DT_FOREACH_CHILD(n, FLASH_AREA_TUPLE)

const mp_obj_type_t zephyr_flash_area_type;

// Generate tuple definitions for all partitions
DT_FOREACH_STATUS_OKAY(fixed_partitions, FOREACH_PARTITION_TUPLE)

typedef struct _zephyr_flash_area_obj_t {
    mp_obj_base_t base;
    const struct flash_area *area;
    int block_size;
    int block_count;
    uint8_t id;
} zephyr_flash_area_obj_t;

static void zephyr_flash_area_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    zephyr_flash_area_obj_t *self = self_in;
    mp_printf(print, "FlashArea(%d)", self->id);
}

static mp_obj_t zephyr_flash_area_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 2, 2, false);
    zephyr_flash_area_obj_t *self = mp_obj_malloc(zephyr_flash_area_obj_t, type);
    self->id = mp_obj_get_int(args[0]);
    self->block_size = mp_obj_get_int(args[1]);

    if (self->block_size <= 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid block size"));
    }

    if (flash_area_open(self->id, &self->area) != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("unable to open flash area"));
    }

    self->block_count = self->area->fa_size / self->block_size;

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t zephyr_flash_area_readblocks(size_t n_args, const mp_obj_t *args) {
    zephyr_flash_area_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t block_num = mp_obj_get_int(args[1]);
    off_t offset = block_num * self->block_size;
    mp_buffer_info_t bufinfo;
    int ret;

    if (n_args >= 4) {
        offset += mp_obj_get_int(args[3]);
    }

    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_WRITE);
    ret = flash_area_read(self->area, offset, bufinfo.buf, bufinfo.len);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(zephyr_flash_area_readblocks_obj, 3, 4, zephyr_flash_area_readblocks);

static mp_obj_t zephyr_flash_area_writeblocks(size_t n_args, const mp_obj_t *args) {
    zephyr_flash_area_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t block_num = mp_obj_get_int(args[1]);
    off_t offset = block_num * self->block_size;
    mp_buffer_info_t bufinfo;
    int ret;

    if (n_args == 3) {
        ret = flash_area_erase(self->area, offset, self->block_size);
        if (ret) {
            return MP_OBJ_NEW_SMALL_INT(ret);
        }
    } else {
        offset += mp_obj_get_int(args[3]);
    }

    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_READ);
    ret = flash_area_write(self->area, offset, bufinfo.buf, bufinfo.len);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(zephyr_flash_area_writeblocks_obj, 3, 4, zephyr_flash_area_writeblocks);

static mp_obj_t zephyr_flash_area_ioctl(mp_obj_t self_in, mp_obj_t cmd_in, mp_obj_t arg_in) {
    zephyr_flash_area_obj_t *self = self_in;
    mp_int_t cmd = mp_obj_get_int(cmd_in);
    mp_int_t block_num = mp_obj_get_int(arg_in);
    int ret;

    switch (cmd) {
        case MP_BLOCKDEV_IOCTL_INIT:
        case MP_BLOCKDEV_IOCTL_DEINIT:
        case MP_BLOCKDEV_IOCTL_SYNC:
            return MP_OBJ_NEW_SMALL_INT(0);

        case MP_BLOCKDEV_IOCTL_BLOCK_COUNT:
            return MP_OBJ_NEW_SMALL_INT(self->block_count);

        case MP_BLOCKDEV_IOCTL_BLOCK_SIZE:
            return MP_OBJ_NEW_SMALL_INT(self->block_size);

        case MP_BLOCKDEV_IOCTL_BLOCK_ERASE:
            ret = flash_area_erase(self->area, block_num * self->block_size, self->block_size);
            return MP_OBJ_NEW_SMALL_INT(ret);

        default:
            return MP_OBJ_NEW_SMALL_INT(-1);
    }
}
static MP_DEFINE_CONST_FUN_OBJ_3(zephyr_flash_area_ioctl_obj, zephyr_flash_area_ioctl);

static const mp_rom_map_elem_t zephyr_flash_areas_table[] = {
    /* Generate list of partition IDs from Zephyr Devicetree */
    DT_FOREACH_STATUS_OKAY(fixed_partitions, FOREACH_PARTITION)
};
static MP_DEFINE_CONST_DICT(zephyr_flash_areas_dict, zephyr_flash_areas_table);

static const mp_rom_map_elem_t zephyr_flash_area_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&zephyr_flash_area_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&zephyr_flash_area_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&zephyr_flash_area_ioctl_obj) },
    { MP_ROM_QSTR(MP_QSTR_areas), MP_ROM_PTR(&zephyr_flash_areas_dict) },
};
static MP_DEFINE_CONST_DICT(zephyr_flash_area_locals_dict, zephyr_flash_area_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    zephyr_flash_area_type,
    MP_QSTR_FlashArea,
    MP_TYPE_FLAG_NONE,
    make_new, zephyr_flash_area_make_new,
    print, zephyr_flash_area_print,
    locals_dict, &zephyr_flash_area_locals_dict
    );
#endif // CONFIG_FLASH_MAP
