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

#include <string.h>

#include "py/mphal.h"
#include "py/runtime.h"
#include "py/mperrno.h"
#include "extmod/vfs.h"
#include "modrp2.h"
#include "hardware/flash.h"
#include "pico/binary_info.h"
#include "rp2_psram.h"
#ifdef PICO_RP2350
#include "hardware/structs/ioqspi.h"
#include "hardware/structs/qmi.h"
#else
#include "hardware/structs/ssi.h"
#endif

#define BLOCK_SIZE_BYTES (FLASH_SECTOR_SIZE)

// Size of buffer for flash writes from PSRAM, since they are mutually exclusive
#define COPY_BUFFER_SIZE_BYTES (FLASH_PAGE_SIZE)

static_assert(MICROPY_HW_ROMFS_BYTES % 4096 == 0, "ROMFS size must be a multiple of 4K");
static_assert(MICROPY_HW_FLASH_STORAGE_BYTES % 4096 == 0, "Flash storage size must be a multiple of 4K");

#ifndef MICROPY_HW_FLASH_MAX_FREQ
// Emulate Pico SDK's SYS_CLK_HZ / PICO_FLASH_SPI_CLKDIV behaviour by default.
// On RP2040 if PICO_USE_FASTEST_SUPPORTED_CLOCK is set then SYS_CLK_HZ can be
// 200MHz, potentially putting timings derived from PICO_FLASH_SPI_CLKDIV
// out of range.
#ifdef PICO_FLASH_SPI_CLKDIV
#define MICROPY_HW_FLASH_MAX_FREQ (SYS_CLK_HZ / PICO_FLASH_SPI_CLKDIV)
#else
// A default PICO_FLASH_SPI_CLKDIV of 4 is set in boot2_generic_03h.S
#define MICROPY_HW_FLASH_MAX_FREQ (SYS_CLK_HZ / 4)
#endif
#endif

#ifndef MICROPY_HW_FLASH_STORAGE_BASE
#define MICROPY_HW_FLASH_STORAGE_BASE (PICO_FLASH_SIZE_BYTES - MICROPY_HW_FLASH_STORAGE_BYTES)
#endif

// Put ROMFS at the upper end of the code space.
#define MICROPY_HW_ROMFS_BASE (MICROPY_HW_FLASH_STORAGE_BASE - MICROPY_HW_ROMFS_BYTES)

static_assert(MICROPY_HW_FLASH_STORAGE_BYTES <= PICO_FLASH_SIZE_BYTES, "MICROPY_HW_FLASH_STORAGE_BYTES too big");
static_assert(MICROPY_HW_FLASH_STORAGE_BASE + MICROPY_HW_FLASH_STORAGE_BYTES <= PICO_FLASH_SIZE_BYTES, "MICROPY_HW_FLASH_STORAGE_BYTES too big");

typedef struct _rp2_flash_obj_t {
    mp_obj_base_t base;
    uint32_t flash_base;
    uint32_t flash_size;
} rp2_flash_obj_t;

#if MICROPY_HW_ROMFS_BYTES > 0
static rp2_flash_obj_t rp2_flash_romfs_obj = {
    .base = { &rp2_flash_type },
    .flash_base = MICROPY_HW_ROMFS_BASE,
    .flash_size = MICROPY_HW_ROMFS_BYTES,
};
#endif

static rp2_flash_obj_t rp2_flash_obj = {
    .base = { &rp2_flash_type },
    .flash_base = MICROPY_HW_FLASH_STORAGE_BASE,
    .flash_size = MICROPY_HW_FLASH_STORAGE_BYTES,
};

// Tag the flash drive in the binary as readable/writable (but not reformatable)
bi_decl(bi_block_device(
    BINARY_INFO_TAG_MICROPYTHON,
    "MicroPython",
    XIP_BASE + MICROPY_HW_FLASH_STORAGE_BASE,
    MICROPY_HW_FLASH_STORAGE_BYTES,
    NULL,
    BINARY_INFO_BLOCK_DEV_FLAG_READ |
    BINARY_INFO_BLOCK_DEV_FLAG_WRITE |
    BINARY_INFO_BLOCK_DEV_FLAG_PT_UNKNOWN));

