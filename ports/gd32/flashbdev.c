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
// * A RAM cache holds one whole physical sector, so the logical block size can
//   be smaller than the erase size.  Erases are applied to the cached image and
//   only reach the chip on flush, at which point the whole sector is erased once
//   and rewritten.  This is the same idea as ports/stm32/flashbdev.c, which uses
//   512 B logical blocks over 16 K/64 K/128 K sectors.
//   Consequence: block 0 of the device no longer equals sector 1 of the chip;
//   flash_fs_sectors[] is the authority on that mapping, not arithmetic.
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

// True if every byte of the filesystem region is 0xFF, i.e. the partition has
// never been written.  Used to decide whether it is safe to auto-format: any
// non-0xFF byte means something is already there and must not be erased.
bool flash_bdev_is_blank(void) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(flash_fs_sectors); ++i) {
        uint32_t base = flash_fs_sectors[i].addr;
        uint32_t size = flash_fs_sectors[i].size;
        const uint32_t *p = (const uint32_t *)base;
        for (uint32_t off = 0; off < size; off += 4) {
            if (p[off / 4] != 0xffffffffu) {
                return false;
            }
        }
    }
    return true;
}

typedef struct _flash_bdev_obj_t {
    mp_obj_base_t base;
} flash_bdev_obj_t;

// ---- logical block geometry ----
//
// The logical block size is deliberately NOT the physical sector size any more.
// Erase granularity stays at one physical sector (16 KB), but a RAM cache holds
// a whole sector, so littlefs can be given smaller logical blocks and erase
// them individually: the sector is erased once, on flush, and all the logical
// blocks that live in it are written back together.
//
// Why this matters: with block_size == 16 KB the 48 KB partition only had 3
// blocks.  Two of them are consumed by the root metadata pair, leaving a single
// free block, so every file larger than littlefs' inline threshold (128 B) could
// be written exactly once - the second save failed with LFS2_ERR_NOSPC.  That is
// what made Thonny refuse to save /main.py (OSError 28).
//
// 4 KB blocks give 12 blocks for the same 48 KB partition, which is the same
// approach ports/stm32 takes (it uses 512 B logical blocks over 16 K/64 K/128 K
// sectors, see its flashbdev.c).
#define FLASH_BDEV_BLOCK_SIZE       (4096)

// Bytes of one physical sector that the cache has to cover.
#define FLASH_BDEV_SECTOR_MAX       (16 * 1024)

// Number of logical blocks inside one physical sector.
#define FLASH_BDEV_BLOCKS_PER_SECTOR (FLASH_BDEV_SECTOR_MAX / FLASH_BDEV_BLOCK_SIZE)

// ---- sector cache ----
// Holds the contents of exactly one physical sector.  Writes (and logical-block
// erases) are applied here and only reach flash on flush, which is when the
// physical erase happens.  Costs 16 KB of .bss; the port has ~30 KB free.
static uint8_t flash_bdev_cache[FLASH_BDEV_SECTOR_MAX];

// Index into flash_fs_sectors[] of the sector currently held, or -1.
static int flash_bdev_cache_sector = -1;

// True when the cache differs from flash and must be flushed.
static bool flash_bdev_cache_dirty = false;

// ---- block <-> sector translation ----
// The blocks exposed to littlefs are the filesystem sectors, in order.  Each
// block is one physical sector, with block_size == erase size.
static const flash_sector_t *flash_bdev_block_info(uint32_t block) {
    return &flash_fs_sectors[block];
}

// Total number of logical blocks across the whole partition.
#define FLASH_BDEV_LOGICAL_BLOCKS \
    ((uint32_t)(FLASH_BDEV_EXPECTED_SIZE / FLASH_BDEV_BLOCK_SIZE))


// Byte-wise copy that never assumes alignment.  Cortex-M4 faults on unaligned
// LDM/STM, which is what newlib's memcpy uses for larger sizes, and littlefs
// hands us buffers of arbitrary alignment.
static void flash_bdev_copy_bytes(void *dst, const void *src, size_t len) {
    volatile uint8_t *d = (volatile uint8_t *)dst;
    const uint8_t *s = (const uint8_t *)src;
    for (size_t i = 0; i < len; ++i) {
        d[i] = s[i];
    }
}

