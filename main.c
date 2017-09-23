/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2017 Scott Shawcroft for Adafruit Industries
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
#include <string.h>

#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"

#include "py/nlr.h"
#include "py/compile.h"
#include "py/frozenmod.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/stackctrl.h"

#include "lib/mp-readline/readline.h"
#include "lib/utils/pyexec.h"

#include "mpconfigboard.h"
#include "supervisor/port.h"
#include "supervisor/filesystem.h"
// TODO(tannewt): Figure out how to choose language at compile time.
#include "supervisor/messages/en-US.h"
#include "supervisor/shared/autoreload.h"
#include "supervisor/shared/rgb_led_status.h"
#include "supervisor/serial.h"

void do_str(const char *src, mp_parse_input_kind_t input_kind) {
    mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, src, strlen(src), 0);
    if (lex == NULL) {
        //printf("MemoryError: lexer could not allocate memory\n");
        return;
    }

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        qstr source_name = lex->source_name;
        mp_parse_tree_t parse_tree = mp_parse(lex, input_kind);
        mp_obj_t module_fun = mp_compile(&parse_tree, source_name, MP_EMIT_OPT_NONE, true);
        mp_call_function_0(module_fun);
        nlr_pop();
    } else {
        // uncaught exception
        mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
    }
}

static char heap[PORT_HEAP_SIZE];

void reset_mp(void) {
    reset_status_led();
    new_status_color(0x8f008f);
    autoreload_stop();

    // Sync the file systems in case any used RAM from the GC to cache. As soon
    // as we re-init the GC all bets are off on the cache.
    filesystem_flush();

    // Clear the readline history. It references the heap we're about to destroy.
    readline_init0();

    #if MICROPY_ENABLE_GC
    gc_init(heap, heap + sizeof(heap));
    #endif
    mp_init();
    mp_obj_list_init(mp_sys_path, 0);
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR_)); // current dir (or base dir of the script)
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_));
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_lib));
    // Frozen modules are in their own pseudo-dir, e.g., ".frozen".
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_FROZEN_FAKE_DIR_QSTR));

    mp_obj_list_init(mp_sys_argv, 0);
}

bool maybe_run(const char* filename, pyexec_result_t* exec_result) {
    mp_import_stat_t stat = mp_import_stat(filename);
    if (stat != MP_IMPORT_STAT_FILE) {
        return false;
    }
    serial_write(filename);
    serial_write(MSG_OUTPUT_SUFFIX);
    pyexec_file(filename, exec_result);
    return true;
}

bool start_mp(safe_mode_t safe_mode) {
    bool serial_connected_at_start = serial_connected();
    #ifdef CIRCUITPY_AUTORELOAD_DELAY_MS
    if (serial_connected_at_start) {
        serial_write(MSG_NEWLINE);
        if (autoreload_is_enabled()) {
            serial_write(MSG_AUTORELOAD_ON);
        } else if (safe_mode != NO_SAFE_MODE) {
            serial_write(MSG_SAFE_MODE_ON);
        } else if (!autoreload_is_enabled()) {
            serial_write(MSG_AUTORELOAD_OFF);
        }
    }
    #endif

    pyexec_result_t result;
    bool found_main = false;
    if (safe_mode != NO_SAFE_MODE) {
        serial_write(MSG_SAFE_MODE_NO_MAIN);
    } else {
        new_status_color(MAIN_RUNNING);
        found_main = maybe_run("code.txt", &result) ||
                     maybe_run("code.py", &result) ||
                     maybe_run("main.py", &result) ||
                     maybe_run("main.txt", &result);
        reset_status_led();

        if (result.return_code & PYEXEC_FORCED_EXIT) {
            return reload_next_character;
        }
    }

    // Wait for connection or character.
    bool serial_connected_before_animation = false;
    rgb_status_animation_t animation;
    prep_rgb_status_animation(&result, found_main, &animation);
    while (true) {
        #ifdef MICROPY_VM_HOOK_LOOP
            MICROPY_VM_HOOK_LOOP
        #endif
        if (reload_next_character) {
            return true;
        }
        if (serial_connected() && serial_bytes_available()) {
            // Skip REPL if reload was requested.
            return serial_read() == CHAR_CTRL_D;
        }

        if (!serial_connected_before_animation && serial_connected()) {
            if (serial_connected_at_start) {
                serial_write(MSG_NEWLINE MSG_NEWLINE);
            }

            if (!serial_connected_at_start) {
                if (autoreload_is_enabled()) {
                    serial_write(MSG_AUTORELOAD_ON);
                } else {
                    serial_write(MSG_AUTORELOAD_OFF);
                }
            }
            // Output a user safe mode string if its set.
            #ifdef BOARD_USER_SAFE_MODE
            if (safe_mode == USER_SAFE_MODE) {
                serial_write(MSG_NEWLINE MSG_SAFE_MODE_USER_REQUESTED);
                serial_write(BOARD_USER_SAFE_MODE_ACTION);
                serial_write(MSG_NEWLINE MSG_SAFE_MODE_USER_EXIT);
                serial_write(BOARD_USER_SAFE_MODE_ACTION);
                serial_write(MSG_NEWLINE);
            } else
            #endif
            if (safe_mode != NO_SAFE_MODE) {
                serial_write(MSG_NEWLINE MSG_BAD_SAFE_MODE MSG_NEWLINE);
                if (safe_mode == HARD_CRASH) {
                    serial_write(MSG_SAFE_MODE_CRASH MSG_NEWLINE);
                    serial_write(MSG_SAFE_MODE_FILE_ISSUE MSG_NEWLINE);
                    serial_write(MSG_SAFE_MODE_ISSUE_LINK MSG_NEWLINE);
                } else if (safe_mode == BROWNOUT) {
                    serial_write(MSG_SAFE_MODE_BROWN_OUT_LINE_1 MSG_NEWLINE);
                    serial_write(MSG_SAFE_MODE_BROWN_OUT_LINE_2 MSG_NEWLINE);
                }
            }
            serial_write(MSG_NEWLINE MSG_WAIT_BEFORE_REPL MSG_NEWLINE);
        }
        if (serial_connected_before_animation && !serial_connected()) {
            serial_connected_at_start = false;
        }
        serial_connected_before_animation = serial_connected();

        tick_rgb_status_animation(&animation);
    }
}

