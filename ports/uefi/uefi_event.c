// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Nicko van Someren

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
#include "uefi_serial.h"

// --- stdin ring buffer, filled by the ConIn poll notify (the "RX ISR") ---
// Sizing matters for the raw-REPL raw-paste upload path (the test harness): raw-paste
// lets the host have up to MICROPY_REPL_STDIN_BUFFER_MAX bytes in flight before the
// first flow-control grant (the device advertises a buf_max/2 window and implicitly
// grants two of them). A whole poll's worth can land in the ring in one notify, so the
// ring must be comfortably LARGER than twice that ceiling — 1 KiB vs a 256 B ceiling.
// In serial-REPL mode the REPL reads its serial port's own ring (uefi_serial.c) of the
// same size; that poll stops reading when the ring is full, leaving the bytes in the
// driver (which, with RTS/CTS, back-pressures the sender) rather than dropping them.
#ifndef MICROPY_UEFI_STDIN_BUFFER_LEN
#define MICROPY_UEFI_STDIN_BUFFER_LEN (1024)
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

// The DXE core's low-level periodic tick (EFI_TIMER_ARCH_PROTOCOL) is the granularity of
// every BootServices timer event we use (WFE timeout, machine.Timer). The firmware default
// is coarse (~10 ms on AAVMF), which shows up as up to ~10 ms of sleep/timer jitter. We
// retune it finer while we run, then restore it. 2.5 ms is a deliberate middle ground:
// fine enough that jitter is well under a typical 10 ms tolerance, but 4x coarser than
// 1 ms — bounding the extra interrupt load and, more importantly, avoiding swamping any
// firmware consumer that registered a sub-tick periodic timer with a slow handler.
#define MPY_UEFI_TIMER_PERIOD_100NS (25000ULL) // 2.5 ms
static EFI_TIMER_ARCH_PROTOCOL *timer_arch = NULL;
static UINT64 saved_timer_period = 0; // non-zero once we've changed it (needs restore)

// Byte-clean serial REPL. ConOut/ConIn go through the firmware's TerminalDxe, a
// terminal *emulator* that mangles non-printable bytes (the raw REPL's 0x04 EOT is
// rendered as '?') and eats input escapes — so it can't carry the raw-REPL protocol.
// In serial-REPL mode the REPL is attached to one port of the serial driver layer
// (uefi_serial.c) instead. See main.c and docs/serial-uart-design.md.
bool mp_uefi_serial_repl = false;   // the REPL was configured onto a serial port
static mp_uefi_serial_t *repl_port; // that port (its slot survives unplugging)
static uint32_t repl_baud;          // settings to re-apply when it is re-plugged
static int repl_flow;

// Give up on a REPL write that has made no progress for this long (e.g. CTS held low by
// a far end that went away), rather than hang the interpreter.
#define SERIAL_TX_STALL_MS (2000)

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
    // Serial ports first; only the REPL's port has the interrupt char applied.
    mp_uefi_serial_poll(interrupt_char);
    if (repl_port != NULL && repl_port->owned) {
        return;     // the REPL is on its serial port; ConIn is not REPL input
    }
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

