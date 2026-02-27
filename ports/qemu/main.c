/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014-2023 Damien P. George
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

#include <stdlib.h>

#include "extmod/vfs.h"
#include "extmod/vfs_semihosting.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "shared/runtime/gchelper.h"
#include "shared/runtime/pyexec.h"
#include "shared/runtime/semihosting.h"

#if MICROPY_HEAP_SIZE <= 0
#error MICROPY_HEAP_SIZE must be a positive integer.
#endif

static uint32_t gc_heap[MICROPY_HEAP_SIZE / sizeof(uint32_t)];

int main(int argc, char **argv) {
    mp_cstack_init_with_sp_here(10240);
    gc_init(gc_heap, (char *)gc_heap + MICROPY_HEAP_SIZE);

    #if MICROPY_VFS_SEMIHOSTING
    // The fastest way to get something like command line pararameters into a
    // non-Linux QEMU kernel is to use semihosting's command line argument
    // string.  If there is no data to be put in the semihosting command
    // buffer, QEMU puts the file name of the running kernel image in there
    // instead (probably as an argv[0] emulation mechanism).  Anything coming
    // in from the outside will be put in the buffer with no checks.
    //
    // Semihosting command buffer data can be passed from QEMU via the
    // `-semihosting-config` command line argument, in the form
    // '-semihosting-config arg="command line"' (the enclosing quotes will not
    // be put in the buffer).
    //
    // One thing worth mentioning is the horrific lack of security of this
    // mechanism, as besides MicroPython's honouring an optional `readonly`
    // mount flag, the emulated interpreter is free to run havoc all through
    // the host filesystem.

    char command_line[512] = { 0 };
    if (mp_semihosting_get_cmdline(&command_line, sizeof(command_line) - 1) != 0) {
        mp_printf(&mp_plat_print, "Semihosting command line too long.\n");
        exit(1);
    }

    // Since QEMU cannot have an empty buffer we have to add some command line
    // parsing to make sure what is in there is actually an intended mountpoint
    // and not the kernel image path.  For that, the mount point should be
    // prefixed by the "-mp" commandline argument at position 0.

    char *mountpoint = NULL;
    if (command_line[0] == '-' && command_line[1] == 'm' &&
        command_line[2] == 'p' && command_line[3] == ' ') {
        // Skip any further whitespace.
        mountpoint = &command_line[4];
        while (*mountpoint == ' ' || *mountpoint == '\t') {
            mountpoint++;
        }
        // Make sure there's something worthwhile.
        if (*mountpoint == '\0') {
            mountpoint = NULL;
        }
    }
    #endif

    for (;;) {
        mp_init();

        #if MICROPY_VFS_SEMIHOSTING
        if (mountpoint) {
            mp_obj_t mountpoint_obj = mp_obj_new_str_from_cstr(mountpoint);
            mp_obj_t args[2] = {
                MP_OBJ_TYPE_GET_SLOT(&mp_type_vfs_semihosting, make_new)(&mp_type_vfs_semihosting, 1, 0, &mountpoint_obj),
                MP_OBJ_NEW_QSTR(MP_QSTR__slash_),
            };
            mp_vfs_mount(2, args, (mp_map_t *)&mp_const_empty_map);
            MP_STATE_VM(vfs_cur) = MP_STATE_VM(vfs_mount_table);
        }
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

        mp_printf(&mp_plat_print, "MPY: soft reboot\n");

        gc_sweep_all();
        mp_deinit();
    }
}

void gc_collect(void) {
    gc_collect_start();
    gc_helper_collect_regs_and_stack();
    gc_collect_end();
}

void nlr_jump_fail(void *val) {
    mp_printf(&mp_plat_print, "uncaught NLR\n");
    exit(1);
}
