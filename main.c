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

#include "genhdr/mpversion.h"
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

#include "background.h"
#include "mpconfigboard.h"
#include "supervisor/background_callback.h"
#include "supervisor/board.h"
#include "supervisor/cpu.h"
#include "supervisor/filesystem.h"
#include "supervisor/memory.h"
#include "supervisor/port.h"
#include "supervisor/serial.h"
#include "supervisor/shared/autoreload.h"
#include "supervisor/shared/rgb_led_status.h"
#include "supervisor/shared/safe_mode.h"
#include "supervisor/shared/stack.h"
#include "supervisor/shared/status_leds.h"
#include "supervisor/shared/translate.h"
#include "supervisor/shared/workflow.h"
#include "supervisor/usb.h"

#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Processor.h"
#include "shared-bindings/supervisor/Runtime.h"

#if CIRCUITPY_ALARM
#include "shared-bindings/alarm/__init__.h"
#endif

#if CIRCUITPY_BLEIO
#include "shared-bindings/_bleio/__init__.h"
#include "supervisor/shared/bluetooth.h"
#endif

#if CIRCUITPY_BOARD
#include "shared-module/board/__init__.h"
#endif

#if CIRCUITPY_CANIO
#include "common-hal/canio/CAN.h"
#endif

#if CIRCUITPY_DISPLAYIO
#include "shared-module/displayio/__init__.h"
#endif

#if CIRCUITPY_MEMORYMONITOR
#include "shared-module/memorymonitor/__init__.h"
#endif

#if CIRCUITPY_NETWORK
#include "shared-module/network/__init__.h"
#endif

#if CIRCUITPY_USB_CDC
#include "shared-module/usb_cdc/__init__.h"
#endif

#if CIRCUITPY_WIFI
#include "shared-bindings/wifi/__init__.h"
#endif

#if MICROPY_ENABLE_PYSTACK
static size_t PLACE_IN_DTCM_BSS(_pystack[CIRCUITPY_PYSTACK_SIZE / sizeof(size_t)]);
#endif

static void reset_devices(void) {
#if CIRCUITPY_BLEIO_HCI
    bleio_reset();
#endif
}

STATIC void start_mp(supervisor_allocation* heap) {
    reset_status_led();
    autoreload_stop();
    supervisor_workflow_reset();

    // Stack limit should be less than real stack size, so we have a chance
    // to recover from limit hit.  (Limit is measured in bytes.)
    mp_stack_ctrl_init();

    if (stack_get_bottom() != NULL) {
        mp_stack_set_limit(stack_get_length() - 1024);
    }


#if MICROPY_MAX_STACK_USAGE
    // _ezero (same as _ebss) is an int, so start 4 bytes above it.
    if (stack_get_bottom() != NULL) {
        mp_stack_set_bottom(stack_get_bottom());
        mp_stack_fill_with_sentinel();
    }
#endif

    // Sync the file systems in case any used RAM from the GC to cache. As soon
    // as we re-init the GC all bets are off on the cache.
    filesystem_flush();

    // Clear the readline history. It references the heap we're about to destroy.
    readline_init0();

    #if MICROPY_ENABLE_PYSTACK
    mp_pystack_init(_pystack, _pystack + (sizeof(_pystack) / sizeof(size_t)));
    #endif

    #if MICROPY_ENABLE_GC
    gc_init(heap->ptr, heap->ptr + get_allocation_length(heap) / 4);
    #endif
    mp_init();
    mp_obj_list_init(mp_sys_path, 0);
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR_)); // current dir (or base dir of the script)
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_));
    // Frozen modules are in their own pseudo-dir, e.g., ".frozen".
    // Prioritize .frozen over /lib.
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_FROZEN_FAKE_DIR_QSTR));
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_lib));

    mp_obj_list_init(mp_sys_argv, 0);

    #if CIRCUITPY_ALARM
    // Record which alarm woke us up, if any. An object may be created so the heap must be functional.
    shared_alarm_save_wake_alarm();
    // Reset alarm module only after we retrieved the wakeup alarm.
    common_hal_alarm_reset();
    #endif

    #if CIRCUITPY_NETWORK
    network_module_init();
    #endif
}

