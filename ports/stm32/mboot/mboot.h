/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020 Damien P. George
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
#ifndef MICROPY_INCLUDED_STM32_MBOOT_MBOOT_H
#define MICROPY_INCLUDED_STM32_MBOOT_MBOOT_H

#include "py/mpconfig.h"
#include "py/mphal.h"

// Use this to tag global static data in RAM that doesn't need to be zeroed on startup
#define SECTION_NOZERO_BSS __attribute__((section(".nozero_bss")))

#define ELEM_DATA_SIZE (1024)
#define ELEM_DATA_START (&_estack[0])
#define ELEM_DATA_MAX (&_estack[ELEM_DATA_SIZE])

#define NORETURN __attribute__((noreturn))
#define MP_ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

// The default UI code in ui.c only works if there is at least one LED configured.
#if defined(MBOOT_LED1) || defined(MICROPY_HW_LED1)
#define MBOOT_ENABLE_DEFAULT_UI (1)
#else
#define MBOOT_ENABLE_DEFAULT_UI (0)
#endif

#ifndef MBOOT_BOARD_EARLY_INIT
#define MBOOT_BOARD_EARLY_INIT(initial_r0)
#endif

#ifndef MBOOT_BOARD_ENTRY_INIT
#define MBOOT_BOARD_ENTRY_INIT(initial_r0) mboot_entry_init_default()
#endif

#ifndef MBOOT_BOARD_GET_RESET_MODE
#if MBOOT_ENABLE_DEFAULT_UI
#define MBOOT_BOARD_GET_RESET_MODE(initial_r0) mboot_get_reset_mode_default()
#else
#define MBOOT_BOARD_GET_RESET_MODE(initial_r0) BOARDCTRL_RESET_MODE_NORMAL
#endif
#endif

#ifndef MBOOT_BOARD_STATE_CHANGE
#if MBOOT_ENABLE_DEFAULT_UI
#define MBOOT_BOARD_STATE_CHANGE(state, arg) mboot_state_change_default((state), (arg))
#else
#define MBOOT_BOARD_STATE_CHANGE(state, arg)
#endif
#endif

#ifndef MBOOT_BOARD_SYSTICK
#if MBOOT_ENABLE_DEFAULT_UI
#define MBOOT_BOARD_SYSTICK() mboot_ui_systick()
#else
#define MBOOT_BOARD_SYSTICK()
#endif
#endif

#ifndef MBOOT_ADDRESS_SPACE_64BIT
#define MBOOT_ADDRESS_SPACE_64BIT (0)
#endif

// These values are used in initial_r0 to enter mboot programmatically.
#define MBOOT_INITIAL_R0_KEY (0x70ad0000)
#define MBOOT_INITIAL_R0_KEY_FSLOAD (MBOOT_INITIAL_R0_KEY | 0x80)

// These are for led_state_all() and can be or'd together.
#define MBOOT_LED_STATE_LED0 (0x01)
#define MBOOT_LED_STATE_LED1 (0x02)
#define MBOOT_LED_STATE_LED2 (0x04)
#define MBOOT_LED_STATE_LED3 (0x08)

// These enum values are passed as the first argument to mboot_state_change() to
// notify of a change in state of the bootloader activity.  This function has a
// default implementation in ui.c but can be overridden by a board.  Some states
// have an argument passed along as the second argument to mboot_state_change().
// If this argument is unused then 0 is passed in.  A result of an operation is
// 0 for success and <0 for failure, with a failure being either an MP_Exxx code
// or MBOOT_ERRNO_xxx code.
typedef enum {
    MBOOT_STATE_DFU_START,          // arg: unused
    MBOOT_STATE_DFU_END,            // arg: unused
    MBOOT_STATE_FSLOAD_START,       // arg: unused
    MBOOT_STATE_FSLOAD_END,         // arg: result of fsload operation
    MBOOT_STATE_FSLOAD_PASS_START,  // arg: pass number, 0 or 1 (verify and write respectively)
    MBOOT_STATE_FSLOAD_PROGRESS,    // arg: total bytes processed so far, high bit set when doing write pass
    MBOOT_STATE_ERASE_START,        // arg: address of erase
    MBOOT_STATE_ERASE_END,          // arg: result of erase
    MBOOT_STATE_READ_START,         // arg: address of read
    MBOOT_STATE_READ_END,           // arg: result of read
    MBOOT_STATE_WRITE_START,        // arg: address of write
    MBOOT_STATE_WRITE_END,          // arg: result of write
} mboot_state_t;