int __attribute__((used)) main(void) {
    // initialise the cpu and peripherals
    safe_mode_t safe_mode = port_init();

    rgb_led_status_init();

    // Stack limit should be less than real stack size, so we have a chance
    // to recover from limit hit.  (Limit is measured in bytes.)
    mp_stack_ctrl_init();
    mp_stack_set_limit((char*)&_estack - (char*)&_ebss - 1024);

#if MICROPY_MAX_STACK_USAGE
    // _ezero (same as _ebss) is an int, so start 4 bytes above it.
    mp_stack_set_bottom(&_ezero + 1);
    mp_stack_fill_with_sentinel();
#endif

    filesystem_init();

    // Reset everything and prep MicroPython to run boot.py.
    reset_port();
    reset_board();
    reset_mp();

    // Turn on autoreload by default but before boot.py in case it wants to change it.
    autoreload_enable();

    // By default our internal flash is readonly to local python code and
    // writeable over USB. Set it here so that boot.py can change it.
    filesystem_default_writeable(false);

    // If not in safe mode, run boot before initing USB and capture output in a
    // file.
    if (filesystem_present() && safe_mode == NO_SAFE_MODE && MP_STATE_VM(vfs_mount_table) != NULL) {
        new_status_color(BOOT_RUNNING);
        #ifdef CIRCUITPY_BOOT_OUTPUT_FILE
        // Since USB isn't up yet we can cheat and let ourselves write the boot
        // output file.
        filesystem_default_writeable(true);
        FIL file_pointer;
        boot_output_file = &file_pointer;
        f_open(&((fs_user_mount_t *) MP_STATE_VM(vfs_mount_table)->obj)->fatfs,
            boot_output_file, CIRCUITPY_BOOT_OUTPUT_FILE, FA_WRITE | FA_CREATE_ALWAYS);
        filesystem_default_writeable(false);
        #endif

        // TODO(tannewt): Re-add support for flashing boot error output.
        bool found_boot = maybe_run("settings.txt", NULL) ||
                          maybe_run("settings.py", NULL) ||
                          maybe_run("boot.py", NULL) ||
                          maybe_run("boot.txt", NULL);
        (void) found_boot;

        #ifdef CIRCUITPY_BOOT_OUTPUT_FILE
        f_close(boot_output_file);
        filesystem_flush();
        boot_output_file = NULL;
        #endif

        // Reset to remove any state that boot.py setup. It should only be used to
        // change internal state thats not in the heap.
        reset_port();
        reset_mp();
    }

    // Start serial after giving boot.py a chance to tweak behavior.
    serial_init();

    // Boot script is finished, so now go into REPL/main mode.
    int exit_code = PYEXEC_FORCED_EXIT;
    bool skip_repl = true;
    bool first_run = true;
    for (;;) {
        if (!skip_repl) {
            // The REPL mode can change, or it can request a reload.
            bool autoreload_on = autoreload_is_enabled();
            autoreload_disable();
            new_status_color(REPL_RUNNING);
            if (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL) {
                exit_code = pyexec_raw_repl();
            } else {
                exit_code = pyexec_friendly_repl();
            }
            if (autoreload_on) {
                autoreload_enable();
            }
            reset_port();
            reset_board();
            reset_mp();
        }
        if (exit_code == PYEXEC_FORCED_EXIT) {
            if (!first_run) {
                serial_write(MSG_SOFT_REBOOT MSG_NEWLINE);
            }
            first_run = false;
            skip_repl = start_mp(safe_mode);
            reset_port();
            reset_board();
            reset_mp();
        } else if (exit_code != 0) {
            break;
        }
    }
    mp_deinit();
    return 0;
}

void gc_collect(void) {
    // WARNING: This gc_collect implementation doesn't try to get root
    // pointers from CPU registers, and thus may function incorrectly.
    void *dummy;
    gc_collect_start();
    // This collects root pointers from the VFS mount table. Some of them may
    // have lost their references in the VM even though they are mounted.
    gc_collect_root((void**)&MP_STATE_VM(vfs_mount_table), sizeof(mp_vfs_mount_t) / sizeof(mp_uint_t));
    // This naively collects all object references from an approximate stack
    // range.
    gc_collect_root(&dummy, ((mp_uint_t)&_estack - (mp_uint_t)&dummy) / sizeof(mp_uint_t));
    gc_collect_end();
}

void NORETURN nlr_jump_fail(void *val) {
    HardFault_Handler();
    while (true) {}
}

void NORETURN __fatal_error(const char *msg) {
    HardFault_Handler();
    while (true) {}
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    __fatal_error("Assertion failed");
}
#endif
