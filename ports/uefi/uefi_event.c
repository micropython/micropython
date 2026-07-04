/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Nicko van Someren
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

// UEFI event framework: the single blocking primitive (mp_uefi_wfe), the
// TPL-based atomic section, and the ConIn "RX interrupt" that feeds stdin.
//
// Concurrency model (see README.md):
//   * A UEFI event notify runs at raised TPL and preempts the interpreter — it
//     is a hard ISR. It must touch only plain memory, the stdin ringbuf, and the
//     async-safe scheduler entry points (mp_sched_*). Never the heap/VM/GC.
//   * MICROPY_BEGIN/END_ATOMIC_SECTION == RaiseTPL(HIGH_LEVEL)/RestoreTPL, which
//     masks our notifies so scheduler/ringbuf state is consistent.
//   * Everything that blocks (REPL input, time.sleep, select.poll, asyncio) ends
//     up in mp_uefi_wfe, which parks in BootServices->WaitForEvent.

#include "py/mphal.h"
#include "py/runtime.h"
#include "py/ringbuf.h"
#include "py/stream.h"
#include "shared/runtime/pyexec.h"
#include "efi.h"
#include "uefi_port.h"

// --- stdin ring buffer, filled by the ConIn poll notify (the "RX ISR") ---
#ifndef MICROPY_UEFI_STDIN_BUFFER_LEN
#define MICROPY_UEFI_STDIN_BUFFER_LEN (256)
#endif
static uint8_t stdin_ringbuf_array[MICROPY_UEFI_STDIN_BUFFER_LEN];
static ringbuf_t stdin_ringbuf = {stdin_ringbuf_array, sizeof(stdin_ringbuf_array)};

// Interrupt character (Ctrl-C when the REPL enables it; -1 = disabled). When the
// notify sees it, it raises a pending KeyboardInterrupt instead of buffering it.
static int interrupt_char = -1;

// Events. wakeup: plain waitable event the producers signal to break a WFE.
// wfe_timer: one-shot timeout for a bounded WFE. conin_timer: periodic timer
// whose notify polls ConIn (our software "RX interrupt").
static EFI_EVENT wakeup_event;
static EFI_EVENT wfe_timer;
static EFI_EVENT conin_timer;

// Map a UEFI scan-only key (UnicodeChar == 0) to the byte sequence readline
// understands. We re-emit the CSI escape sequences (the firmware parsed them out
// of the terminal into scan codes); readline's escape parser handles arrows/home/
// end inline, so this needs no MICROPY_REPL_EMACS_KEYS and avoids the empty-line
// control-char shortcuts (e.g. Ctrl-A would enter the raw REPL). Returns NULL to
// ignore the key.
static const char *scan_to_seq(uint16_t scan) {
    switch (scan) {
        case SCAN_UP:
            return "\x1b[A";               // up: previous history line
        case SCAN_DOWN:
            return "\x1b[B";               // down: next history line
        case SCAN_RIGHT:
            return "\x1b[C";               // cursor right
        case SCAN_LEFT:
            return "\x1b[D";               // cursor left
        case SCAN_HOME:
            return "\x1b[H";               // start of line
        case SCAN_END:
            return "\x1b[F";               // end of line
        // The firmware maps both a 0x7f-backspace and the forward-delete key to
        // SCAN_DELETE, so they're indistinguishable; treat it as backspace (DEL),
        // which is what is overwhelmingly pressed.
        case SCAN_DELETE:
            return "\x7f";
        default:
            return NULL;                   // F-keys, PgUp/Dn, Insert, ...: ignore
    }
}

