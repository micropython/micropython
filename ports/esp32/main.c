/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Development of the code in this file was sponsored by Microbric Pty Ltd
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
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

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_task.h"
#include "soc/cpu.h"
#include "esp_log.h"

#if CONFIG_IDF_TARGET_ESP32
#include "esp32/spiram.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/spiram.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/spiram.h"
#endif

#include "py/stackctrl.h"
#include "py/nlr.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/persistentcode.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/mphal.h"
#include "shared/readline/readline.h"
#include "shared/runtime/pyexec.h"
#include "uart.h"
#include "usb.h"
#include "usb_serial_jtag.h"
#include "modmachine.h"
#include "modnetwork.h"
#include "mpthreadport.h"

#if MICROPY_BLUETOOTH_NIMBLE
#include "extmod/modbluetooth.h"
#endif

#if MICROPY_ESPNOW
#include "modespnow.h"
#endif

// MicroPython runs as a task under FreeRTOS
#define MP_TASK_PRIORITY        (ESP_TASK_PRIO_MIN + 1)
#define MP_TASK_STACK_SIZE      (16 * 1024)

// Set the margin for detecting stack overflow, depending on the CPU architecture.
#if CONFIG_IDF_TARGET_ESP32C3
#define MP_TASK_STACK_LIMIT_MARGIN (2048)
#else
#define MP_TASK_STACK_LIMIT_MARGIN (1024)
#endif

int vprintf_null(const char *format, va_list ap) {
    // do nothing: this is used as a log target during raw repl mode
    return 0;
}

void mp_task(void *pvParameter) {
    volatile uint32_t sp = (uint32_t)get_sp();
    #if MICROPY_PY_THREAD
    mp_thread_init(pxTaskGetStackStart(NULL), MP_TASK_STACK_SIZE / sizeof(uintptr_t));
    #endif
    #if CONFIG_USB_ENABLED
    usb_init();
    #elif CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
    usb_serial_jtag_init();
    #endif
    #if MICROPY_HW_ENABLE_UART_REPL
    uart_stdout_init();
    #endif
    machine_init();

    size_t mp_task_heap_size;
    void *mp_task_heap = NULL;

    #if CONFIG_SPIRAM_USE_MALLOC
    // SPIRAM is issued using MALLOC, fallback to normal allocation rules
    mp_task_heap = NULL;
    #elif CONFIG_ESP32_SPIRAM_SUPPORT
    // Try to use the entire external SPIRAM directly for the heap
    mp_task_heap = (void *)SOC_EXTRAM_DATA_LOW;
    switch (esp_spiram_get_chip_size()) {
        case ESP_SPIRAM_SIZE_16MBITS:
            mp_task_heap_size = 2 * 1024 * 1024;
            break;
        case ESP_SPIRAM_SIZE_32MBITS:
        case ESP_SPIRAM_SIZE_64MBITS:
            mp_task_heap_size = 4 * 1024 * 1024;
            break;
        default:
            // No SPIRAM, fallback to normal allocation
            mp_task_heap = NULL;
            break;
    }
    #elif CONFIG_ESP32S2_SPIRAM_SUPPORT || CONFIG_ESP32S3_SPIRAM_SUPPORT
    // Try to use the entire external SPIRAM directly for the heap
    size_t esp_spiram_size = esp_spiram_get_size();
    if (esp_spiram_size > 0) {
        mp_task_heap = (void *)SOC_EXTRAM_DATA_HIGH - esp_spiram_size;
        mp_task_heap_size = esp_spiram_size;
    }
    #endif

    if (mp_task_heap == NULL) {
        // Allocate the uPy heap using malloc and get the largest available region,
        // limiting to 1/2 total available memory to leave memory for the OS.
        #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 1, 0)
        size_t heap_total = heap_caps_get_total_size(MALLOC_CAP_8BIT);
        #else
        multi_heap_info_t info;
        heap_caps_get_info(&info, MALLOC_CAP_8BIT);
        size_t heap_total = info.total_free_bytes + info.total_allocated_bytes;
        #endif
        mp_task_heap_size = MIN(heap_caps_get_largest_free_block(MALLOC_CAP_8BIT), heap_total / 2);
        mp_task_heap = malloc(mp_task_heap_size);
    }

soft_reset:
    // initialise the stack pointer for the main thread
    mp_stack_set_top((void *)sp);
    mp_stack_set_limit(MP_TASK_STACK_SIZE - MP_TASK_STACK_LIMIT_MARGIN);
    gc_init(mp_task_heap, mp_task_heap + mp_task_heap_size);
    mp_init();
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_lib));
    readline_init0();

    MP_STATE_PORT(native_code_pointers) = MP_OBJ_NULL;

    // initialise peripherals
    machine_pins_init();
    #if MICROPY_PY_MACHINE_I2S
    machine_i2s_init0();
    #endif

    // run boot-up scripts
    pyexec_frozen_module("_boot.py", false);
    pyexec_file_if_exists("boot.py");
    if (pyexec_mode_kind == PYEXEC_MODE_FRIENDLY_REPL) {
        int ret = pyexec_file_if_exists("main.py");
        if (ret & PYEXEC_FORCED_EXIT) {
            goto soft_reset_exit;
        }
    }

    for (;;) {
        if (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL) {
            vprintf_like_t vprintf_log = esp_log_set_vprintf(vprintf_null);
            if (pyexec_raw_repl() != 0) {
                break;
            }
            esp_log_set_vprintf(vprintf_log);
        } else {
            if (pyexec_friendly_repl() != 0) {
                break;
            }
        }
    }