#if MICROPY_HW_ROMFS_BYTES
// Tag the ROMFS partition in the binary
bi_decl(bi_block_device(
    BINARY_INFO_TAG_MICROPYTHON,
    "ROMFS",
    XIP_BASE + MICROPY_HW_ROMFS_BASE,
    MICROPY_HW_ROMFS_BYTES,
    NULL,
    BINARY_INFO_BLOCK_DEV_FLAG_READ |
    BINARY_INFO_BLOCK_DEV_FLAG_PT_UNKNOWN));
#endif

// This is a workaround to pico-sdk #2201: https://github.com/raspberrypi/pico-sdk/issues/2201
// which means the multicore_lockout_victim_is_initialized returns true even after core1 is reset.
static bool use_multicore_lockout(void) {
    return multicore_lockout_victim_is_initialized(1 - get_core_num())
           #if MICROPY_PY_THREAD
           && core1_entry != NULL
           #endif
    ;
}

// Function to set the flash divisor to the correct divisor, assumes interrupts disabled
// and core1 locked out if relevant.
static void __no_inline_not_in_flash_func(rp2_flash_set_timing_internal)(int clock_hz) {

    // Use the minimum divisor based upon our target MICROPY_HW_FLASH_MAX_FREQ
    int divisor = (clock_hz + MICROPY_HW_FLASH_MAX_FREQ - 1) / MICROPY_HW_FLASH_MAX_FREQ;

    #if PICO_RP2350
    // Make sure flash is deselected - QMI doesn't appear to have a busy flag(!)
    while ((ioqspi_hw->io[1].status & IO_QSPI_GPIO_QSPI_SS_STATUS_OUTTOPAD_BITS) != IO_QSPI_GPIO_QSPI_SS_STATUS_OUTTOPAD_BITS) {
        ;
    }

    // RX delay equal to the divisor means sampling at the same time as the next falling edge of SCK after the
    // falling edge that generated the data.  This is pretty tight at 133MHz but seems to work with the Winbond flash chips.
    const int rxdelay = divisor;
    qmi_hw->m[0].timing = (1 << QMI_M0_TIMING_COOLDOWN_LSB) |
        rxdelay << QMI_M1_TIMING_RXDELAY_LSB |
        divisor << QMI_M1_TIMING_CLKDIV_LSB;

    // Force a read through XIP to ensure the timing is applied
    volatile uint32_t *ptr = (volatile uint32_t *)0x14000000;
    (void)*ptr;
    #else
    // RP2040 SSI hardware only supports even divisors
    if (divisor & 1) {
        divisor += 1;
    }

    // Wait for SSI not busy
    while (ssi_hw->sr & SSI_SR_BUSY_BITS) {
        ;
    }

    // Disable, set the new divisor, and re-enable
    hw_clear_bits(&ssi_hw->ssienr, SSI_SSIENR_SSI_EN_BITS);
    ssi_hw->baudr = divisor;
    hw_set_bits(&ssi_hw->ssienr, SSI_SSIENR_SSI_EN_BITS);
    #endif
}

// Flash erase and write must run with interrupts disabled and the other core suspended,
// because the XIP bit gets disabled.
static uint32_t begin_critical_flash_section(void) {
    if (use_multicore_lockout()) {
        multicore_lockout_start_blocking();
    }
    uint32_t state = save_and_disable_interrupts();

    #if MICROPY_HW_ENABLE_PSRAM
    // We're about to invalidate the XIP cache, clean it first to commit any dirty writes to PSRAM
    // Use the upper 16k of the maintenance space (0x1bffc000 through 0x1bffffff) to workaround
    // incorrect behaviour of the XIP clean operation, where it also alters the tag of the associated
    // cache line: https://forums.raspberrypi.com/viewtopic.php?t=378249#p2263677
    volatile uint8_t *maintenance_ptr = (volatile uint8_t *)(XIP_SRAM_BASE + (XIP_MAINTENANCE_BASE - XIP_BASE));
    for (int i = 1; i < 16 * 1024; i += 8) {
        maintenance_ptr[i] = 0;
    }
    #endif

    return state;
}