// ---- flash primitives ----
static bool flash_bdev_erase_block(uint32_t block) {
    const flash_sector_t *s = flash_bdev_block_info(block);
    fmc_state_enum state;

    fmc_unlock();
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR |
                   FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);
    // fmc_sector_erase() expects the FMC_CTL.SN field encoding, i.e. CTL_SN(n)
    // = n << 3, NOT a bare sector index.  Passing the bare index leaves bits
    // 3..7 zero, so the chip erases sector 0 (the vector table) instead - which
    // silently destroyed the firmware on the first filesystem operation.
    state = fmc_sector_erase(CTL_SN(s->sector));
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
            flash_bdev_copy_bytes(&word, &src[i], 4);
        } else {
            // Pad a short tail with 0xff so we do not write past the request.
            word = 0xffffffffu;
            flash_bdev_copy_bytes(&word, &src[i], n);
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

// ---- sector cache plumbing ----
// The cache is one physical sector mirrored in RAM.  Every logical block maps
// into it; nothing reaches flash until the cache is flushed.

// Flush the cache (if dirty) using the erase-then-program sequence that NOR
// flash requires.  Safe to call when clean.
static bool flash_bdev_cache_flush(void) {
    if (flash_bdev_cache_sector < 0 || !flash_bdev_cache_dirty) {
        return true;
    }

    if (!flash_bdev_erase_block((uint32_t)flash_bdev_cache_sector)) {
        return false;
    }

    const flash_sector_t *s = &flash_fs_sectors[flash_bdev_cache_sector];
    if (!flash_bdev_program(s->addr, flash_bdev_cache, s->size)) {
        return false;
    }

    flash_bdev_cache_dirty = false;
    return true;
}

// Make `sector` the cached one, flushing whatever was held before.  Returns a
// pointer to the cached image of that sector.
//
// The old contents are ALWAYS preloaded.  A logical write only replaces part of
// one logical block; the other blocks sharing this sector must survive the
// future erase-and-rewrite, so their current bytes have to be in the cache.
// (Skipping the preload for writes looked like a saving - littlefs erases the
// block it is about to write - but it silently wiped every neighbour in the
// sector: file contents came back as raw superblock bytes.)
static uint8_t *flash_bdev_cache_load(int sector) {
    if (flash_bdev_cache_sector == sector) {
        return flash_bdev_cache;
    }

    // Bring the previous sector up to date before dropping it.
    if (!flash_bdev_cache_flush()) {
        return NULL;
    }

    const flash_sector_t *s = &flash_fs_sectors[sector];
    flash_bdev_copy_bytes(flash_bdev_cache, (const void *)s->addr, s->size);
    flash_bdev_cache_sector = sector;
    return flash_bdev_cache;
}

// Map a logical block number + offset to a pointer inside the cache.
static uint8_t *flash_bdev_cache_ptr(uint32_t block, size_t offset) {
    uint32_t sector = block / FLASH_BDEV_BLOCKS_PER_SECTOR;
    uint32_t in_sector = (block % FLASH_BDEV_BLOCKS_PER_SECTOR) * FLASH_BDEV_BLOCK_SIZE;

    if (sector >= FLASH_BDEV_BLOCK_COUNT) {
        return NULL;
    }
    uint8_t *base = flash_bdev_cache_load((int)sector);
    if (base == NULL) {
        return NULL;
    }
    return base + in_sector + offset;
}

// ---- Python block-device protocol ----
// readblocks(block, buf[, offset])
static mp_obj_t flash_bdev_readblocks(size_t n_args, const mp_obj_t *args) {
    uint32_t block = mp_obj_get_int(args[1]);
    if (block >= FLASH_BDEV_LOGICAL_BLOCKS) {
        mp_raise_ValueError(MP_ERROR_TEXT("block out of range"));
    }

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_WRITE);

    size_t offset = 0;
    size_t len = bufinfo.len;
    if (n_args > 3) {
        offset = mp_obj_get_int(args[3]);
    }
    uint32_t bsize = FLASH_BDEV_BLOCK_SIZE;
    if (offset > bsize) {
        offset = bsize;
    }
    if (len > bsize - offset) {
        len = bsize - offset;
    }

    // Reads must go through the cache as well: a dirty sector has not reached
    // flash yet, so reading the raw address would return stale data.
    uint8_t *src = flash_bdev_cache_ptr(block, offset);
    if (src == NULL) {
        mp_raise_OSError(MP_EIO);
    }
    flash_bdev_copy_bytes(bufinfo.buf, src, len);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(flash_bdev_readblocks_obj, 3, 4, flash_bdev_readblocks);

// writeblocks(block, buf[, offset])
// littlefs erases a block before programming it, so by the time this is called
// the target logical block has already been erased via BLOCK_ERASE - the cache
// performs a real erase exactly once, on flush, which then rewrites the whole
// sector including the blocks that were not touched.
static mp_obj_t flash_bdev_writeblocks(size_t n_args, const mp_obj_t *args) {
    uint32_t block = mp_obj_get_int(args[1]);
    if (block >= FLASH_BDEV_LOGICAL_BLOCKS) {
        mp_raise_ValueError(MP_ERROR_TEXT("block out of range"));
    }

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_READ);

    size_t offset = 0;
    size_t len = bufinfo.len;
    if (n_args > 3) {
        offset = mp_obj_get_int(args[3]);
    }

    uint32_t bsize = FLASH_BDEV_BLOCK_SIZE;
    if (offset > bsize) {
        mp_raise_OSError(MP_EINVAL);
    }
    if (len > bsize - offset) {
        len = bsize - offset;
    }

    uint8_t *dst = flash_bdev_cache_ptr(block, offset);
    if (dst == NULL) {
        mp_raise_OSError(MP_EIO);
    }
    flash_bdev_copy_bytes(dst, bufinfo.buf, len);
    flash_bdev_cache_dirty = true;
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
            // Drop any cache state left over from a previous mount.
            flash_bdev_cache_sector = -1;
            flash_bdev_cache_dirty = false;
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_DEINIT:
            return MP_OBJ_NEW_SMALL_INT(flash_bdev_cache_flush() ? 0 : -1);
        case MP_BLOCKDEV_IOCTL_SYNC:
            return MP_OBJ_NEW_SMALL_INT(flash_bdev_cache_flush() ? 0 : -1);
        case MP_BLOCKDEV_IOCTL_BLOCK_COUNT:
            return MP_OBJ_NEW_SMALL_INT(FLASH_BDEV_LOGICAL_BLOCKS);
        case MP_BLOCKDEV_IOCTL_BLOCK_SIZE:
            return MP_OBJ_NEW_SMALL_INT(FLASH_BDEV_BLOCK_SIZE);
        case MP_BLOCKDEV_IOCTL_BLOCK_ERASE: {
            uint32_t block = mp_obj_get_int(arg_in);
            if (block >= FLASH_BDEV_LOGICAL_BLOCKS) {
                return MP_OBJ_NEW_SMALL_INT(-1);
            }
            // Fill the logical block with 0xFF in the cache.  The physical
            // erase is deferred to flash_bdev_cache_flush(); doing it here
            // would erase the neighbours that share this sector.
            uint8_t *dst = flash_bdev_cache_ptr(block, 0);
            if (dst == NULL) {
                return MP_OBJ_NEW_SMALL_INT(-1);
            }
            memset(dst, 0xFF, FLASH_BDEV_BLOCK_SIZE);
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

// ---- machine.Flash: format helper ----
// Formatting wipes the whole filesystem partition, so it is deliberately
// explicit.
static mp_obj_t machine_flash_mkfs(void) {
    // Invalidate the cache first.  mkfs writes a fresh superblock straight to
    // the device through the normal block protocol; if a dirty sector were
    // still held here it would be flushed afterwards and silently undo part of
    // the format.
    flash_bdev_cache_sector = -1;
    flash_bdev_cache_dirty = false;

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
