/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022-2025 Infineon Technologies AG
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

// std includes
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>


// MTB includes
#include "cybsp.h"
#include "retarget_io_init.h"

// FreeRTOS header files
#include <FreeRTOS.h>
#include <task.h>

// micropython includes
#include "py/builtin.h"
#include "py/compile.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/stackctrl.h"
#include "py/runtime.h"
#include "shared/runtime/gchelper.h"
#include "shared/runtime/pyexec.h"
#include "shared/readline/readline.h"
#include "extmod/modnetwork.h"

#if MICROPY_PY_LWIP
#include "lwip/init.h"
#include "lwip/apps/mdns.h"
#endif

#if MICROPY_PY_NETWORK
#include "network_ifx_wcm.h"
#endif

// port-specific includes

// FreeRTOS task parameters for the MicroPython task
#define MPY_TASK_STACK_SIZE     (5120u)
#define MPY_TASK_PRIORITY       (1u)

// Size of the MicroPython GC heap.
// Must be small enough that .bss + this array still fits within the m33_data
// region (256 KB) while leaving adequate dynamic heap for FreeRTOS task stacks
// (~40 KB) and the WiFi/WCM/LwIP stack (~60-80 KB).
#ifndef MICROPY_GC_HEAP_SIZE
#define MICROPY_GC_HEAP_SIZE    (96 * 1024u)
#endif

typedef enum {
    BOOT_MODE_NORMAL,
    BOOT_MODE_SAFE
} boot_mode_t;

extern void machine_rtc_init_all(void);
extern void time_init(void);
extern void machine_pin_irq_deinit_all(void);
extern void machine_hw_i2c_deinit_all(void);
extern void machine_pdm_pcm_deinit_all(void);
extern void machine_ipc_deinit_all(void);

void mpy_task(void *arg);
static TaskHandle_t mpy_task_handle;

// Static GC heap in .bss – keeps it out of the dynamic heap that FreeRTOS
// task stacks and the WiFi/WCM/LwIP stack consume at runtime.
#if MICROPY_ENABLE_GC
static char mpy_gc_heap[MICROPY_GC_HEAP_SIZE];
#endif

boot_mode_t check_boot_mode(void) {
    boot_mode_t boot_mode;

    // Initialize user LED
    Cy_GPIO_Pin_FastInit(CYBSP_USER_LED_PORT, CYBSP_USER_LED_NUM, CY_GPIO_DM_STRONG, 0, HSIOM_SEL_GPIO);

    // Initialize user button
    Cy_GPIO_Pin_FastInit(CYBSP_USER_BTN1_PORT, CYBSP_USER_BTN1_NUM, CY_GPIO_DM_PULLUP, 1, HSIOM_SEL_GPIO);

    // Added 5ms delay to allow bypass capacitor connected to the user button without external pull-up to charge.
    Cy_SysLib_Delay(5);

    if (Cy_GPIO_Read(CYBSP_USER_BTN1_PORT, CYBSP_USER_BTN1_NUM) == CYBSP_BTN_PRESSED) {
        // Blink LED twice to indicate safe boot mode was entered
        for (int i = 0; i < 4; i++) {
            Cy_GPIO_Inv(CYBSP_USER_LED_PORT, CYBSP_USER_LED_NUM);
            Cy_SysLib_Delay(500); // delay in milliseconds
        }
        boot_mode = BOOT_MODE_SAFE;
        mp_printf(&mp_plat_print, "- DEVICE IS IN SAFE BOOT MODE -\n");
    } else {
        boot_mode = BOOT_MODE_NORMAL;
    }

    // Turn off LED after boot mode check
    Cy_GPIO_Clr(CYBSP_USER_LED_PORT, CYBSP_USER_LED_NUM);

    return boot_mode;
}

int main(void) {
    cy_rslt_t result = CY_RSLT_SUCCESS;

    /* Initialize the device and board peripherals. */
    result = cybsp_init();
    if (result != CY_RSLT_SUCCESS) {
        CY_ASSERT(0);
    }

    /* Enable global interrupts */
    __enable_irq();

    /* Initialize retarget-io middleware */
    init_retarget_io();

    xTaskCreate(mpy_task, "MicroPython task", MPY_TASK_STACK_SIZE, NULL, MPY_TASK_PRIORITY, &mpy_task_handle);
    vTaskStartScheduler();

    // Should never get here
    CY_ASSERT(0);
    return 0;
}

// mpy_task runs the entire MicroPython interpreter inside a FreeRTOS task.
// This is required unconditionally because the PSoC Edge firmware is built
// with FreeRTOS and the WiFi stack (cy_wcm/WHD) relies on RTOS primitives.
void mpy_task(void *arg) {
    // One-time initialisation – must be before the soft_reset label.
    #if MICROPY_ENABLE_GC
    gc_init(mpy_gc_heap, mpy_gc_heap + MICROPY_GC_HEAP_SIZE);
    mp_cstack_init_with_top((void *)&arg, MPY_TASK_STACK_SIZE * sizeof(StackType_t));
    #endif

    printf("MPY: time_init\r\n");
    time_init();

soft_reset:
    printf("MPY: machine_rtc_init_all\r\n");
    machine_rtc_init_all();
    printf("MPY: mp_init\r\n");
    mp_init();

    readline_init0();

    #if MICROPY_PY_NETWORK
    printf("MPY: network_init\r\n");
    {
        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            network_init();
            mod_network_init();
            nlr_pop();
        } else {
            // network init raised an exception – print a warning and continue
            // so the REPL still starts even if WiFi hardware is unavailable.
            mp_printf(&mp_plat_print, "Warning: network init failed\n");
            mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(nlr.ret_val));
        }
    }
    printf("MPY: network_init done\r\n");
    #endif

    #if MICROPY_VFS
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_));
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_lib));


    #if MICROPY_VFS_LFS2
    pyexec_frozen_module("_boot.py", false);
    #endif
    #endif

    if (check_boot_mode() == BOOT_MODE_NORMAL) {
        // Execute user scripts.
        int ret = pyexec_file_if_exists("/boot.py");

        if (ret & PYEXEC_FORCED_EXIT) {
            goto soft_reset;
        }

        if (pyexec_mode_kind == PYEXEC_MODE_FRIENDLY_REPL) {
            ret = pyexec_file_if_exists("/main.py");

            if (ret & PYEXEC_FORCED_EXIT) {
                goto soft_reset;
            }
        }
    }

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

    machine_pin_irq_deinit_all();
    machine_hw_i2c_deinit_all();
    machine_pdm_pcm_deinit_all();
    machine_ipc_deinit_all();

    #if MICROPY_PY_NETWORK
    mod_network_deinit();
    network_deinit();
    #endif

    #if MICROPY_ENABLE_GC
    gc_sweep_all();
    #endif
    mp_deinit();

    goto soft_reset;
}

#if MICROPY_ENABLE_GC
void gc_collect(void) {
    gc_collect_start();
    gc_helper_collect_regs_and_stack();
    gc_collect_end();
}
#endif

// Handle uncaught exceptions (should never be reached in a correct C implementation).
void nlr_jump_fail(void *val) {
    for (;;) {
    }
}