STATIC void stop_mp(void) {
    #if CIRCUITPY_NETWORK
    network_module_deinit();
    #endif

    #if MICROPY_VFS
    mp_vfs_mount_t *vfs = MP_STATE_VM(vfs_mount_table);

    // Unmount all heap allocated vfs mounts.
    while (gc_nbytes(vfs) > 0) {
        vfs = vfs->next;
    }
    MP_STATE_VM(vfs_mount_table) = vfs;
    MP_STATE_VM(vfs_cur) = vfs;
    #endif

    background_callback_reset();
    usb_background();

    gc_deinit();
}

#define STRING_LIST(...) {__VA_ARGS__, ""}

// Look for the first file that exists in the list of filenames, using mp_import_stat().
// Return its index. If no file found, return -1.
STATIC const char* first_existing_file_in_list(const char * const * filenames) {
    for (int i = 0; filenames[i] != (char*)""; i++) {
        mp_import_stat_t stat = mp_import_stat(filenames[i]);
        if (stat == MP_IMPORT_STAT_FILE) {
            return filenames[i];
        }
    }
    return NULL;
}

STATIC bool maybe_run_list(const char * const * filenames, pyexec_result_t* exec_result) {
    const char* filename = first_existing_file_in_list(filenames);
    if (filename == NULL) {
        return false;
    }
    mp_hal_stdout_tx_str(filename);
    const compressed_string_t* compressed = translate(" output:\n");
    char decompressed[decompress_length(compressed)];
    decompress(compressed, decompressed);
    mp_hal_stdout_tx_str(decompressed);
    pyexec_file(filename, exec_result);
    return true;
}

STATIC void cleanup_after_vm(supervisor_allocation* heap) {
    // Reset port-independent devices, like CIRCUITPY_BLEIO_HCI.
    reset_devices();
    // Turn off the display and flush the filesystem before the heap disappears.
    #if CIRCUITPY_DISPLAYIO
    reset_displays();
    #endif
    #if CIRCUITPY_MEMORYMONITOR
    memorymonitor_reset();
    #endif
    filesystem_flush();
    stop_mp();
    free_memory(heap);
    supervisor_move_memory();

    #if CIRCUITPY_CANIO
    common_hal_canio_reset();
    #endif

    // reset_board_busses() first because it may release pins from the never_reset state, so that
    // reset_port() can reset them.
    #if CIRCUITPY_BOARD
    reset_board_busses();
    #endif
    reset_port();
    reset_board();
    reset_status_led();
}

STATIC void print_code_py_status_message(safe_mode_t safe_mode) {
    if (autoreload_is_enabled()) {
        serial_write_compressed(translate("Auto-reload is on. Simply save files over USB to run them or enter REPL to disable.\n"));
    } else {
        serial_write_compressed(translate("Auto-reload is off.\n"));
    }
    if (safe_mode != NO_SAFE_MODE) {
        serial_write_compressed(translate("Running in safe mode! "));
        serial_write_compressed(translate("Not running saved code.\n"));
    }
}