soft_reset_exit:

    #if MICROPY_BLUETOOTH_NIMBLE
    mp_bluetooth_deinit();
    #endif

    #if MICROPY_ESPNOW
    espnow_deinit(mp_const_none);
    MP_STATE_PORT(espnow_singleton) = NULL;
    #endif

    machine_timer_deinit_all();

    #if MICROPY_PY_THREAD
    mp_thread_deinit();
    #endif

    // Free any native code pointers that point to iRAM.
    if (MP_STATE_PORT(native_code_pointers) != MP_OBJ_NULL) {
        size_t len;
        mp_obj_t *items;
        mp_obj_list_get(MP_STATE_PORT(native_code_pointers), &len, &items);
        for (size_t i = 0; i < len; ++i) {
            heap_caps_free(MP_OBJ_TO_PTR(items[i]));
        }
    }

    gc_sweep_all();

    mp_hal_stdout_tx_str("MPY: soft reboot\r\n");

    // deinitialise peripherals
    machine_pwm_deinit_all();
    // TODO: machine_rmt_deinit_all();
    machine_pins_deinit();
    machine_deinit();
    #if MICROPY_PY_USOCKET_EVENTS
    usocket_events_deinit();
    #endif

    mp_deinit();
    fflush(stdout);
    goto soft_reset;
}

void boardctrl_startup(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }
}

void app_main(void) {
    // Hook for a board to run code at start up.
    // This defaults to initialising NVS.
    MICROPY_BOARD_STARTUP();

    // Create and transfer control to the MicroPython task.
    xTaskCreatePinnedToCore(mp_task, "mp_task", MP_TASK_STACK_SIZE / sizeof(StackType_t), NULL, MP_TASK_PRIORITY, &mp_main_task_handle, MP_TASK_COREID);
}

void nlr_jump_fail(void *val) {
    printf("NLR jump failed, val=%p\n", val);
    esp_restart();
}

// modussl_mbedtls uses this function but it's not enabled in ESP IDF
void mbedtls_debug_set_threshold(int threshold) {
    (void)threshold;
}

void *esp_native_code_commit(void *buf, size_t len, void *reloc) {
    len = (len + 3) & ~3;
    uint32_t *p = heap_caps_malloc(len, MALLOC_CAP_EXEC);
    if (p == NULL) {
        m_malloc_fail(len);
    }
    if (MP_STATE_PORT(native_code_pointers) == MP_OBJ_NULL) {
        MP_STATE_PORT(native_code_pointers) = mp_obj_new_list(0, NULL);
    }
    mp_obj_list_append(MP_STATE_PORT(native_code_pointers), MP_OBJ_TO_PTR(p));
    if (reloc) {
        mp_native_relocate(reloc, buf, (uintptr_t)p);
    }
    memcpy(p, buf, len);
    return p;
}

MP_REGISTER_ROOT_POINTER(mp_obj_t native_code_pointers);
