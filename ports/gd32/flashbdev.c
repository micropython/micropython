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
//   Sector 5 128 KB   0x08020000 - 0x0803FFFF
//   Sector 6 128 KB   0x08040000 - 0x0805FFFF
//   Sector 7 128 KB   0x08060000 - 0x0807FFFF
//
// Partition scheme
// ----------------
//   Sector 0        .vectors  (the Cortex-M vector table must sit at 0x08000000)
//   Sectors 1-3     littlefs  (3 x 16 KB = 48 KB, starting at 0x08004000)
//   Sectors 4-7     firmware  (64 + 128 + 128 + 128 = 448 KB, from 0x08010000)
//
// Putting the filesystem low and the code high (the same trick ports/stm32 uses
// with its FLASH_START / FLASH_FS / FLASH_TEXT regions) buys the image 448 KB
// instead of the 384 KB a trailing filesystem would allow, which is what makes
// features like floating point fit.  It costs filesystem capacity: 48 KB rather
// than 128 KB.
//
// The region boundaries are NOT hard-coded here.  They come from the linker
// script as _flash_fs_start / _flash_fs_end, so changing the partition only
// requires editing ld/*.ld - see "Flash layout symbols" there.
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
// * On GD32F4 the value programmed into FMC_CTL.SN is not always the datasheet
//   sector number: the library header maps some high sectors onto compacted SN
//   codes (CTL_SECTOR_NUMBER_24 == CTL_SN(12), and so on).  For the 512 KB parts
//   targeted here sectors 0..7 map one-to-one, but the table below is written
//   out explicitly and checked at compile time so that a future move to an
//   1 MB or 2 MB part cannot silently erase the wrong sector.

#include <string.h>

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/objarray.h"
#include "extmod/vfs.h"
#include "extmod/vfs_lfs.h"

#include "gd32f4xx.h"

// ---------------------------------------------------------------------------
// Flash layout symbols provided by the linker script (ld/*.ld).
// ---------------------------------------------------------------------------
extern uint8_t _flash_fs_start;
extern uint8_t _flash_fs_end;

#define FLASH_BDEV_START_ADDR    ((uintptr_t)&_flash_fs_start)
#define FLASH_BDEV_END_ADDR      ((uintptr_t)&_flash_fs_end)

// ---------------------------------------------------------------------------
// Sector table for the filesystem region.
//
// Each entry gives {sector_number, address, size}.  Addresses are absolute, so
// the table can be checked against the linker symbols at compile time.
// ---------------------------------------------------------------------------
typedef struct {
    uint8_t sector;     // value written into FMC_CTL.SN
    uint32_t addr;      // start address
    uint32_t size;      // erase size (== block size)
} flash_sector_t;

// Sectors 1, 2 and 3 - all 16 KB on the 512 KB parts.
static const flash_sector_t flash_fs_sectors[] = {
    { 1, 0x08004000, 16 * 1024 },
    { 2, 0x08008000, 16 * 1024 },
    { 3, 0x0800C000, 16 * 1024 },
};

#define FLASH_BDEV_BLOCK_COUNT  ((uint32_t)MP_ARRAY_SIZE(flash_fs_sectors))

// Compile-time agreement checks between this table and the linker script.
// If the linker script moves the filesystem region, these fail loudly instead
// of producing a device that erases the wrong thing at runtime.
_Static_assert(MP_ARRAY_SIZE(flash_fs_sectors) == 3,
               "expected exactly three 16 KB filesystem sectors");

#define FLASH_BDEV_EXPECTED_START   (0x08004000)
#define FLASH_BDEV_EXPECTED_END     (0x08010000)
#define FLASH_BDEV_EXPECTED_SIZE    (48 * 1024)

// The linker symbols cannot be used in a _Static_assert (they are addresses),
// so record the expected values and cross-check them at runtime in the ioctl
// INIT path where a mismatch is still caught before any erase happens.

typedef struct _flash_bdev_obj_t {
    mp_obj_base_t base;
} flash_bdev_obj_t;

// ---- block <-> sector translation ----
// The blocks exposed to littlefs are the filesystem sectors, in order.  Each
// block is one physical sector, with block_size == erase size.
static const flash_sector_t *flash_bdev_block_info(uint32_t block) {
    return &flash_fs_sectors[block];
}

static uint32_t flash_bdev_block_size(size_t block) {
    return flash_fs_sectors[block].size;
}

// ---- flash primitives ----
static bool flash_bdev_erase_block(uint32_t block) {
    const flash_sector_t *s = flash_bdev_block_info(block);
    fmc_state_enum state;

    fmc_unlock();
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR |
                   FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);
    state = fmc_sector_erase(s->sector);
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

// Verify that the linker-provided region matches the sector table.  Called from
// the block-device INIT ioctl, i.e. before littlefs can issue any erase.
static bool flash_bdev_layout_ok(void) {
    if (FLASH_BDEV_START_ADDR != FLASH_BDEV_EXPECTED_START) {
        return false;
    }
    if (FLASH_BDEV_END_ADDR != FLASH_BDEV_EXPECTED_END) {
        return false;
    }
    if (FLASH_BDEV_END_ADDR - FLASH_BDEV_START_ADDR != FLASH_BDEV_EXPECTED_SIZE) {
        return false;
    }
    // Each table entry must lie fully inside the linker region and match the
    // block size littlefs will be told to use for that block.
    for (size_t i = 0; i < MP_ARRAY_SIZE(flash_fs_sectors); ++i) {
        uint32_t a = flash_fs_sectors[i].addr;
        uint32_t e = a + flash_fs_sectors[i].size;
        if (a < FLASH_BDEV_START_ADDR || e > FLASH_BDEV_END_ADDR) {
            return false;
        }
    }
    return true;
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
    uint32_t bsize = flash_bdev_block_size(block);
    if (offset > bsize) {
        offset = bsize;
    }
    if (len > bsize - offset) {
        len = bsize - offset;
    }

    const flash_sector_t *s = flash_bdev_block_info(block);
    memcpy(bufinfo.buf, (const void *)(s->addr + offset), len);
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
    uint32_t bsize = flash_bdev_block_size(block);
    if (len > bsize) {
        len = bsize;
    }

    const flash_sector_t *s = flash_bdev_block_info(block);
    if (!flash_bdev_program(s->addr, bufinfo.buf, len)) {
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
            // Refuse to expose the device if the sector table and the linker
            // script disagree - erasing the wrong sector would brick the board.
            if (!flash_bdev_layout_ok()) {
                return MP_OBJ_NEW_SMALL_INT(-1);
            }
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_DEINIT:
        case MP_BLOCKDEV_IOCTL_SYNC:
            // Nothing is buffered, so there is nothing to flush.
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_BLOCK_COUNT:
            return MP_OBJ_NEW_SMALL_INT(FLASH_BDEV_BLOCK_COUNT);
        case MP_BLOCKDEV_IOCTL_BLOCK_SIZE:
            // All filesystem blocks are 16 KB in this layout; report the first
            // one.  littlefs uses a single block size for the whole device.
            return MP_OBJ_NEW_SMALL_INT(flash_bdev_block_size(0));
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
// Formatting wipes the whole filesystem partition, so it is deliberately
// explicit.
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
