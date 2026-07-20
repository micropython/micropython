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

#include "py/mpconfig.h"
#include "py/ringbuf.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "shared/runtime/interrupt_char.h"
#include "mphalport.h"

#include "bao/platform.h"
#include "hardware/irq.h"
#include "hardware/regs/addressmap.h"
#include "hardware/regs/timer.h"
#include "hardware/regs/udma.h"
#include "hardware/uart.h"

#ifndef MICROPY_HW_STDIN_BUFFER_LEN
#define MICROPY_HW_STDIN_BUFFER_LEN 256
#endif

static uint8_t stdin_ringbuf_array[MICROPY_HW_STDIN_BUFFER_LEN];
ringbuf_t stdin_ringbuf = { stdin_ringbuf_array, sizeof(stdin_ringbuf_array) };

// Per-byte RX_CHAR event for the REPL UART within IRQ_UART (IRQARRAY5).
// In IRQ mode (RX_IRQ_EN=1, RX_POLLING_EN=0) the receiver fires this
// event each time a byte lands in DATA via the 4-deep DC FIFO inside
// udma_uart_top.sv; that FIFO gives the handler ~350us at 115200 baud
// before the receiver back-pressures and ERR fires.
#define REPL_UART_RX_CHAR_EVT  UART2_EVT_RX_CHAR
#define REPL_UART_ERR_EVT      UART2_EVT_ERR

// UART instance base addresses.  Indexed by UART number so this survives
// any future SoC variant that breaks the +0x1000-per-instance stride.
static const uintptr_t uart_bases[] = {
    UDMA_UART0_BASE,
    UDMA_UART1_BASE,
    UDMA_UART2_BASE,
    UDMA_UART3_BASE,
};
#define REPL_UART_BASE         (uart_bases[MICROPY_HW_UART_REPL])

// Silently-dropped-byte counter, incremented by the ERR ISR.  Exposed
// for diagnostics; user code can read it via the platform module once
// that lands.
static volatile uint32_t uart_repl_rx_overflow_count = 0;

// TX output is buffered and flushed as one uart_write so logically
// related writes land in a single TCP segment (the REPL is reached over
// a USB/IP bridge from the host; otherwise Nagle delays the trailing
// short writes).  Buffer state is shared between Python-thread writers
// and stdout_flush; mp_hal_stdout_tx_strn may run from a scheduler
// callback, so writes are wrapped in an atomic section.
#define STDOUT_TXBUF_SIZE 256
static uint8_t stdout_txbuf[STDOUT_TXBUF_SIZE];
static size_t stdout_txbuf_len = 0;

static void stdout_flush(void) {
    if (stdout_txbuf_len > 0) {
        uart_write(MICROPY_HW_UART_REPL, stdout_txbuf, (uint32_t)stdout_txbuf_len);
        stdout_txbuf_len = 0;
    }
}

void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    bool ends_with_newline = (len > 0) && (str[len - 1] == '\n');
    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    while (len > 0) {
        size_t space = STDOUT_TXBUF_SIZE - stdout_txbuf_len;
        if (len >= space) {
            memcpy(stdout_txbuf + stdout_txbuf_len, str, space);
            stdout_txbuf_len = STDOUT_TXBUF_SIZE;
            stdout_flush();
            str += space;
            len -= space;
        } else {
            memcpy(stdout_txbuf + stdout_txbuf_len, str, len);
            stdout_txbuf_len += len;
            break;
        }
    }
    // Flush line-buffered: keeps print() output prompt when the REPL is
    // not blocked on stdin.  Bursts without a trailing newline stay
    // buffered so the USB/IP bridge sees one TCP segment per logical
    // message.
    if (ends_with_newline) {
        stdout_flush();
    }
    MICROPY_END_ATOMIC_SECTION(atomic_state);
}

// mp_hal_stdout_tx_str and mp_hal_stdout_tx_strn_cooked come from
// shared/runtime/stdout_helpers.c -- they're written in terms of the
// strn() above.

int mp_hal_stdin_rx_chr(void) {
    stdout_flush();
    for (;;) {
        int c = ringbuf_get(&stdin_ringbuf);
        if (c != -1) {
            return c;
        }
        // The RX_CHAR ISR feeds stdin_ringbuf; nothing to poll here.
        mp_event_handle_nowait();
    }
}

uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags) {
    uintptr_t ret = 0;
    if ((poll_flags & MP_STREAM_POLL_RD) && ringbuf_avail(&stdin_ringbuf) > 0) {
        ret |= MP_STREAM_POLL_RD;
    }
    if (poll_flags & MP_STREAM_POLL_WR) {
        ret |= MP_STREAM_POLL_WR;
    }
    return ret;
}

// mp_hal_set_interrupt_char + mp_interrupt_char come from
// shared/runtime/interrupt_char.c; we don't define our own.

