/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2016-2017 Linaro Limited
 * Copyright (c) 2020 NXP
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
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <zephyr.h>
#ifdef CONFIG_NETWORKING
#include <net/net_context.h>
#endif

#ifdef CONFIG_USB_DEVICE_STACK
#include <usb/usb_device.h>
#endif

#include <storage/flash_map.h>

#include "py/mperrno.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/mphal.h"
#include "py/stackctrl.h"
#include "shared/runtime/pyexec.h"
#include "shared/readline/readline.h"
#include "extmod/modbluetooth.h"

#if MICROPY_VFS
#include "extmod/vfs.h"
#endif

#include "modmachine.h"
#include "modzephyr.h"

#ifdef TEST
#include "shared/upytesthelper/upytesthelper.h"
#include "lib/tinytest/tinytest.c"
#include "shared/upytesthelper/upytesthelper.c"
#include TEST
#endif

static char heap[MICROPY_HEAP_SIZE];

void init_zephyr(void) {
    // We now rely on CONFIG_NET_APP_SETTINGS to set up bootstrap
    // network addresses.
    #if 0
    #ifdef CONFIG_NETWORKING
    if (net_if_get_default() == NULL) {
        // If there's no default networking interface,
        // there's nothing to configure.
        return;
    }
    #endif
    #ifdef CONFIG_NET_IPV4
    static struct in_addr in4addr_my = {{{192, 0, 2, 1}}};
    net_if_ipv4_addr_add(net_if_get_default(), &in4addr_my, NET_ADDR_MANUAL, 0);
    static struct in_addr in4netmask_my = {{{255, 255, 255, 0}}};
    net_if_ipv4_set_netmask(net_if_get_default(), &in4netmask_my);
    static struct in_addr in4gw_my = {{{192, 0, 2, 2}}};
    net_if_ipv4_set_gw(net_if_get_default(), &in4gw_my);
    #endif
    #ifdef CONFIG_NET_IPV6
    // 2001:db8::1
    static struct in6_addr in6addr_my = {{{0x20, 0x01, 0x0d, 0xb8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}}};
    net_if_ipv6_addr_add(net_if_get_default(), &in6addr_my, NET_ADDR_MANUAL, 0);
    #endif
    #endif
}

#if MICROPY_VFS
STATIC void vfs_init(void) {
    mp_obj_t bdev = NULL;
    mp_obj_t mount_point;
    const char *mount_point_str = NULL;
    int ret = 0;

    #ifdef CONFIG_DISK_DRIVER_SDMMC
    mp_obj_t args[] = { mp_obj_new_str(CONFIG_SDMMC_VOLUME_NAME, strlen(CONFIG_SDMMC_VOLUME_NAME)) };
    bdev = zephyr_disk_access_type.make_new(&zephyr_disk_access_type, ARRAY_SIZE(args), 0, args);
    mount_point_str = "/sd";
    #elif defined(CONFIG_FLASH_MAP) && FLASH_AREA_LABEL_EXISTS(storage)
    mp_obj_t args[] = { MP_OBJ_NEW_SMALL_INT(FLASH_AREA_ID(storage)), MP_OBJ_NEW_SMALL_INT(4096) };
    bdev = zephyr_flash_area_type.make_new(&zephyr_flash_area_type, ARRAY_SIZE(args), 0, args);
    mount_point_str = "/flash";
    #endif

    if ((bdev != NULL)) {
        mount_point = mp_obj_new_str(mount_point_str, strlen(mount_point_str));
        ret = mp_vfs_mount_and_chdir_protected(bdev, mount_point);
        // TODO: if this failed, make a new file system and try to mount again
    }
}
#endif // MICROPY_VFS

int real_main(void) {
    mp_stack_ctrl_init();
    // Make MicroPython's stack limit somewhat smaller than full stack available
    mp_stack_set_limit(CONFIG_MAIN_STACK_SIZE - 512);

    init_zephyr();
    mp_hal_init();

    #ifdef TEST
    static const char *argv[] = {"test"};
    upytest_set_heap(heap, heap + sizeof(heap));
    int r = tinytest_main(1, argv, groups);
    printf("status: %d\n", r);
    #endif

soft_reset:
    #if MICROPY_ENABLE_GC
    gc_init(heap, heap + sizeof(heap));
    #endif
    mp_init();

    #ifdef CONFIG_USB_DEVICE_STACK
    usb_enable(NULL);
    #endif

    #if MICROPY_VFS
    vfs_init();
    #endif

    #if MICROPY_MODULE_FROZEN || MICROPY_VFS
    pyexec_file_if_exists("main.py");
    #endif

    for (;;) {
        if (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL) {
            if (pyexec_raw_repl() != 0) {
                break;
            }
        } else {
            if (pyexec_friendly_repl() != 0) {
                break;
            }
        }
    }

    printf("soft reboot\n");

    #if MICROPY_PY_BLUETOOTH
    mp_bluetooth_deinit();
    #endif
    #if MICROPY_PY_MACHINE
    machine_pin_deinit();
    #endif

    goto soft_reset;

    return 0;
}

void gc_collect(void) {
    // WARNING: This gc_collect implementation doesn't try to get root
    // pointers from CPU registers, and thus may function incorrectly.
    void *dummy;
    gc_collect_start();
    gc_collect_root(&dummy, ((mp_uint_t)MP_STATE_THREAD(stack_top) - (mp_uint_t)&dummy) / sizeof(mp_uint_t));
    gc_collect_end();
    // gc_dump_info();
}

#if !MICROPY_READER_VFS
mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    mp_raise_OSError(ENOENT);
}
#endif

mp_import_stat_t mp_import_stat(const char *path) {
    #if MICROPY_VFS
    return mp_vfs_import_stat(path);
    #else
    return MP_IMPORT_STAT_NO_EXIST;
    #endif
}

mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    #if MICROPY_VFS
    return mp_vfs_open(n_args, args, kwargs);
    #else
    return mp_const_none;
    #endif
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);

NORETURN void nlr_jump_fail(void *val) {
    while (1) {
        ;
    }
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    __fatal_error("Assertion failed");
}
#endif
