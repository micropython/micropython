/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 MicroPython contributors
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

#include <string.h>

#include "py/builtin.h"
#include "py/compile.h"
#include "py/gc.h"
#include "py/lexer.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "py/stackctrl.h"
#include "shared/runtime/gchelper.h"
#include "shared/runtime/pyexec.h"

#include "hardware/irq.h"
#include "hardware/uart.h"
#include "hardware/regs/addressmap.h"
#include "hardware/regs/udma.h"
#include "bao/platform.h"

#include "machine_pin.h"
#include "mphalport.h"

// Heap and stack bounds defined by the linker script.  _stack_size is
// the stack reservation size (an absolute value, not an address), so it
// has to be referenced via &_stack_size and cast through uintptr_t.
extern uint32_t _heap_start;
extern uint32_t _heap_end;
extern uint32_t _stack_top;
extern uint32_t _stack_size;

#ifndef MICROPY_HW_UART_REPL
#define MICROPY_HW_UART_REPL        (2)
#endif
#ifndef MICROPY_HW_UART_REPL_BAUD
#define MICROPY_HW_UART_REPL_BAUD   (115200)
#endif

// Stack guard size matches the linker's reservation minus a small
// safety margin so MP_STACK_CHECK() trips before we hit the heap.
#define STACK_GUARD_BYTES       (1024)

#if MICROPY_DEBUG_VERBOSE
static void trace(const char *msg) {
    uart_write(MICROPY_HW_UART_REPL, (const uint8_t *)msg, strlen(msg));
}
#else
#define trace(msg) ((void)0)
#endif

int main(void) {
    // Pre-enable UART2 clock gate so detect_perclk() (called from inside
    // uart_init) reads a valid SETUP register; uart_init ORs the same bit.
    REG32(UDMA_CTRL_BASE + UDMA_CTRL_CG_OFFSET) |= UDMA_CG_UART2;

    // Configure UART2 (PB14 = TX, PB13 = RX) at 115200 8N1.  boot0/boot1
    // left it at 1 Mbaud; this retunes for user-firmware logging.
    uart_init(MICROPY_HW_UART_REPL, MICROPY_HW_UART_REPL_BAUD);

    // Abort any stale UDMA TX/RX from boot0/boot1 with the CLR bit, then
    // write 0 to guarantee EN=0 regardless of CLR self-clear timing.
    REG32(UDMA_UART2_BASE + UDMA_TX_CFG_OFFSET) = UDMA_CFG_CLR;
    memory_fence();
    REG32(UDMA_UART2_BASE + UDMA_TX_CFG_OFFSET) = 0u;
    memory_fence();
    REG32(UDMA_UART2_BASE + UDMA_RX_CFG_OFFSET) = UDMA_CFG_CLR;
    memory_fence();
    REG32(UDMA_UART2_BASE + UDMA_RX_CFG_OFFSET) = 0u;
    memory_fence();

    // Enable the per-character RX interrupt at the UART level.  Without
    // this UART_IRQ_EN write, UART_VALID is never set even in poll mode
    // (SETUP bit 4 = 1).
    REG32(UDMA_UART2_BASE + UDMA_UART_IRQ_EN_OFFSET) = 0xFFFFFFFFu;

    trace("TRACE:uart_ok\r\n");

    mp_hal_ticktimer_init();
    trace("TRACE:tick_ok\r\n");

    // MicroPython stack bounds; the heap is set up per soft-reset
    // iteration so Ctrl-D properly clears all dynamic state.
    mp_stack_set_top(&_stack_top);
    mp_stack_set_limit((size_t)(uintptr_t)&_stack_size - STACK_GUARD_BYTES);

    // Outer loop: each iteration is one full MicroPython session.
    // pyexec_friendly_repl returns non-zero on Ctrl-D, at which point
    // we tear down, re-init, and re-enter -- matching the soft-reset
    // semantics every other MicroPython port provides.
    for (;;) {
        // Per-soft-reset hardware teardown.  Currently only the pin OD
        // emulation state needs clearing; as drivers land (I2C, SPI, PWM,
        // ...), their deinit hooks belong here.
        memset(machine_pin_open_drain_mask, 0, sizeof(machine_pin_open_drain_mask));

        trace("TRACE:gc_init\r\n");
        gc_init(&_heap_start, &_heap_end);
        trace("TRACE:mp_init\r\n");
        mp_init();
        trace("TRACE:irq_init\r\n");

        // UART RX IRQ feeds stdin_ringbuf; the handler may call
        // mp_sched_keyboard_interrupt() on a Ctrl-C byte, which writes
        // into MicroPython scheduler state, so this must run AFTER
        // mp_init() has initialised that state.  Setting up once per
        // session iteration is harmless -- irq_init clears the handler
        // table and re-enables MIE/MEIE each time.
        mp_hal_stdin_uart_irq_init();
        trace("TRACE:repl\r\n");

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

        mp_printf(MP_PYTHON_PRINTER, "MPY: soft reboot\n");
        irq_disable(IRQ_UART);
        mp_deinit();
    }
}

void gc_collect(void) {
    gc_collect_start();
    gc_helper_collect_regs_and_stack();
    gc_collect_end();
}

void nlr_jump_fail(void *val) {
    (void)val;
    for (;;) {
    }
}

// Filesystem stubs (no VFS yet in Phase 1).
mp_import_stat_t mp_import_stat(const char *path) {
    (void)path;
    return MP_IMPORT_STAT_NO_EXIST;
}

mp_lexer_t *mp_lexer_new_from_file(qstr filename) {
    (void)filename;
    mp_raise_OSError(MP_ENOENT);
}

mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    (void)n_args;
    (void)args;
    (void)kwargs;
    mp_raise_OSError(MP_ENOENT);
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);
