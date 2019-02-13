/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Analog Devices, Inc.
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
#define asm __asm

#include <sys/adi_core.h>
#include <sys/platform.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/unistd.h>

#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/stackctrl.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "lib/mp-readline/readline.h"
#include "lib/utils/pyexec.h"
#include "lib/oofatfs/ff.h"
#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"

#include <services/rtc/adi_rtc.h>
#include "adi_initialize.h"
#include "bm_uart.h"
#include "pin.h"
#include "modmachine.h"
#include "systick.h"
#include "spi.h"
#include "i2c.h"
#include "sdcard.h"

BM_UART uart0;

#define HEAP_SIZE 128*1024*1024

STATIC bool init_sdcard_fs(void);

int main(int argc, char **argv) {

    // Capture the stack top ASAP
    mp_stack_ctrl_init();
    mp_stack_set_limit(32*1024);

    adi_initComponents();

    uart_initialize(&uart0, UART_BAUD_RATE_115200, UART_SERIAL_8N1, UART0);
    uart_write_byte(&uart0, 0x0C); // Clear the screen

    adi_rtc_Init();

#if MICROPY_ENABLE_GC
    char *heap = malloc(HEAP_SIZE);
    gc_init(heap, heap + HEAP_SIZE);
#endif

    mp_init();
    machine_init();
    readline_init0();
    pin_init0();
    sys_tick_init();
    spi_init0();
    i2c_init0();
    sdcard_init();

    #if MICROPY_HW_SDCARD_MOUNT_AT_BOOT
    // if an SD card is present then mount it on /sd/
    if (sdcard_is_present()) {
        init_sdcard_fs();
    }
    #endif

    pyexec_friendly_repl();

    machine_deinit();
    mp_deinit();

    while(1);

    return 0; // never reaches here
}

#if MICROPY_HW_SDCARD_MOUNT_AT_BOOT
STATIC bool init_sdcard_fs(void) {
    bool first_part = true;
    for (int part_num = 1; part_num <= 4; ++part_num) {
        // create vfs object
        fs_user_mount_t *vfs_fat = m_new_obj_maybe(fs_user_mount_t);
        mp_vfs_mount_t *vfs = m_new_obj_maybe(mp_vfs_mount_t);
        if (vfs == NULL || vfs_fat == NULL) {
            break;
        }
        vfs_fat->flags = FSUSER_FREE_OBJ;
        sdcard_init_vfs(vfs_fat, part_num);

        // try to mount the partition
        FRESULT res = f_mount(&vfs_fat->fatfs);

        if (res != FR_OK) {
            // couldn't mount
            m_del_obj(fs_user_mount_t, vfs_fat);
            m_del_obj(mp_vfs_mount_t, vfs);
        } else {
            // mounted via FatFs, now mount the SD partition in the VFS
            if (first_part) {
                // the first available partition is traditionally called "sd" for simplicity
                vfs->str = "/sd";
                vfs->len = 3;
            } else {
                // subsequent partitions are numbered by their index in the partition table
                if (part_num == 2) {
                    vfs->str = "/sd2";
                } else if (part_num == 2) {
                    vfs->str = "/sd3";
                } else {
                    vfs->str = "/sd4";
                }
                vfs->len = 4;
            }
            vfs->obj = MP_OBJ_FROM_PTR(vfs_fat);
            vfs->next = NULL;
            for (mp_vfs_mount_t **m = &MP_STATE_VM(vfs_mount_table);; m = &(*m)->next) {
                if (*m == NULL) {
                    *m = vfs;
                    break;
                }
            }

            if (first_part) {
                // use SD card as current directory
                MP_STATE_PORT(vfs_cur) = vfs;
            }

            first_part = false;
        }
    }

    if (first_part) {
        mp_printf(&mp_plat_print, "sc5xx: can't mount SD card\n");
        return false;
    } else {
        return true;
    }
}
#endif

void nlr_jump_fail(void *val) {
    uart_write_block(&uart0, (uint8_t *)"\nFatal: nlr jump fail\n", 22);
    while(1);
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    while(1);
}
#endif