STATIC bool run_code_py(safe_mode_t safe_mode) {
    bool serial_connected_at_start = serial_connected();
    #if CIRCUITPY_AUTORELOAD_DELAY_MS > 0
    serial_write("\n");
    print_code_py_status_message(safe_mode);
    print_safe_mode_message(safe_mode);
    serial_write("\n");
    #endif

    pyexec_result_t result;

    result.return_code = 0;
    result.exception_type = NULL;
    result.exception_line = 0;

    bool found_main = false;

    if (safe_mode == NO_SAFE_MODE) {
        new_status_color(MAIN_RUNNING);

        static const char * const supported_filenames[] = STRING_LIST(
            "code.txt", "code.py", "main.py", "main.txt");
        #if CIRCUITPY_FULL_BUILD
        static const char * const double_extension_filenames[] = STRING_LIST(
            "code.txt.py", "code.py.txt", "code.txt.txt","code.py.py",
            "main.txt.py", "main.py.txt", "main.txt.txt","main.py.py");
        #endif

        stack_resize();
        filesystem_flush();
        supervisor_allocation* heap = allocate_remaining_memory();
        start_mp(heap);

        found_main = maybe_run_list(supported_filenames, &result);
        #if CIRCUITPY_FULL_BUILD
        if (!found_main){
            found_main = maybe_run_list(double_extension_filenames, &result);
            if (found_main) {
                serial_write_compressed(translate("WARNING: Your code filename has two extensions\n"));
            }
        }
        #endif

        cleanup_after_vm(heap);

        if (result.return_code & PYEXEC_FORCED_EXIT) {
            return reload_requested;
        }

        if (reload_requested && result.return_code == PYEXEC_EXCEPTION) {
            serial_write_compressed(translate("\nCode stopped by auto-reload.\n"));
        } else {
            serial_write_compressed(translate("\nCode done running.\n"));
        }
    }

    // Program has finished running.

    bool printed_press_any_key = false;
    #if CIRCUITPY_DISPLAYIO
    bool refreshed_epaper_display = false;
    #endif

    rgb_status_animation_t animation;
    prep_rgb_status_animation(&result, found_main, safe_mode, &animation);
    bool fake_sleeping = false;
    while (true) {
        RUN_BACKGROUND_TASKS;
        if (reload_requested) {
            #if CIRCUITPY_ALARM
            if (fake_sleeping) {
                board_init();
            }
            #endif
            supervisor_set_run_reason(RUN_REASON_AUTO_RELOAD);
            reload_requested = false;
            return true;
        }

        if (serial_connected() && serial_bytes_available()) {
            #if CIRCUITPY_ALARM
            if (fake_sleeping) {
                board_init();
            }
            #endif
            // Skip REPL if reload was requested.
            bool ctrl_d = serial_read() == CHAR_CTRL_D;
            if (ctrl_d) {
                supervisor_set_run_reason(RUN_REASON_REPL_RELOAD);
            }
            return ctrl_d;
        }

        // Check for a deep sleep alarm and restart the VM. This can happen if
        // an alarm alerts faster than our USB delay or if we pretended to deep
        // sleep.
        #if CIRCUITPY_ALARM
        if (fake_sleeping && common_hal_alarm_woken_from_sleep()) {
            serial_write_compressed(translate("Woken up by alarm.\n"));
            board_init();
            supervisor_set_run_reason(RUN_REASON_STARTUP);
            return true;
        }
        #endif

        if (!printed_press_any_key && serial_connected()) {
            if (!serial_connected_at_start) {
                print_code_py_status_message(safe_mode);
            }

            print_safe_mode_message(safe_mode);
            serial_write("\n");
            serial_write_compressed(translate("Press any key to enter the REPL. Use CTRL-D to reload.\n"));
            printed_press_any_key = true;
        }
        if (!serial_connected()) {
            serial_connected_at_start = false;
            printed_press_any_key = false;
        }

        // Refresh the ePaper display if we have one. That way it'll show an error message.
        #if CIRCUITPY_DISPLAYIO
        // Don't refresh the display if we're about to deep sleep.
        #if CIRCUITPY_ALARM
        refreshed_epaper_display = refreshed_epaper_display || result.return_code & PYEXEC_DEEP_SLEEP;
        #endif
        if (!refreshed_epaper_display) {
            refreshed_epaper_display = maybe_refresh_epaperdisplay();
        }
        #endif

        // Sleep until our next interrupt.
        #if CIRCUITPY_ALARM
        if (result.return_code & PYEXEC_DEEP_SLEEP) {
            // Make sure we have been awake long enough for USB to connect (enumeration delay).
            int64_t connecting_delay_ticks = CIRCUITPY_USB_CONNECTED_SLEEP_DELAY * 1024 - port_get_raw_ticks(NULL);
            // Until it's safe to decide whether we're real/fake sleeping, just run the RGB
            if (connecting_delay_ticks < 0 && !fake_sleeping) {
                fake_sleeping = true;
                new_status_color(BLACK);
                board_deinit();
                if (!supervisor_workflow_active()) {
                    // Enter true deep sleep. When we wake up we'll be back at the
                    // top of main(), not in this loop.
                    common_hal_alarm_enter_deep_sleep();
                    // Does not return.
                } else {
                    serial_write_compressed(translate("Pretending to deep sleep until alarm, CTRL-C or file write.\n"));
                }
            }
        }
        #endif

        if (!fake_sleeping) {
            tick_rgb_status_animation(&animation);
        } else {
            // This waits until a pretend deep sleep alarm occurs. They are set
            // during common_hal_alarm_set_deep_sleep_alarms. On some platforms
            // it may also return due to another interrupt, that's why we check
            // for deep sleep alarms above. If it wasn't a deep sleep alarm,
            // then we'll idle here again.
            #if CIRCUITPY_ALARM
                common_hal_alarm_pretending_deep_sleep();
            #else
                port_idle_until_interrupt();
            #endif
        }
    }
}

