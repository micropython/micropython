// flashbdev.c - internal flash block device + littlefs root filesystem
// for the GD32F4xx port.
//
// GD32F4xx (512 KB part) flash sector layout, from the user manual:
//
//   Sector 0  16 KB   0x08000000 - 0x08003FFF
//   Sector 1  16 KB   0x08004000 - 0x08007FFF
//   Sector 2  16 KB   0x08008000 - 0x0800BFFF
//   Sector 3  16 KB   0x0800C000 - 0x0800FFFF
//   Sector 4  64 KB   0x08010000 - 0x0801FFFF
//   Sector 5  128 KB  0x08020000 - 0x0803FFFF
//   Sector 6  128 KB  0x08040000 - 0x0805FFFF
//   Sector 7  128 KB  0x08060000 - 0x0807FFFF
//
// The filesystem occupies Sector 6 and Sector 7 (256 KB), which sit above the
// firmware image on the boards this port targets.
//
// Design notes
// ------------
// * `fmc_sector_erase()` takes a *sector number* (it is written straight into
//   FMC_CTL.SN), not an address.  Passing an address silently selects sector 0
//   and erases the start of the firmware.
// * The logical block size is set equal to the physical sector size.  littlefs
//   erases a block before programming it and never reads back a block it is
//   writing, so no read-modify-write buffer is needed.  Keeping block_size ==
//   erase size is what makes that safe, and it costs no RAM beyond the
//   peripheral FIFO.

#include <string.h>

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/objarray.h"
#include "extmod/vfs.h"
#include "extmod/vfs_lfs.h"

#include "gd32f4xx.h"

// Filesystem region: the last two 128 KB sectors.
#define FLASH_BDEV_BLOCK_SIZE    (128 * 1024)                    // == erase size
#define FLASH_BDEV_BLOCK_COUNT   (2)
#define FLASH_BDEV_START_ADDR    (0x08060000)                    // Sector 6
#define FLASH_BDEV_FIRST_SECTOR  (6)                             // physical sector number

_Static_assert(FLASH_BDEV_START_ADDR == 0x08060000, "start must match Sector 6");

typedef struct _flash_bdev_obj_t {
    mp_obj_base_t base;
} flash_bdev_obj_t;

// Translate a block number into the hardware sector number and its address.
static uint32_t flash_bdev_sector_number(uint32_t block) {
    return FLASH_BDEV_FIRST_SECTOR + block;
}

static uint32_t flash_bdev_sector_addr(uint32_t block) {
    return FLASH_BDEV_START_ADDR + block * FLASH_BDEV_BLOCK_SIZE;
}

// ---- flash primitives ----
static bool flash_bdev_erase_block(uint32_t block) {
    fmc_state_enum state;

    fmc_unlock();
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR |
                   FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);
    state = fmc_sector_erase(flash_bdev_sector_number(block));
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR |
                   FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);
    fmc_lock();

    return state == FMC_READY;
}

static bool flash_bdev_program(uint32_t addr, const uint8_t *src, size_t len) {
    bool ok = true;

    fmc_unlock();
    for (size_t i = 0; i < len; i += 4) {
        uint32_t word;
        size_t n = (len - i < 4) ? (len - i) : 4;
        if (n == 4) {
            memcpy(&word, &src[i], 4);
        } else {
            // Pad a short tail with 0xff so we do not write past the request.
            word = 0xffffffffu;
            memcpy(&word, &src[i], n);
        }
        if (fmc_word_program(addr + i, word) != FMC_READY) {
            ok = false;
            break;
        }
    }
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR |
                   FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);
    fmc_lock();

    return ok;
}

// ---- Python block-device protocol ----
// readblocks(block, buf[, offset])
static mp_obj_t flash_bdev_readblocks(size_t n_args, const mp_obj_t *args) {
    uint32_t block = mp_obj_get_int(args[1]);
    if (block >= FLASH_BDEV_BLOCK_COUNT) {
        mp_raise_ValueError(MP_ERROR_TEXT("block out of range"));
    }

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_WRITE);

    size_t offset = 0;
    size_t len = bufinfo.len;
    if (n_args > 3) {
        offset = mp_obj_get_int(args[3]);
    }
    if (offset > FLASH_BDEV_BLOCK_SIZE) {
        offset = FLASH_BDEV_BLOCK_SIZE;
    }
    if (len > FLASH_BDEV_BLOCK_SIZE - offset) {
        len = FLASH_BDEV_BLOCK_SIZE - offset;
    }

    memcpy(bufinfo.buf, (const void *)(flash_bdev_sector_addr(block) + offset), len);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(flash_bdev_readblocks_obj, 3, 4, flash_bdev_readblocks);

// writeblocks(block, buf[, offset])
// littlefs always erases a block first, so a write that starts at offset 0 is
// programmed directly.  A partial write has to fall back to erase-then-program
// of the surrounding data, which we refuse rather than corrupt the block.
static mp_obj_t flash_bdev_writeblocks(size_t n_args, const mp_obj_t *args) {
    uint32_t block = mp_obj_get_int(args[1]);
    if (block >= FLASH_BDEV_BLOCK_COUNT) {
        mp_raise_ValueError(MP_ERROR_TEXT("block out of range"));
    }

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_READ);

    size_t offset = 0;
    size_t len = bufinfo.len;
    if (n_args > 3) {
        offset = mp_obj_get_int(args[3]);
    }

    if (offset != 0) {
        // A partial write would need the rest of the sector, which we do not
        // keep in RAM.  littlefs never does this, so treat it as an error.
        mp_raise_OSError(MP_EINVAL);
    }
    if (len > FLASH_BDEV_BLOCK_SIZE) {
        len = FLASH_BDEV_BLOCK_SIZE;
    }

    if (!flash_bdev_program(flash_bdev_sector_addr(block), bufinfo.buf, len)) {
        mp_raise_OSError(MP_EIO);
    }
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
        case MP_BLOCKDEV_IOCTL_SYNC:
            // Nothing is buffered, so there is nothing to flush.
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_BLOCK_COUNT:
            return MP_OBJ_NEW_SMALL_INT(FLASH_BDEV_BLOCK_COUNT);
        case MP_BLOCKDEV_IOCTL_BLOCK_SIZE:
            return MP_OBJ_NEW_SMALL_INT(FLASH_BDEV_BLOCK_SIZE);
        case MP_BLOCKDEV_IOCTL_BLOCK_ERASE: {
            uint32_t block = mp_obj_get_int(arg_in);
            if (block >= FLASH_BDEV_BLOCK_COUNT) {
                return MP_OBJ_NEW_SMALL_INT(-1);
            }
            return MP_OBJ_NEW_SMALL_INT(flash_bdev_erase_block(block) ? 0 : -1);
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

// ---- machine.Flash: format helper ----
// Formatting wipes the whole 256 KB partition, so it is deliberately explicit.
static mp_obj_t machine_flash_mkfs(void) {
    mp_obj_t vfs_mod = mp_import_name(MP_QSTR_vfs, mp_const_none, MP_OBJ_NEW_SMALL_INT(0));
    mp_obj_t vfslfs2 = mp_load_attr(vfs_mod, MP_QSTR_VfsLfs2);
    mp_obj_t mkfs = mp_load_attr(vfslfs2, MP_QSTR_mkfs);
    mp_obj_t args[1] = { pyb_flash_get_block_device() };
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