// --- the ConIn poll notify: hard-ISR context, TPL_CALLBACK ---
static void EFIAPI conin_notify(EFI_EVENT event, void *context) {
    (void)event;
    (void)context;
    EFI_SIMPLE_TEXT_INPUT_PROTOCOL *in = mp_uefi_st->ConIn;
    EFI_INPUT_KEY key;
    bool buffered = false;
    while (in->ReadKeyStroke(in, &key) == EFI_SUCCESS) {
        if (key.UnicodeChar != 0) {
            // Printable / editing char (Enter, Backspace, Ctrl-*): one byte.
            int c = key.UnicodeChar;
            if (c == interrupt_char) {
                #if MICROPY_KBD_EXCEPTION
                mp_sched_keyboard_interrupt();
                #endif
                continue; // don't buffer the interrupt char itself
            }
            ringbuf_put(&stdin_ringbuf, (uint8_t)c); // drops the byte if full
            buffered = true;
            continue;
        }
        // Scan-only key (arrows, Home/End, Delete, ...): inject readline's bytes.
        const char *seq = scan_to_seq(key.ScanCode);
        if (seq != NULL) {
            for (; *seq; seq++) {
                ringbuf_put(&stdin_ringbuf, (uint8_t)*seq);
            }
            buffered = true;
        }
    }
    if (buffered) {
        mp_uefi_st->BootServices->SignalEvent(wakeup_event);
    }
}

// --- atomic section: raise to TPL_HIGH_LEVEL, masking our notifies ---
uintptr_t mp_uefi_begin_atomic(void) {
    return (uintptr_t)mp_uefi_st->BootServices->RaiseTPL(TPL_HIGH_LEVEL);
}

void mp_uefi_end_atomic(uintptr_t old_tpl) {
    mp_uefi_st->BootServices->RestoreTPL((EFI_TPL)old_tpl);
}

// --- timer primitives (the UEFI-event bridge) ---
// Reused now by machine.Timer and later by efi.Timer / efi.Event. The notify runs
// at TPL_CALLBACK and is hard-ISR context: it must only schedule work (e.g.
// mp_sched_schedule), never touch the VM/heap directly.
EFI_STATUS mp_uefi_create_timer(EFI_EVENT_NOTIFY notify, void *context, EFI_EVENT *out) {
    return mp_uefi_st->BootServices->CreateEvent(
        EVT_TIMER | EVT_NOTIFY_SIGNAL, TPL_CALLBACK, notify, context, out);
}

void mp_uefi_arm_timer(EFI_EVENT ev, int periodic, uint64_t period_us) {
    mp_uefi_st->BootServices->SetTimer(
        ev, periodic ? TimerPeriodic : TimerRelative, period_us * 10ULL); // us -> 100ns
}

void mp_uefi_cancel_timer(EFI_EVENT ev) {
    mp_uefi_st->BootServices->SetTimer(ev, TimerCancel, 0);
}

void mp_uefi_close_event(EFI_EVENT ev) {
    mp_uefi_st->BootServices->CloseEvent(ev);
}

// Wake a foreground blocked in WaitForEvent: signal the wakeup event so it returns
// and re-polls. Any notify that changes poll-readiness (without scheduling a Python
// callback) calls this. SignalEvent on a plain (no-notify) event is TPL-safe.
void mp_uefi_wake(void) {
    if (wakeup_event != NULL) {
        mp_uefi_st->BootServices->SignalEvent(wakeup_event);
    }
}

// MICROPY_SCHED_HOOK_SCHEDULED: called (inside the scheduler's atomic section)
// whenever work is scheduled — including from a notify — so a scheduled callback
// wakes a blocked foreground.
void mp_uefi_sched_signal(void) {
    mp_uefi_wake();
}

// --- signal-event primitive (efi.Event, protocol completion tokens) ---
// A UEFI event whose signalling runs `notify` (hard-ISR, TPL_CALLBACK). Unlike a
// waitable event it can't be passed to WaitForEvent, but the notify funnels into
// the wakeup loop — the uniform model for "an external thing signalled us".
EFI_STATUS mp_uefi_create_signal_event(EFI_EVENT_NOTIFY notify, void *context, EFI_EVENT *out) {
    return mp_uefi_st->BootServices->CreateEvent(EVT_NOTIFY_SIGNAL, TPL_CALLBACK, notify, context, out);
}

void mp_uefi_signal_event(EFI_EVENT ev) {
    mp_uefi_st->BootServices->SignalEvent(ev);
}

