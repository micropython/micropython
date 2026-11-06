// flashbdev.c - internal flash block device + littlefs root filesystem
// for the GD32F4xx port.
//
// The last FLASH_BDEV_NUM_SECTORS 128 KB sectors of internal flash are
// reserved for a littlefs2 filesystem mounted at "/".

#include <string.h>

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/objarray.h"
#include "extmod/vfs.h"
#include "extmod/vfs_lfs.h"

#include "gd32f4xx.h"

// 16 KB sector size on GD32F4xx; use the last 4 sectors (64 KB) for the FS.
#define FLASH_BDEV_SECTOR_SIZE   (16 * 1024)
#define FLASH_BDEV_NUM_SECTORS   (4)
#define FLASH_BDEV_SIZE          (FLASH_BDEV_SECTOR_SIZE * FLASH_BDEV_NUM_SECTORS)
#define FLASH_BDEV_START_ADDR    (0x08080000 - FLASH_BDEV_SIZE)   // 0x08070000

#if defined(__GNUC__)
#define FLASH_BDEV_ATTR __attribute__((aligned(4)))
#else
#define FLASH_BDEV_ATTR
#endif

typedef struct _flash_bdev_obj_t {
    mp_obj_base_t base;
} flash_bdev_obj_t;

static uint8_t flash_bdev_cache[FLASH_BDEV_SECTOR_SIZE] FLASH_BDEV_ATTR;
static uint32_t flash_bdev_cache_sector = 0xffffffff;
static bool flash_bdev_cache_dirty = false;

// ---- flash helpers ----
static void flash_bdev_erase_sector(uint32_t sector) {
    fmc_unlock();
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);
    fmc_sector_erase(FLASH_BDEV_START_ADDR + sector * FLASH_BDEV_SECTOR_SIZE);
    fmc_lock();
}

static void flash_bdev_flush(void) {
    if (!flash_bdev_cache_dirty) {
        return;
    }
    flash_bdev_erase_sector(flash_bdev_cache_sector);
    fmc_unlock();
    uint32_t addr = FLASH_BDEV_START_ADDR + flash_bdev_cache_sector * FLASH_BDEV_SECTOR_SIZE;
    for (size_t i = 0; i < FLASH_BDEV_SECTOR_SIZE; i += 4) {
        uint32_t word;
        memcpy(&word, &flash_bdev_cache[i], 4);
        fmc_word_program(addr + i, word);
    }
    fmc_lock();
    flash_bdev_cache_dirty = false;
}

static void flash_bdev_load_sector(uint32_t sector) {
    if (flash_bdev_cache_sector == sector) {
        return;
    }
    flash_bdev_flush();
    flash_bdev_cache_sector = sector;
    memcpy(flash_bdev_cache, (const void *)(FLASH_BDEV_START_ADDR + sector * FLASH_BDEV_SECTOR_SIZE),
           FLASH_BDEV_SECTOR_SIZE);
    flash_bdev_cache_dirty = false;
}

// ---- Python block device protocol ----
static mp_obj_t flash_bdev_readblocks(size_t n_args, const mp_obj_t *args) {
    flash_bdev_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    (void)self;
    uint32_t block_num = mp_obj_get_int(args[1]);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_WRITE);

    size_t len = bufinfo.len;
    if (len > FLASH_BDEV_SECTOR_SIZE) {
        len = FLASH_BDEV_SECTOR_SIZE;
    }

    // optional offset argument (new protocol)
    size_t offset = 0;
    if (n_args > 3) {
        offset = mp_obj_get_int(args[3]);
        if (offset + len > FLASH_BDEV_SECTOR_SIZE) {
            len = FLASH_BDEV_SECTOR_SIZE - offset;
        }
    }

    flash_bdev_load_sector(block_num);
    memcpy(bufinfo.buf, flash_bdev_cache + offset, len);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(flash_bdev_readblocks_obj, 3, 4, flash_bdev_readblocks);