// Attach the REPL to a serial port: `spec` is NULL/"" (first port), a decimal index,
// or text to find in the port's device path. baud 0 keeps the port's current rate.
// Returns true on success; on failure the REPL stays on the ConOut/ConIn console.
bool mp_uefi_serial_repl_attach(const char *spec, uint32_t baud, bool rtscts) {
    mp_uefi_serial_scan();
    int i;
    if (spec == NULL || *spec == 0) {
        i = mp_uefi_serial_find_index(0);
    } else if (*spec >= '0' && *spec <= '9') {
        long n = 0;
        const char *p = spec;
        for (; *p >= '0' && *p <= '9' && n < 1000; p++) {
            n = n * 10 + (*p - '0');
        }
        i = *p == 0 ? mp_uefi_serial_find_index(n) : -1;
    } else {
        i = mp_uefi_serial_find_text(spec);
    }
    if (i < 0) {
        return false;
    }
    mp_uefi_serial_t *s = &mp_uefi_serial_slots[i];
    s->repl = true;     // before open: the REPL's ring is never resized
    if (EFI_ERROR(mp_uefi_serial_open(s, MICROPY_UEFI_STDIN_BUFFER_LEN))) {
        s->repl = false;
        return false;
    }
    if (baud == 0) {
        baud = (s->io->Mode != NULL && s->io->Mode->BaudRate != 0) ? (uint32_t)s->io->Mode->BaudRate : 115200;
    }
    repl_baud = baud;
    repl_flow = rtscts ? -1 : 0;
    if (EFI_ERROR(mp_uefi_serial_configure(s, baud, 8, EFI_SERIAL_NO_PARITY, EFI_SERIAL_ONE_STOP_BIT, repl_flow))) {
        mp_uefi_serial_close(s);
        s->repl = false;
        return false;
    }
    repl_port = s;
    mp_uefi_serial_repl = true;
    return true;
}

// The REPL's port, for reporting (NULL if the REPL isn't on serial).
mp_uefi_serial_t *mp_uefi_serial_repl_port(void) {
    return repl_port;
}

// Foreground: can REPL output go to its serial port right now? If the port was
// unplugged and a SerialIo has since appeared, look for it and re-attach.
bool mp_uefi_serial_repl_ready(void) {
    mp_uefi_serial_t *s = repl_port;
    if (s == NULL) {
        return false;
    }
    if (mp_uefi_serial_usable(s)) {
        return true;
    }
    if (!mp_uefi_serial_rescan_pending()) {
        return false;
    }
    mp_uefi_serial_scan();
    if (!s->present || EFI_ERROR(mp_uefi_serial_open(s, MICROPY_UEFI_STDIN_BUFFER_LEN))) {
        return false;
    }
    if (EFI_ERROR(mp_uefi_serial_configure(s, repl_baud, 8, EFI_SERIAL_NO_PARITY, EFI_SERIAL_ONE_STOP_BIT, repl_flow))) {
        mp_uefi_serial_close(s);
        return false;
    }
    return true;
}

void mp_uefi_serial_close_user(void) {
    mp_uefi_serial_close_all(false);
}

// Hand every serial port back to the firmware (the REPL's last): restore settings and
// reconnect TerminalDxe so the console mirror comes back. Call after the last REPL
// output, with the poll timer stopped.
void mp_uefi_serial_deinit(void) {
    mp_uefi_serial_close_all(true);
    repl_port = NULL;
    mp_uefi_serial_repl = false;
}

// Byte-clean REPL output — the raw bytes go straight out, so control codes (the raw
// REPL's 0x04 EOT, etc.) survive. Used by mp_hal_stdout_tx_strn when
// mp_uefi_serial_repl_ready(). Gives up after SERIAL_TX_STALL_MS without progress.
void mp_uefi_serial_tx(const char *str, size_t len) {
    EFI_STATUS err;
    mp_uefi_serial_write(repl_port, (const uint8_t *)str, len, SERIAL_TX_STALL_MS, &err);
}

// --- atomic section: raise to TPL_HIGH_LEVEL, masking our notifies ---
uintptr_t mp_uefi_begin_atomic(void) {
    return (uintptr_t)mp_uefi_st->BootServices->RaiseTPL(TPL_HIGH_LEVEL);
}

void mp_uefi_end_atomic(uintptr_t old_tpl) {
    mp_uefi_st->BootServices->RestoreTPL((EFI_TPL)old_tpl);
}

// --- timer primitives (the UEFI-event bridge) ---
// Reused now by machine.Timer and later by uefi.Timer / uefi.Event. The notify runs
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