// --- the single blocking primitive ---
// Park until a producer signals the wakeup event or the timeout elapses. Must be
// called at TPL_APPLICATION (the VM/main context); never from a notify.
void mp_uefi_wfe(int64_t timeout_ms) {
    // Don't sleep while scheduled callbacks are still pending: mp_handle_pending
    // runs at most one per call, so let the caller loop and drain the rest before
    // we actually block (avoids one-callback-per-wakeup latency).
    if (MP_STATE_VM(sched_state) == MP_SCHED_PENDING) {
        return;
    }
    if (wakeup_event == NULL) {
        return; // async machinery torn down (shutdown) — nothing to wait on
    }
    EFI_BOOT_SERVICES *bs = mp_uefi_st->BootServices;
    EFI_EVENT events[2];
    UINTN n = 0;
    events[n++] = wakeup_event;
    if (timeout_ms >= 0) {
        bs->SetTimer(wfe_timer, TimerRelative, (UINT64)timeout_ms * 10000ULL); // ms -> 100ns
        events[n++] = wfe_timer;
    }
    UINTN index;
    bs->WaitForEvent(n, events, &index);
    // Clear signalled state so the next wait blocks again. The ringbuf — not the
    // event — is the source of truth, so a spuriously cleared signal is harmless.
    bs->CheckEvent(wakeup_event);
    if (timeout_ms >= 0) {
        bs->SetTimer(wfe_timer, TimerCancel, 0);
        bs->CheckEvent(wfe_timer);
    }
}

// --- HAL stdin, backed by the ringbuf ---
int mp_hal_stdin_rx_chr(void) {
    for (;;) {
        int c = ringbuf_get(&stdin_ringbuf);
        if (c >= 0) {
            return c;
        }
        mp_event_wait_indefinite(); // sleeps in WFE; the notify wakes us
    }
}

uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags) {
    uintptr_t ret = 0;
    if ((poll_flags & MP_STREAM_POLL_RD) && ringbuf_avail(&stdin_ringbuf) > 0) {
        ret |= MP_STREAM_POLL_RD;
    }
    if (poll_flags & MP_STREAM_POLL_WR) {
        ret |= MP_STREAM_POLL_WR; // ConOut is always ready to accept output
    }
    return ret;
}

void mp_hal_set_interrupt_char(int c) {
    interrupt_char = c;
}

// --- one-time setup, called after mp_init() ---
void mp_uefi_async_init(void) {
    EFI_BOOT_SERVICES *bs = mp_uefi_st->BootServices;

    mp_uefi_time_init();

    // Plain waitable event the producers signal to break a WFE.
    bs->CreateEvent(0, TPL_APPLICATION, NULL, NULL, &wakeup_event);
    // One-shot timeout timer for bounded waits (waited on, no notify).
    bs->CreateEvent(EVT_TIMER, TPL_APPLICATION, NULL, NULL, &wfe_timer);
    // Periodic ConIn poll: fires the "RX ISR" notify at TPL_CALLBACK every 10 ms,
    // so keys are drained (and Ctrl-C is delivered) even while Python code runs.
    bs->CreateEvent(EVT_TIMER | EVT_NOTIFY_SIGNAL, TPL_CALLBACK, conin_notify, NULL, &conin_timer);
    bs->SetTimer(conin_timer, TimerPeriodic, 100000ULL); // 10 ms in 100 ns units
}

// --- teardown, called before mp_deinit() ---
// Stop the ConIn poll timer and close the wait events so no notify fires (and no
// WFE is attempted) once the interpreter is being torn down.
void mp_uefi_async_deinit(void) {
    EFI_BOOT_SERVICES *bs = mp_uefi_st->BootServices;
    if (conin_timer != NULL) {
        bs->SetTimer(conin_timer, TimerCancel, 0);
        bs->CloseEvent(conin_timer);
        conin_timer = NULL;
    }
    if (wfe_timer != NULL) {
        bs->CloseEvent(wfe_timer);
        wfe_timer = NULL;
    }
    if (wakeup_event != NULL) {
        bs->CloseEvent(wakeup_event);
        wakeup_event = NULL; // makes mp_uefi_wfe a no-op from here on
    }
}