static mp_obj_t flash_bdev_writeblocks(size_t n_args, const mp_obj_t *args) {
    flash_bdev_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    (void)self;
    uint32_t block_num = mp_obj_get_int(args[1]);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_READ);

    size_t len = bufinfo.len;
    if (len > FLASH_BDEV_SECTOR_SIZE) {
        len = FLASH_BDEV_SECTOR_SIZE;
    }

    size_t offset = 0;
    if (n_args > 3) {
        offset = mp_obj_get_int(args[3]);
        if (offset + len > FLASH_BDEV_SECTOR_SIZE) {
            len = FLASH_BDEV_SECTOR_SIZE - offset;
        }
    }

    flash_bdev_load_sector(block_num);
    memcpy(flash_bdev_cache + offset, bufinfo.buf, len);
    flash_bdev_cache_dirty = true;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(flash_bdev_writeblocks_obj, 3, 4, flash_bdev_writeblocks);

static mp_obj_t flash_bdev_ioctl(mp_obj_t self_in, mp_obj_t cmd_in, mp_obj_t arg_in) {
    (void)self_in;
    mp_int_t cmd = mp_obj_get_int(cmd_in);
    switch (cmd) {
        case MP_BLOCKDEV_IOCTL_INIT:
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_DEINIT:
            flash_bdev_flush();
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_SYNC:
            flash_bdev_flush();
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_BLOCK_COUNT:
            return MP_OBJ_NEW_SMALL_INT(FLASH_BDEV_NUM_SECTORS);
        case MP_BLOCKDEV_IOCTL_BLOCK_SIZE:
            return MP_OBJ_NEW_SMALL_INT(FLASH_BDEV_SECTOR_SIZE);
        case MP_BLOCKDEV_IOCTL_BLOCK_ERASE: {
            uint32_t block = mp_obj_get_int(arg_in);
            flash_bdev_load_sector(block);
            memset(flash_bdev_cache, 0xff, FLASH_BDEV_SECTOR_SIZE);
            flash_bdev_cache_dirty = true;
            return MP_OBJ_NEW_SMALL_INT(0);
        }
        default:
            return MP_OBJ_NEW_SMALL_INT(-1);
    }
}
static MP_DEFINE_CONST_FUN_OBJ_3(flash_bdev_ioctl_obj, flash_bdev_ioctl);

static const mp_rom_map_elem_t flash_bdev_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_readblocks),  MP_ROM_PTR(&flash_bdev_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&flash_bdev_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl),       MP_ROM_PTR(&flash_bdev_ioctl_obj) },
};
static MP_DEFINE_CONST_DICT(flash_bdev_locals_dict, flash_bdev_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    flash_bdev_type,
    MP_QSTR_FlashBlockDev,
    MP_TYPE_FLAG_NONE,
    locals_dict, &flash_bdev_locals_dict
    );

static flash_bdev_obj_t flash_bdev_obj = {
    .base = { &flash_bdev_type },
};


mp_obj_t pyb_flash_get_block_device(void);

// ---- machine.Flash: let Python format the littlefs partition ----
// The block device itself is not a Python type here; instead this exposes a
// small object with the operations a user actually needs (format), which keeps
// the surface area minimal.

static mp_obj_t machine_flash_mkfs(void) {
    // Lazy import: vfs is only available when MICROPY_VFS is enabled.
    mp_obj_t vfs_mod = mp_import_name(MP_QSTR_vfs, mp_const_none, MP_OBJ_NEW_SMALL_INT(0));
    mp_obj_t bdev = pyb_flash_get_block_device();
    mp_obj_t args[2] = { bdev, MP_OBJ_NEW_SMALL_INT(0) };
    // vfs.VfsLfs2.mkfs(bdev)
    mp_obj_t vfslfs2 = mp_load_attr(vfs_mod, MP_QSTR_VfsLfs2);
    mp_obj_t mkfs = mp_load_attr(vfslfs2, MP_QSTR_mkfs);
    mp_call_function_n_kw(mkfs, 1, 0, args);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(machine_flash_mkfs_obj, machine_flash_mkfs);

static const mp_rom_map_elem_t machine_flash_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_mkfs), MP_ROM_PTR(&machine_flash_mkfs_obj) },
};
static MP_DEFINE_CONST_DICT(machine_flash_locals_dict, machine_flash_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_flash_type,
    MP_QSTR_Flash,
    MP_TYPE_FLAG_NONE,
    locals_dict, &machine_flash_locals_dict
    );

mp_obj_t pyb_flash_get_block_device(void) {
    return MP_OBJ_FROM_PTR(&flash_bdev_obj);
}