static void end_critical_flash_section(uint32_t state) {
    // The ROM function to program flash will have reset flash and PSRAM timings to defaults
    rp2_flash_set_timing_internal(clock_get_hz(clk_sys));
    #if MICROPY_HW_ENABLE_PSRAM
    psram_init(MICROPY_HW_PSRAM_CS_PIN);
    #endif
    restore_interrupts(state);
    if (use_multicore_lockout()) {
        multicore_lockout_end_blocking();
    }
}

static mp_obj_t rp2_flash_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // Parse arguments
    enum { ARG_start, ARG_len };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_start, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_len,   MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_start].u_int == -1 && args[ARG_len].u_int == -1) {
        #ifndef NDEBUG
        extern char __flash_binary_end;
        assert((uintptr_t)&__flash_binary_end - XIP_BASE <= MICROPY_HW_FLASH_STORAGE_BASE);
        #endif

        // Default singleton object that accesses entire flash
        return MP_OBJ_FROM_PTR(&rp2_flash_obj);
    }

    rp2_flash_obj_t *self = mp_obj_malloc(rp2_flash_obj_t, &rp2_flash_type);

    mp_int_t start = args[ARG_start].u_int;
    if (start == -1) {
        start = 0;
    } else if (!(0 <= start && start < MICROPY_HW_FLASH_STORAGE_BYTES && start % BLOCK_SIZE_BYTES == 0)) {
        mp_raise_ValueError(NULL);
    }

    mp_int_t len = args[ARG_len].u_int;
    if (len == -1) {
        len = MICROPY_HW_FLASH_STORAGE_BYTES - start;
    } else if (!(0 < len && start + len <= MICROPY_HW_FLASH_STORAGE_BYTES && len % BLOCK_SIZE_BYTES == 0)) {
        mp_raise_ValueError(NULL);
    }

    self->flash_base = MICROPY_HW_FLASH_STORAGE_BASE + start;
    self->flash_size = len;

    return MP_OBJ_FROM_PTR(self);
}

static mp_int_t rp2_flash_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    rp2_flash_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (flags == MP_BUFFER_READ) {
        bufinfo->buf = (void *)(XIP_BASE + self->flash_base);
        bufinfo->len = self->flash_size;
        bufinfo->typecode = 'B';
        return 0;
    } else {
        // Write unsupported.
        return 1;
    }
}

static mp_obj_t rp2_flash_readblocks(size_t n_args, const mp_obj_t *args) {
    rp2_flash_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t offset = mp_obj_get_int(args[1]) * BLOCK_SIZE_BYTES;
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_WRITE);
    if (n_args == 4) {
        offset += mp_obj_get_int(args[3]);
    }
    memcpy(bufinfo.buf, (void *)(XIP_BASE + self->flash_base + offset), bufinfo.len);
    // mp_event_handle_nowait() is called here to avoid a fail in registering
    // USB at boot time, if the board is busy loading files or scanning the file
    // system. mp_event_handle_nowait() will call the TinyUSB task if needed.
    mp_event_handle_nowait();
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(rp2_flash_readblocks_obj, 3, 4, rp2_flash_readblocks);

static mp_obj_t rp2_flash_writeblocks(size_t n_args, const mp_obj_t *args) {
    rp2_flash_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t offset = mp_obj_get_int(args[1]) * BLOCK_SIZE_BYTES;
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_READ);
    if (n_args == 3) {
        mp_uint_t atomic_state = begin_critical_flash_section();
        flash_range_erase(self->flash_base + offset, bufinfo.len);
        end_critical_flash_section(atomic_state);
        mp_event_handle_nowait();
        // TODO check return value
    } else {
        offset += mp_obj_get_int(args[3]);
    }

    // If copying from SRAM, can write direct to flash.
    // If copying from PSRAM/flash, use an SRAM buffer and write in chunks.
    #if MICROPY_HW_ENABLE_PSRAM
    bool write_direct = (uintptr_t)bufinfo.buf >= SRAM_BASE;
    #else
    bool write_direct = true;
    #endif

    if (write_direct) {
        // If copying from SRAM, write direct
        mp_uint_t atomic_state = begin_critical_flash_section();
        flash_range_program(self->flash_base + offset, bufinfo.buf, bufinfo.len);
        end_critical_flash_section(atomic_state);
        mp_event_handle_nowait();
    }
    #if MICROPY_HW_ENABLE_PSRAM
    else {
        size_t bytes_left = bufinfo.len;
        size_t bytes_offset = 0;
        static uint8_t copy_buffer[COPY_BUFFER_SIZE_BYTES] = {0};

        while (bytes_left) {
            memcpy(copy_buffer, bufinfo.buf + bytes_offset, MIN(bytes_left, COPY_BUFFER_SIZE_BYTES));
            mp_uint_t atomic_state = begin_critical_flash_section();
            flash_range_program(self->flash_base + offset + bytes_offset, copy_buffer, MIN(bytes_left, COPY_BUFFER_SIZE_BYTES));
            end_critical_flash_section(atomic_state);
            bytes_offset += COPY_BUFFER_SIZE_BYTES;
            if (bytes_left <= COPY_BUFFER_SIZE_BYTES) {
                break;
            }
            bytes_left -= COPY_BUFFER_SIZE_BYTES;
            mp_event_handle_nowait();
        }
    }
    #endif

    // TODO check return value
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(rp2_flash_writeblocks_obj, 3, 4, rp2_flash_writeblocks);

