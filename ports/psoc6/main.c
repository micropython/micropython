
// std includes
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>


// MTB includes
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "cyhal.h"

// FreeRTOS header file
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>


// micropython includes
#include "genhdr/mpversion.h"
#include "py/gc.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "py/stackctrl.h"
#include "shared/readline/readline.h"
#include "shared/runtime/pyexec.h"
#include "extmod/modnetwork.h"

#include "modules/machine/machine_pin_phy.h"

#if MICROPY_PY_NETWORK
#include "cybsp_wifi.h"
#include "cy_wcm.h"
#endif

#if MICROPY_PY_LWIP
#include "lwip/init.h"
#include "lwip/apps/mdns.h"
#endif

// port-specific includes
#include "mplogger.h"

#define MPY_TASK_STACK_SIZE                    (4096u)
#define MPY_TASK_PRIORITY                      (3u)

#if MICROPY_ENABLE_GC
extern uint8_t __StackTop, __StackLimit;
__attribute__((section(".bss"))) static char gc_heap[MICROPY_GC_HEAP_SIZE];
#endif

extern void rtc_init(void);
extern void time_init(void);
extern void os_init(void);
extern void machine_init(void);
extern void machine_deinit(void);
extern void network_init(void);
extern void network_deinit(void);
extern void mod_pin_deinit(void);
extern void mod_adc_block_deinit(void);
extern void mod_i2c_deinit(void);
extern void mod_pwm_deinit(void);
extern void mod_spi_deinit(void);

void mpy_task(void *arg);

TaskHandle_t mpy_task_handle;

int main(int argc, char **argv) {
    // Initialize the device and board peripherals
    cy_rslt_t result = cybsp_init();
    if (result != CY_RSLT_SUCCESS) {
        mp_raise_ValueError(MP_ERROR_TEXT("cybsp_init failed !\n"));
    }

    // Initialize retarget-io to use the debug UART port
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
    if (result != CY_RSLT_SUCCESS) {
        mp_raise_ValueError(MP_ERROR_TEXT("cy_retarget_io_init failed !\n"));
    }

    xTaskCreate(mpy_task, "MicroPython task", MPY_TASK_STACK_SIZE, NULL, MPY_TASK_PRIORITY, &mpy_task_handle);
    vTaskStartScheduler();

    // Should never get here
    CY_ASSERT(0);
    return 0;
}

void mpy_task(void *arg) {
    #if MICROPY_ENABLE_GC
    mp_stack_set_top(&__StackTop);
    // mp_stack_set_limit((mp_uint_t)&__StackTop - (mp_uint_t)&__StackLimit);
    mp_stack_set_limit((mp_uint_t)&__StackLimit);
    gc_init(&gc_heap[0], &gc_heap[MP_ARRAY_SIZE(gc_heap)]);
    #endif

    #ifdef SIGPIPE
    // Do not raise SIGPIPE, instead return EPIPE. Otherwise, e.g. writing
    // to peer-closed socket will lead to sudden termination of MicroPython
    // process. SIGPIPE is particularly nasty, because unix shell doesn't
    // print anything for it, so the above looks like completely sudden and
    // silent termination for unknown reason. Ignoring SIGPIPE is also what
    // CPython does. Note that this may lead to problems using MicroPython
    // scripts as pipe filters, but again, that's what CPython does. So,
    // scripts which want to follow unix shell pipe semantics (where SIGPIPE
    // means "pipe was requested to terminate, it's not an error"), should
    // catch EPIPE themselves.
    signal(SIGPIPE, SIG_IGN);
    #endif

    // Initialize modules. Or to be redone after a reset and therefore to be placed next to machine_init below ?
    os_init();
    rtc_init();
    time_init();


soft_reset:

    mp_init();

    // ANSI ESC sequence for clear screen. Refer to  https://stackoverflow.com/questions/517970/how-to-clear-the-interpreter-console
    mp_printf(&mp_plat_print, "\033[H\033[2J");

    mp_printf(&mp_plat_print, MICROPY_BANNER_NAME_AND_VERSION);
    mp_printf(&mp_plat_print, "; " MICROPY_BANNER_MACHINE);
    mp_printf(&mp_plat_print, "\nUse Ctrl-D to exit, Ctrl-E for paste mode\n");

    // indicate in REPL console when debug mode is selected
    mplogger_print("\n...LOGGER DEBUG MODE...\n\n");

    readline_init0();
    machine_init();
    #if MICROPY_PY_NETWORK
    network_init();
    mod_network_init();
    #endif

    #if MICROPY_VFS
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_));
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_lib));
    #if MICROPY_VFS_FAT
    pyexec_frozen_module("vfs_fat.py", false);
    #elif MICROPY_VFS_LFS2
    pyexec_frozen_module("vfs_lfs2.py", false);
    #endif

    #endif

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

    __enable_irq();

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

    // Deinitialize modules
    machine_deinit();
    mod_pin_deinit();
    mod_adc_block_deinit();
    mod_i2c_deinit();
    mod_pwm_deinit();
    mod_spi_deinit();
    mod_pin_phy_deinit();
    #if MICROPY_PY_NETWORK
    mod_network_deinit();
    network_deinit();
    #endif

    // gc_sweep_all();
    mp_deinit();

    goto soft_reset;
}

// TODO: to be implemented
void nlr_jump_fail(void *val) {
    mplogger_print("nlr_jump_fail\n");

    mp_printf(&mp_plat_print, "FATAL: uncaught exception %p\n", val);
    mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(val));

    for (;;) {
        __BKPT(0);
    }
}