FIL* boot_output_file;

STATIC void __attribute__ ((noinline)) run_boot_py(safe_mode_t safe_mode) {
    // If not in safe mode, run boot before initing USB and capture output in a
    // file.
    if (filesystem_present() && safe_mode == NO_SAFE_MODE && MP_STATE_VM(vfs_mount_table) != NULL) {
        static const char * const boot_py_filenames[] = STRING_LIST("settings.txt", "settings.py", "boot.py", "boot.txt");

        new_status_color(BOOT_RUNNING);

        #ifdef CIRCUITPY_BOOT_OUTPUT_FILE
        FIL file_pointer;
        boot_output_file = &file_pointer;

        // Get the base filesystem.
        FATFS *fs = &((fs_user_mount_t *) MP_STATE_VM(vfs_mount_table)->obj)->fatfs;

        bool have_boot_py = first_existing_file_in_list(boot_py_filenames) != NULL;

        bool skip_boot_output = false;

        // If there's no boot.py file that might write some changing output,
        // read the existing copy of CIRCUITPY_BOOT_OUTPUT_FILE and see if its contents
        // match the version info we would print anyway. If so, skip writing CIRCUITPY_BOOT_OUTPUT_FILE.
        // This saves wear and tear on the flash and also prevents filesystem damage if power is lost
        // during the write, which may happen due to bobbling the power connector or weak power.

        static const size_t NUM_CHARS_TO_COMPARE = 160;
        if (!have_boot_py && f_open(fs, boot_output_file, CIRCUITPY_BOOT_OUTPUT_FILE, FA_READ) == FR_OK) {

            char file_contents[NUM_CHARS_TO_COMPARE];
            UINT chars_read = 0;
            f_read(boot_output_file, file_contents, NUM_CHARS_TO_COMPARE, &chars_read);
            f_close(boot_output_file);
            skip_boot_output =
                // + 2 accounts for  \r\n.
                chars_read == strlen(MICROPY_FULL_VERSION_INFO) + 2 &&
                strncmp(file_contents, MICROPY_FULL_VERSION_INFO, strlen(MICROPY_FULL_VERSION_INFO)) == 0;
        }

        if (!skip_boot_output) {
            // Wait 1.5 seconds before opening CIRCUITPY_BOOT_OUTPUT_FILE for write,
            // in case power is momentary or will fail shortly due to, say a low, battery.
            if (common_hal_mcu_processor_get_reset_reason() == RESET_REASON_POWER_ON) {
                mp_hal_delay_ms(1500);
            }
            // USB isn't up, so we can write the file.
            filesystem_set_internal_writable_by_usb(false);
            f_open(fs, boot_output_file, CIRCUITPY_BOOT_OUTPUT_FILE, FA_WRITE | FA_CREATE_ALWAYS);

            // Switch the filesystem back to non-writable by Python now instead of later,
            // since boot.py might change it back to writable.
            filesystem_set_internal_writable_by_usb(true);

            // Write version info to boot_out.txt.
            mp_hal_stdout_tx_str(MICROPY_FULL_VERSION_INFO);
            mp_hal_stdout_tx_str("\r\n");
        }
        #endif

        // TODO(tannewt): Allocate temporary space to hold custom usb descriptors.
        filesystem_flush();
        supervisor_allocation* heap = allocate_remaining_memory();
        start_mp(heap);

        // TODO(tannewt): Re-add support for flashing boot error output.
        bool found_boot = maybe_run_list(boot_py_filenames, NULL);
        (void) found_boot;

        #ifdef CIRCUITPY_BOOT_OUTPUT_FILE
        if (!skip_boot_output) {
            f_close(boot_output_file);
            filesystem_flush();
        }
        boot_output_file = NULL;
        #endif

        cleanup_after_vm(heap);
    }
}