// --- signal-event primitive (uefi.Event, protocol completion tokens) ---
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
        int c = repl_port != NULL ? ringbuf_get(&repl_port->rx) : -1;
        if (c < 0) {
            c = ringbuf_get(&stdin_ringbuf);
        }
        if (c >= 0) {
            return c;
        }
        if (repl_port != NULL && mp_uefi_serial_rescan_pending()) {
            mp_uefi_serial_repl_ready();    // the REPL's port may be back: re-attach
        }
        mp_event_wait_indefinite(); // sleeps in WFE; the notify wakes us
    }
}

uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags) {
    uintptr_t ret = 0;
    bool rx_ready = ringbuf_avail(&stdin_ringbuf) > 0
        || (repl_port != NULL && ringbuf_avail(&repl_port->rx) > 0);
    if ((poll_flags & MP_STREAM_POLL_RD) && rx_ready) {
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
    // Seed the wall-clock anchor now that the counter is calibrated (RuntimeServices
    // is always available); every later wall-time read interpolates from here.
    mp_uefi_time_anchor();

    // Plain waitable event the producers signal to break a WFE.
    bs->CreateEvent(0, TPL_APPLICATION, NULL, NULL, &wakeup_event);
    // One-shot timeout timer for bounded waits (waited on, no notify).
    bs->CreateEvent(EVT_TIMER, TPL_APPLICATION, NULL, NULL, &wfe_timer);
    // Periodic ConIn poll: fires the "RX ISR" notify at TPL_CALLBACK every 5 ms, so keys
    // are drained (and Ctrl-C delivered) even while Python code runs. 5 ms (vs the old
    // 10 ms) halves how much the host can pump into QEMU's UART FIFO between drains, which
    // — together with the enlarged stdin ring — keeps the raw-paste upload path from ever
    // overrunning. Cheap on the GHz-class machines this targets; well within the timer's
    // 2.5 ms tick resolution.
    bs->CreateEvent(EVT_TIMER | EVT_NOTIFY_SIGNAL, TPL_CALLBACK, conin_notify, NULL, &conin_timer);
    bs->SetTimer(conin_timer, TimerPeriodic, 50000ULL); // 5 ms in 100 ns units
    // Notice serial ports appearing (USB hot-plug), for re-attach and new UART ids.
    mp_uefi_serial_events_init();

    // Best-effort: retune the DXE-core timer tick finer (see MPY_UEFI_TIMER_PERIOD_100NS).
    // Only ever go finer than the current period — never coarsen a firmware that is already
    // fine — and remember the old value so mp_uefi_async_deinit can restore it.
    static const EFI_GUID g_timer_arch = EFI_TIMER_ARCH_PROTOCOL_GUID;
    if (!EFI_ERROR(bs->LocateProtocol((void *)&g_timer_arch, NULL, (void **)&timer_arch))
        && timer_arch != NULL) {
        UINT64 cur = 0;
        if (!EFI_ERROR(timer_arch->GetTimerPeriod(timer_arch, &cur))
            && cur > MPY_UEFI_TIMER_PERIOD_100NS
            && !EFI_ERROR(timer_arch->SetTimerPeriod(timer_arch, MPY_UEFI_TIMER_PERIOD_100NS))) {
            saved_timer_period = cur;
        }
    }
}

// --- teardown, called before mp_deinit() ---
// Stop the ConIn poll timer and close the wait events so no notify fires (and no
// WFE is attempted) once the interpreter is being torn down.
void mp_uefi_async_deinit(void) {
    EFI_BOOT_SERVICES *bs = mp_uefi_st->BootServices;
    // Restore the firmware's original timer tick before we hand control back to the boot
    // manager / shell, which expects its clock unchanged.
    if (timer_arch != NULL && saved_timer_period != 0) {
        timer_arch->SetTimerPeriod(timer_arch, saved_timer_period);
        saved_timer_period = 0;
    }
    mp_uefi_serial_events_deinit();
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