enum {
    MBOOT_ERRNO_FLASH_ERASE_DISALLOWED = 200,
    MBOOT_ERRNO_FLASH_ERASE_FAILED,
    MBOOT_ERRNO_FLASH_READ_DISALLOWED,
    MBOOT_ERRNO_FLASH_WRITE_DISALLOWED,

    MBOOT_ERRNO_DFU_INVALID_HEADER = 210,
    MBOOT_ERRNO_DFU_INVALID_TARGET,
    MBOOT_ERRNO_DFU_INVALID_SIZE,
    MBOOT_ERRNO_DFU_TOO_MANY_TARGETS,
    MBOOT_ERRNO_DFU_READ_ERROR,
    MBOOT_ERRNO_DFU_INVALID_CRC,

    MBOOT_ERRNO_FSLOAD_NO_FSLOAD = 220,
    MBOOT_ERRNO_FSLOAD_NO_MOUNT,
    MBOOT_ERRNO_FSLOAD_INVALID_MOUNT,

    MBOOT_ERRNO_PACK_INVALID_ADDR = 230,
    MBOOT_ERRNO_PACK_INVALID_CHUNK,
    MBOOT_ERRNO_PACK_INVALID_VERSION,
    MBOOT_ERRNO_PACK_DECRYPT_FAILED,
    MBOOT_ERRNO_PACK_SIGN_FAILED,

    MBOOT_ERRNO_VFS_FAT_MOUNT_FAILED = 240,
    MBOOT_ERRNO_VFS_FAT_OPEN_FAILED,
    MBOOT_ERRNO_VFS_LFS1_MOUNT_FAILED,
    MBOOT_ERRNO_VFS_LFS1_OPEN_FAILED,
    MBOOT_ERRNO_VFS_LFS2_MOUNT_FAILED,
    MBOOT_ERRNO_VFS_LFS2_OPEN_FAILED,

    MBOOT_ERRNO_GUNZIP_FAILED = 250,
};

enum {
    ELEM_TYPE_END = 1,
    ELEM_TYPE_MOUNT,
    ELEM_TYPE_FSLOAD,
    ELEM_TYPE_STATUS,
};

enum {
    ELEM_MOUNT_FAT = 1,
    ELEM_MOUNT_LFS1,
    ELEM_MOUNT_LFS2,
};

// Configure the type used to hold an address in the mboot address space.
#if MBOOT_ADDRESS_SPACE_64BIT
typedef uint64_t mboot_addr_t;
#else
typedef uint32_t mboot_addr_t;
#endif

extern volatile uint32_t systick_ms;
extern uint8_t _estack[ELEM_DATA_SIZE];
extern int32_t first_writable_flash_sector;

void systick_init(void);
void led_init(void);
void mboot_ui_systick(void);
void SystemClock_Config(void);

uint32_t get_le32(const uint8_t *b);
uint64_t get_le64(const uint8_t *b);
void led_state_all(unsigned int mask);

int hw_page_erase(uint32_t addr, uint32_t *next_addr);
void hw_read(mboot_addr_t addr, size_t len, uint8_t *buf);
int hw_write(uint32_t addr, const uint8_t *src8, size_t len);

int do_page_erase(uint32_t addr, uint32_t *next_addr);
void do_read(mboot_addr_t addr, size_t len, uint8_t *buf);
int do_write(uint32_t addr, const uint8_t *src8, size_t len, bool dry_run);

const uint8_t *elem_search(const uint8_t *elem, uint8_t elem_id);
int fsload_process(void);

static inline void mboot_entry_init_default(void) {
    #if MBOOT_ENABLE_DEFAULT_UI
    // Init subsystems (mboot_get_reset_mode() may call these, calling them again is ok)
    led_init();
    #endif

    // set the system clock to be HSE
    SystemClock_Config();

    #if defined(STM32H7)
    // Ensure IRQs are enabled (needed coming out of ST bootloader on H7)
    __set_PRIMASK(0);
    #endif
}

int mboot_get_reset_mode_default(void);
void mboot_state_change_default(mboot_state_t state, uint32_t arg);

static inline void mboot_state_change(mboot_state_t state, uint32_t arg) {
    return MBOOT_BOARD_STATE_CHANGE(state, arg);
}

#endif // MICROPY_INCLUDED_STM32_MBOOT_MBOOT_H