// UART RX_CHAR interrupt service.  Fires when a byte is presented in
// the DATA register (VALID rising edge).  trap_dispatch cleared
// EV_PENDING before calling us; if more bytes arrived during dispatch
// they sit in the DC FIFO and re-trigger after we drain DATA, so loop
// on VALID to absorb the whole FIFO before returning.  Ctrl-C bytes
// raise a keyboard interrupt directly here (matches rp2/stm32) rather
// than being deferred through a per-bytecode VM hook.
static void uart_repl_irq_handler(uint32_t pending) {
    if (pending & REPL_UART_RX_CHAR_EVT) {
        while (REG32(REPL_UART_BASE + UDMA_UART_VALID_OFFSET) & 1u) {
            uint8_t byte = (uint8_t)(REG32(REPL_UART_BASE + UDMA_UART_DATA_OFFSET) & 0xffu);
            #if MICROPY_KBD_EXCEPTION
            if (byte == mp_interrupt_char) {
                mp_sched_keyboard_interrupt();
                continue;
            }
            #endif
            ringbuf_put(&stdin_ringbuf, byte);
        }
    }
    if (pending & REPL_UART_ERR_EVT) {
        // Reading ERROR clears the latched error bits in the UART.
        (void)REG32(REPL_UART_BASE + UDMA_UART_ERROR_OFFSET);
        uart_repl_rx_overflow_count++;
    }
}

// Called once from main() before mp_init().  Enables the UART2 clock
// gate and configures the UART for IRQ-mode RX with per-byte events.
void mp_hal_uart_repl_init(void) {
    // Pre-enable clock gate so detect_perclk() inside uart_init reads a
    // valid SETUP register; uart_init ORs the same bit.
    REG32(UDMA_CTRL_BASE + UDMA_CTRL_CG_OFFSET) |= UDMA_CG_UART2;
    uart_init(MICROPY_HW_UART_REPL, MICROPY_HW_UART_REPL_BAUD);
    // RX in IRQ mode: clear RX_POLLING_EN (bit 4) in SETUP so the receiver
    // gates rx_char_event_o via rx_irq_en (see udma_uart_top.sv:140).
    // uart_init() sets UART_SETUP_8N1 which has bit 4 set; clear it here.
    REG32(REPL_UART_BASE + UDMA_UART_SETUP_OFFSET) &= ~(1u << 4);
    memory_fence();
    // IRQ_EN: RX_IRQ_EN (bit 0) for per-byte rx_char events, ERR_IRQ_EN
    // (bit 1) for overflow / framing errors.
    REG32(REPL_UART_BASE + UDMA_UART_IRQ_EN_OFFSET) = (1u << 0) | (1u << 1);
    memory_fence();
}

// Called from main() after mp_init() on every soft-reboot iteration.
// Wires the per-byte RX_CHAR event and the ERR event to our handler.
// irq_enable_events() also enables the IRQ line in MIM; a separate
// irq_enable() would override EV_ENABLE to 0xFFFF, enabling every
// event for every UART instance -- specifically not what we want.
void mp_hal_stdin_uart_irq_init(void) {
    irq_init();
    irq_set_handler(IRQ_UART, uart_repl_irq_handler);
    irq_enable_events(IRQ_UART, REPL_UART_RX_CHAR_EVT | REPL_UART_ERR_EVT);
}

// TickTimer driver.  64-bit free-running counter ticking at 1 us
// resolution (CLOCKS_PER_TICK = ACLK / 1e6 = 350).  Initialised once
// from main() before any time-dependent code runs.

void mp_hal_ticktimer_init(void) {
    TICKTIMER_CLOCKS_PER_TICK = ACLK_HZ / 1000000UL;
    memory_fence();
}

static inline uint64_t ticktimer_read_us(void) {
    // Race-safe 64-bit read: TIME1 is the high word, TIME0 is the low
    // word.  If the high word changes between samples the low word
    // could be from either side of the boundary, so we retry.
    uint32_t hi_a, lo, hi_b;
    do {
        hi_a = TICKTIMER_TIME1;
        lo = TICKTIMER_TIME0;
        hi_b = TICKTIMER_TIME1;
    } while (hi_a != hi_b);
    return ((uint64_t)hi_b << 32) | lo;
}

mp_uint_t mp_hal_ticks_us(void) {
    return (mp_uint_t)ticktimer_read_us();
}

mp_uint_t mp_hal_ticks_ms(void) {
    return (mp_uint_t)(ticktimer_read_us() / 1000U);
}

mp_uint_t mp_hal_ticks_cpu(void) {
    uint32_t cycles;
    __asm__ volatile ("csrr %0, mcycle" : "=r" (cycles));
    return (mp_uint_t)cycles;
}

void mp_hal_delay_us(mp_uint_t us) {
    uint64_t start = ticktimer_read_us();
    while ((ticktimer_read_us() - start) < us) {
    }
}

void mp_hal_delay_ms(mp_uint_t ms) {
    uint64_t start = ticktimer_read_us();
    uint64_t target = (uint64_t)ms * 1000U;
    while ((ticktimer_read_us() - start) < target) {
    }
}