STATIC int run_repl(void) {
    int exit_code = PYEXEC_FORCED_EXIT;
    stack_resize();
    filesystem_flush();
    supervisor_allocation* heap = allocate_remaining_memory();
    start_mp(heap);
    autoreload_suspend();
    new_status_color(REPL_RUNNING);
    if (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL) {
        exit_code = pyexec_raw_repl();
    } else {
        exit_code = pyexec_friendly_repl();
    }
    cleanup_after_vm(heap);
    autoreload_resume();
    return exit_code;
}

int __attribute__((used)) main(void) {
    // initialise the cpu and peripherals
    safe_mode_t safe_mode = port_init();

    // Turn on LEDs
    init_status_leds();
    rgb_led_status_init();

    // Wait briefly to give a reset window where we'll enter safe mode after the reset.
    if (safe_mode == NO_SAFE_MODE) {
        safe_mode = wait_for_safe_mode_reset();
    }

    stack_init();

    // Create a new filesystem only if we're not in a safe mode.
    // A power brownout here could make it appear as if there's
    // no SPI flash filesystem, and we might erase the existing one.
    filesystem_init(safe_mode == NO_SAFE_MODE, false);

    // displays init after filesystem, since they could share the flash SPI
    board_init();

    // Start the debug serial
    serial_early_init();

    // Reset everything and prep MicroPython to run boot.py.
    reset_port();
    // Port-independent devices, like CIRCUITPY_BLEIO_HCI.
    reset_devices();
    reset_board();

    // This is first time we are running CircuitPython after a reset or power-up.
    supervisor_set_run_reason(RUN_REASON_STARTUP);

    // If not in safe mode turn on autoreload by default but before boot.py in case it wants to change it.
    if (safe_mode == NO_SAFE_MODE) {
        autoreload_enable();
    }

    // By default our internal flash is readonly to local python code and
    // writable over USB. Set it here so that boot.py can change it.
    filesystem_set_internal_concurrent_write_protection(true);
    filesystem_set_internal_writable_by_usb(true);

    run_boot_py(safe_mode);

    // Start serial and HID after giving boot.py a chance to tweak behavior.
    serial_init();

    #if CIRCUITPY_BLEIO
    supervisor_start_bluetooth();
    #endif

    // Boot script is finished, so now go into REPL/main mode.
    int exit_code = PYEXEC_FORCED_EXIT;
    bool skip_repl = true;
    bool first_run = true;
    for (;;) {
        if (!skip_repl) {
            exit_code = run_repl();
        }
        if (exit_code == PYEXEC_FORCED_EXIT) {
            if (!first_run) {
                serial_write_compressed(translate("soft reboot\n"));
            }
            first_run = false;
            skip_repl = run_code_py(safe_mode);
        } else if (exit_code != 0) {
            break;
        }
    }
    mp_deinit();
    return 0;
}

void gc_collect(void) {
    gc_collect_start();

    mp_uint_t regs[10];
    mp_uint_t sp = cpu_get_regs_and_sp(regs);

    // This collects root pointers from the VFS mount table. Some of them may
    // have lost their references in the VM even though they are mounted.
    gc_collect_root((void**)&MP_STATE_VM(vfs_mount_table), sizeof(mp_vfs_mount_t) / sizeof(mp_uint_t));

    background_callback_gc_collect();

    #if CIRCUITPY_ALARM
    common_hal_alarm_gc_collect();
    #endif

    #if CIRCUITPY_DISPLAYIO
    displayio_gc_collect();
    #endif

    #if CIRCUITPY_BLEIO
    common_hal_bleio_gc_collect();
    #endif

    #if CIRCUITPY_WIFI
    common_hal_wifi_gc_collect();
    #endif

    // This naively collects all object references from an approximate stack
    // range.
    gc_collect_root((void**)sp, ((uint32_t)port_stack_get_top() - sp) / sizeof(uint32_t));
    gc_collect_end();
}

void NORETURN nlr_jump_fail(void *val) {
    reset_into_safe_mode(MICROPY_NLR_JUMP_FAIL);
    while (true) {}
}

void NORETURN __fatal_error(const char *msg) {
    reset_into_safe_mode(MICROPY_FATAL_ERROR);
    while (true) {}
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    mp_printf(&mp_plat_print, "Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    __fatal_error("Assertion failed");
}
#endif
