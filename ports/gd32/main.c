/*
 * GD32F4xx MicroPython port - main entry
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "py/builtin.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/cstack.h"
#include "py/pystack.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "shared/runtime/pyexec.h"
#include "shared/readline/readline.h"
#include "extmod/vfs.h"
#include "extmod/vfs_lfs.h"

#include "gd32f4xx.h"
#include "board.h"

void machine_mem_backup_init(void);
#include "mphalport.h"

// memory
static char *stack_top;
static char heap[MICROPY_HEAP_SIZE];

// Buffer for MicroPython's Python-level stack (push/pop of VM frames).
// mp_state_ctx is in .bss, so without mp_pystack_init() the start/end
// pointers are NULL and the first allocation reports "pystack exhausted".
#if MICROPY_ENABLE_PYSTACK
static uint8_t pystack[MICROPY_PYSTACK_SIZE];
#endif

extern uint32_t SystemCoreClock;

// Provided by the linker script / Makefile:  _sp is the top of the
// C stack, __stack_size its size (see LDFLAGS --defsym).
extern uint8_t _sp;
extern uint8_t __stack_size;
extern void uart_repl_init(void);

// board init hooks (weak defaults)
MP_WEAK void board_early_init(void) {
}

MP_WEAK void board_init(void) {
}

// GC
void gc_collect(void) {
    void *dummy;
    gc_collect_start();
    gc_collect_root(&dummy, ((mp_uint_t)stack_top - (mp_uint_t)&dummy) / sizeof(mp_uint_t));
    gc_collect_end();
}

// filesystem support: mount littlefs2 on internal flash at "/"
extern mp_obj_t pyb_flash_get_block_device(void);

static void mount_internal_fs(void) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_obj_t bdev = pyb_flash_get_block_device();
        if (mp_vfs_mount_and_chdir_protected(bdev, MP_OBJ_NEW_QSTR(MP_QSTR__slash_)) != 0) {
            // mount failed (e.g. unformatted); root stays empty
        }
        nlr_pop();
    } else {
        // swallow mount errors so the REPL still starts
    }
}

void NORETURN __fatal_error(const char *msg) {
    (void)msg;
    for (;;) {
        ;
    }
}

void nlr_jump_fail(void *val) {
    (void)val;
    __fatal_error("FATAL: nlr_jump_fail");
}

int main(void) {
    int stack_dummy;
    stack_top = (char *)&stack_dummy;

    // enable DWT cycle counter for microsecond delays
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    // SysTick at 1kHz
    SysTick_Config(SystemCoreClock / 1000);

    board_early_init();
    uart_repl_init();

    mp_hal_stdout_tx_str("\r\n");
    mp_hal_stdout_tx_str(MICROPY_HW_BOARD_NAME);
    mp_hal_stdout_tx_str(" with ");
    mp_hal_stdout_tx_str(MICROPY_HW_MCU_NAME);
    mp_hal_stdout_tx_str("\r\n");

    // Tell MicroPython where the C stack is and how big, so that
    // mp_cstack_check() can convert an imminent overflow into a Python
    // RecursionError instead of a hard fault.  Without this the stack limit
    // stays 0 (BSS) and every stack check fires immediately.
    mp_cstack_init_with_top(&_sp, (size_t)&__stack_size);

    // Give MicroPython a real Python stack.  Without this the start/end
    // pointers stay NULL (they live in .bss) and the first allocation
    // raises "pystack exhausted".
    #if MICROPY_ENABLE_PYSTACK
    mp_pystack_init(pystack, pystack + sizeof(pystack));
    #endif

    gc_init(heap, heap + sizeof(heap));
    mp_init();

    mount_internal_fs();

    // Run the frozen _boot.py (if present) so user boot code can run before
    // the REPL appears.  This matches what ports/stm32 and ports/samd do.
    #if MICROPY_MODULE_FROZEN
    pyexec_frozen_module("_boot.py", false);
    #endif

    board_init();

    // Enable write access to the RTC backup domain for machine.mem_backup().
    machine_mem_backup_init();

    #if MICROPY_REPL_EVENT_DRIVEN
    pyexec_event_repl_init();
    for (;;) {
        int c = mp_hal_stdin_rx_chr();
        if (pyexec_event_repl_process_char(c)) {
            break;
        }
    }
    #else
    pyexec_friendly_repl();
    #endif

    mp_deinit();
    return 0;
}