static mp_obj_t rp2_flash_ioctl(mp_obj_t self_in, mp_obj_t cmd_in, mp_obj_t arg_in) {
    rp2_flash_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t cmd = mp_obj_get_int(cmd_in);
    switch (cmd) {
        case MP_BLOCKDEV_IOCTL_INIT:
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_DEINIT:
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_SYNC:
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_BLOCK_COUNT:
            return MP_OBJ_NEW_SMALL_INT(self->flash_size / BLOCK_SIZE_BYTES);
        case MP_BLOCKDEV_IOCTL_BLOCK_SIZE:
            return MP_OBJ_NEW_SMALL_INT(BLOCK_SIZE_BYTES);
        case MP_BLOCKDEV_IOCTL_BLOCK_ERASE: {
            uint32_t offset = mp_obj_get_int(arg_in) * BLOCK_SIZE_BYTES;
            mp_uint_t atomic_state = begin_critical_flash_section();
            flash_range_erase(self->flash_base + offset, BLOCK_SIZE_BYTES);
            end_critical_flash_section(atomic_state);
            // TODO check return value
            return MP_OBJ_NEW_SMALL_INT(0);
        }
        default:
            return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_3(rp2_flash_ioctl_obj, rp2_flash_ioctl);

static const mp_rom_map_elem_t rp2_flash_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&rp2_flash_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&rp2_flash_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&rp2_flash_ioctl_obj) },
};
static MP_DEFINE_CONST_DICT(rp2_flash_locals_dict, rp2_flash_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    rp2_flash_type,
    MP_QSTR_Flash,
    MP_TYPE_FLAG_NONE,
    make_new, rp2_flash_make_new,
    buffer, rp2_flash_get_buffer,
    locals_dict, &rp2_flash_locals_dict
    );

#if MICROPY_VFS_ROM_IOCTL
mp_obj_t mp_vfs_rom_ioctl(size_t n_args, const mp_obj_t *args) {
    switch (mp_obj_get_int(args[0])) {
        #if MICROPY_HW_ROMFS_BYTES > 0
        case MP_VFS_ROM_IOCTL_GET_NUMBER_OF_SEGMENTS:
            return MP_OBJ_NEW_SMALL_INT(1);
        case MP_VFS_ROM_IOCTL_GET_SEGMENT:
            return MP_OBJ_FROM_PTR(&rp2_flash_romfs_obj);
        #endif
        default:
            return MP_OBJ_NEW_SMALL_INT(-MP_EINVAL);
    }
}
#endif

// Modify the flash timing.  Ensure flash access is suspended while
// the timings are altered.
void rp2_flash_set_timing_for_freq(int clock_hz) {
    if (multicore_lockout_victim_is_initialized(1 - get_core_num())) {
        multicore_lockout_start_blocking();
    }
    uint32_t state = save_and_disable_interrupts();

    rp2_flash_set_timing_internal(clock_hz);

    restore_interrupts(state);
    if (multicore_lockout_victim_is_initialized(1 - get_core_num())) {
        multicore_lockout_end_blocking();
    }
}

void rp2_flash_set_timing(void) {
    rp2_flash_set_timing_for_freq(clock_get_hz(clk_sys));
}
