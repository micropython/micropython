// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Nicko van Someren

// Serial port driver layer over EFI_SERIAL_IO_PROTOCOL: port identity across hot-plug,
// taking a port from TerminalDxe and giving it back, settings, background RX and the
// TX loop. Used by the REPL glue (uefi_event.c) and machine.UART (machine_uart.c).
// Design: docs/serial-uart-design.md.
//
// Concurrency: mp_uefi_serial_poll() runs from the 5 ms timer notify at TPL_CALLBACK;
// everything else runs in the foreground. Each RX ring has one producer (the timer, or
// for a background-filled non-REPL port the foreground) and one consumer. Slot-table
// changes raise to TPL_CALLBACK so the timer never sees a half-built slot. Write is
// never called at raised TPL: drivers (ftdi-uefi) must keep their own poll timer
// running while a write waits on CTS.

#include <string.h>

#include "py/mphal.h"
#include "py/runtime.h"
#include "uefi_port.h"
#include "uefi_serial.h"

#define SETTABLE_CONTROL (EFI_SERIAL_DATA_TERMINAL_READY | EFI_SERIAL_REQUEST_TO_SEND | \
    EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE)
#define RX_CHUNK (256)      // most bytes moved per Read
#define DP_MAX (1024)       // longest device path we'll walk

static const EFI_GUID g_serial = EFI_SERIAL_IO_PROTOCOL_GUID;
static const EFI_GUID g_devpath = EFI_DEVICE_PATH_PROTOCOL_GUID;
static const EFI_GUID g_dp2text = EFI_DEVICE_PATH_TO_TEXT_PROTOCOL_GUID;

mp_uefi_serial_t mp_uefi_serial_slots[MP_UEFI_SERIAL_MAX];
size_t mp_uefi_serial_nslots;

static volatile bool rescan_pending;
static EFI_EVENT arrival_event;
static void *arrival_reg;

#define BS (mp_uefi_st->BootServices)

// --- identity ---

// Length of the device path up to (not including) its first Uart node, or of the whole
// path if it has none. Drivers rewrite the Uart node when settings change, so it can't
// be part of a port's identity.
static size_t ident_len(const uint8_t *dp) {
    size_t off = 0;
    while (off + 4 <= DP_MAX) {
        uint8_t type = dp[off], sub = dp[off + 1];
        size_t len = dp[off + 2] | (dp[off + 3] << 8);
        if ((type == 0x7f && sub == 0xff) || (type == 0x03 && sub == 0x0e) || len < 4) {
            break;
        }
        off += len;
    }
    return off;
}

static void *pool_alloc(size_t n) {
    void *p = NULL;
    if (EFI_ERROR(BS->AllocatePool(EfiBootServicesData, n, &p))) {
        return NULL;
    }
    return p;
}

// --- validity ---

bool mp_uefi_serial_check(mp_uefi_serial_t *s) {
    if (!s->present) {
        return false;
    }
    void *iface = NULL;
    if (EFI_ERROR(BS->OpenProtocol(s->handle, (EFI_GUID *)&g_serial, &iface, mp_uefi_image, NULL,
        EFI_OPEN_PROTOCOL_GET_PROTOCOL)) || iface != s->io) {
        // Unplugged (or replaced): never touch s->io again. Wake waiters so they see it.
        s->present = false;
        s->owned = false;
        mp_uefi_wake();
        return false;
    }
    return true;
}

// --- slot table ---

bool mp_uefi_serial_rescan_pending(void) {
    return rescan_pending;
}

void mp_uefi_serial_scan(void) {
    rescan_pending = false;
    EFI_TPL old = BS->RaiseTPL(TPL_CALLBACK);
    for (size_t i = 0; i < mp_uefi_serial_nslots; i++) {
        mp_uefi_serial_check(&mp_uefi_serial_slots[i]);
    }
    UINTN count = 0;
    EFI_HANDLE *hb = NULL;
    if (!EFI_ERROR(BS->LocateHandleBuffer(ByProtocol, (EFI_GUID *)&g_serial, NULL, &count, &hb)) && hb != NULL) {
        for (UINTN k = 0; k < count; k++) {
            EFI_SERIAL_IO_PROTOCOL *io = NULL;
            if (EFI_ERROR(BS->HandleProtocol(hb[k], (EFI_GUID *)&g_serial, (void **)&io)) || io == NULL) {
                continue;
            }
            bool tracked = false;
            for (size_t i = 0; i < mp_uefi_serial_nslots && !tracked; i++) {
                tracked = mp_uefi_serial_slots[i].present && mp_uefi_serial_slots[i].handle == hb[k];
            }
            if (tracked) {
                continue;
            }
            const uint8_t *dp = NULL;
            size_t ilen = 0;
            if (!EFI_ERROR(BS->HandleProtocol(hb[k], (EFI_GUID *)&g_devpath, (void **)&dp)) && dp != NULL) {
                ilen = ident_len(dp);
            }
            // A port coming back to the socket it was in before gets its old slot.
            mp_uefi_serial_t *s = NULL;
            for (size_t i = 0; i < mp_uefi_serial_nslots && s == NULL; i++) {
                mp_uefi_serial_t *c = &mp_uefi_serial_slots[i];
                if (!c->present && ilen > 0 && c->ident_len == ilen && memcmp(c->ident, dp, ilen) == 0) {
                    s = c;
                }
            }
            if (s == NULL) {
                if (mp_uefi_serial_nslots == MP_UEFI_SERIAL_MAX) {
                    continue;   // shortcut: ports beyond 8 per session are ignored
                }
                s = &mp_uefi_serial_slots[mp_uefi_serial_nslots++];
                memset(s, 0, sizeof(*s));
                if (ilen > 0 && (s->ident = pool_alloc(ilen)) != NULL) {
                    memcpy(s->ident, dp, ilen);
                    s->ident_len = ilen;
                }
            }
            s->handle = hb[k];
            s->io = io;
            s->owned = false;
            s->present = true;
        }
        BS->FreePool(hb);
    }
    BS->RestoreTPL(old);
}

int mp_uefi_serial_find_index(long idx) {
    return (idx >= 0 && (size_t)idx < mp_uefi_serial_nslots) ? (int)idx : -1;
}

static EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *dp2text(void) {
    static EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *p;
    if (p == NULL) {
        BS->LocateProtocol((void *)&g_dp2text, NULL, (void **)&p);
    }
    return p;
}

void mp_uefi_serial_path_text(mp_uefi_serial_t *s, char *buf, size_t cap) {
    buf[0] = 0;
    EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *t = dp2text();
    const void *dp = NULL;
    if (t == NULL || !mp_uefi_serial_check(s) ||
        EFI_ERROR(BS->HandleProtocol(s->handle, (EFI_GUID *)&g_devpath, (void **)&dp)) || dp == NULL) {
        return;
    }
    CHAR16 *w = t->ConvertDevicePathToText(dp, 0, 0);
    if (w == NULL) {
        return;
    }
    size_t n = 0;
    for (; w[n] != 0 && n + 1 < cap; n++) {
        buf[n] = w[n] < 0x80 ? (char)w[n] : '?';
    }
    buf[n] = 0;
    BS->FreePool(w);
}

int mp_uefi_serial_find_text(const char *text) {
    if (dp2text() == NULL) {
        return -2;
    }
    char path[256];
    for (size_t i = 0; i < mp_uefi_serial_nslots; i++) {
        mp_uefi_serial_path_text(&mp_uefi_serial_slots[i], path, sizeof(path));
        if (path[0] != 0 && strstr(path, text) != NULL) {
            return (int)i;
        }
    }
    return -1;
}

// --- open / configure / close ---

EFI_STATUS mp_uefi_serial_open(mp_uefi_serial_t *s, size_t rxbuf) {
    if (!mp_uefi_serial_check(s)) {
        return EFI_NOT_FOUND;
    }
    if (rxbuf < 16) {
        rxbuf = 16;
    } else if (rxbuf > 0xffff) {
        rxbuf = 0xffff;     // ringbuf_t sizes are 16-bit
    }
    if (s->rx.buf == NULL || (s->rx.size != rxbuf && !s->repl)) {
        uint8_t *nb = pool_alloc(rxbuf);
        if (nb == NULL) {
            return EFI_OUT_OF_RESOURCES;
        }
        EFI_TPL old = BS->RaiseTPL(TPL_CALLBACK);
        uint8_t *ob = s->rx.buf;
        s->rx.buf = nb;
        s->rx.size = (uint16_t)rxbuf;
        s->rx.iget = s->rx.iput = 0;
        BS->RestoreTPL(old);
        if (ob != NULL) {
            BS->FreePool(ob);
        }
    }
    if (s->owned) {
        return EFI_SUCCESS;
    }
    EFI_SERIAL_IO_MODE *m = s->io->Mode;
    s->saved_valid = m != NULL;
    if (m != NULL) {
        s->saved_baud = m->BaudRate;
        s->saved_timeout = m->Timeout;
        s->saved_parity = m->Parity;
        s->saved_bits = m->DataBits;
        s->saved_stop = m->StopBits;
    }
    s->rx_gated = m != NULL && (m->ControlMask & EFI_SERIAL_INPUT_BUFFER_EMPTY);
    UINT32 c = 0;
    s->io->GetControl(s->io, &c);
    s->saved_control = c & SETTABLE_CONTROL;
    // Evict TerminalDxe (and any other consumer) so nothing else reads or writes it.
    BS->DisconnectController(s->handle, NULL, NULL);
    s->rx.iget = s->rx.iput = 0;
    s->drv_data = false;
    s->gen++;
    s->owned = true;
    return EFI_SUCCESS;
}

bool mp_uefi_serial_flow_supported(mp_uefi_serial_t *s) {
    return s->io->Mode != NULL && (s->io->Mode->ControlMask & EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE);
}

uint32_t mp_uefi_serial_char_bits(mp_uefi_serial_t *s) {
    return 1 + s->bits + (s->parity != EFI_SERIAL_NO_PARITY) + (s->stop == EFI_SERIAL_TWO_STOP_BITS ? 2 : 1);
}

EFI_STATUS mp_uefi_serial_configure(mp_uefi_serial_t *s, uint32_t baud, uint8_t bits,
    uint8_t parity, uint8_t stop, int flow) {
    if (!mp_uefi_serial_usable(s)) {
        return EFI_NOT_FOUND;
    }
    bool can_flow = mp_uefi_serial_flow_supported(s);
    if (flow == 1 && !can_flow) {
        return EFI_UNSUPPORTED;
    }
    // Device timeout = one character time: a Read for more than is buffered returns
    // almost at once with what's there, so the timer notify never blocks.
    s->bits = bits;
    s->parity = parity;
    s->stop = stop;
    UINT32 char_us = (mp_uefi_serial_char_bits(s) * 1000000u + baud - 1) / baud;
    EFI_STATUS st = s->io->SetAttributes(s->io, baud, 0, char_us, parity, bits, stop);
    if (EFI_ERROR(st)) {
        return st;
    }
    EFI_SERIAL_IO_MODE *m = s->io->Mode;
    s->baud = (m != NULL && m->BaudRate != 0) ? (uint32_t)m->BaudRate : baud;
    UINT32 c = 0;
    s->io->GetControl(s->io, &c);
    bool want = flow < 0 ? can_flow : flow == 1;
    s->io->SetControl(s->io, (c & (EFI_SERIAL_DATA_TERMINAL_READY | EFI_SERIAL_REQUEST_TO_SEND)) |
        (want ? EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE : 0));
    c = 0;
    s->io->GetControl(s->io, &c);
    s->rtscts = (c & EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE) != 0;
    // Drivers reinstall the device path when settings change, which reconnects
    // TerminalDxe through ReinstallProtocolInterface: evict it again.
    BS->DisconnectController(s->handle, NULL, NULL);
    if (s->rtscts != want && flow >= 0) {
        return EFI_DEVICE_ERROR;
    }
    return EFI_SUCCESS;
}

void mp_uefi_serial_close(mp_uefi_serial_t *s) {
    if (!s->owned) {
        return;
    }
    s->owned = false;   // the timer skips it from here on
    if (!mp_uefi_serial_check(s)) {
        return;         // gone: nothing to give back
    }
    // Write returns once the device accepts the bytes, not once they're on the wire, and
    // TerminalDxe's Start resets the port (purging the device's TX FIFO), so let it drain.
    // The status may be one driver poll stale, hence the initial settle; bounded to ~1 s.
    if (s->io->Mode != NULL && (s->io->Mode->ControlMask & EFI_SERIAL_OUTPUT_BUFFER_EMPTY)) {
        UINT32 c;
        BS->Stall(20000);
        for (int ms = 0; ms < 1000 && s->io->GetControl(s->io, &c) == EFI_SUCCESS &&
             !(c & EFI_SERIAL_OUTPUT_BUFFER_EMPTY); ms++) {
            BS->Stall(1000);
        }
    }
    if (s->saved_valid) {
        s->io->SetAttributes(s->io, s->saved_baud, 0, s->saved_timeout, s->saved_parity,
            (UINT8)s->saved_bits, s->saved_stop);
    }
    s->io->SetControl(s->io, s->saved_control);
    BS->ConnectController(s->handle, NULL, NULL, 1);
}

void mp_uefi_serial_close_all(bool include_repl) {
    for (size_t i = 0; i < mp_uefi_serial_nslots; i++) {
        mp_uefi_serial_t *s = &mp_uefi_serial_slots[i];
        if (include_repl || !s->repl) {
            mp_uefi_serial_close(s);
        }
    }
    if (!include_repl) {
        return;
    }
    for (size_t i = 0; i < mp_uefi_serial_nslots; i++) {
        mp_uefi_serial_t *s = &mp_uefi_serial_slots[i];
        if (s->rx.buf != NULL) {
            BS->FreePool(s->rx.buf);
        }
        if (s->ident != NULL) {
            BS->FreePool(s->ident);
        }
        memset(s, 0, sizeof(*s));
    }
    mp_uefi_serial_nslots = 0;
}

// --- arrival notification ---

static void EFIAPI arrival_notify(EFI_EVENT event, void *context) {
    (void)event;
    (void)context;
    rescan_pending = true;
    mp_uefi_wake();
}

void mp_uefi_serial_events_init(void) {
    if (!EFI_ERROR(BS->CreateEvent(EVT_NOTIFY_SIGNAL, TPL_CALLBACK, arrival_notify, NULL, &arrival_event))) {
        BS->RegisterProtocolNotify((EFI_GUID *)&g_serial, arrival_event, &arrival_reg);
    }
}

void mp_uefi_serial_events_deinit(void) {
    if (arrival_event != NULL) {
        BS->CloseEvent(arrival_event);   // also cancels the registration
        arrival_event = NULL;
    }
}

// --- RX ---

// One bulk Read of up to RX_CHUNK bytes into the ring; drops nothing (stops when the
// ring is full, leaving bytes in the driver). `intr` bytes become KeyboardInterrupt.
static void fill(mp_uefi_serial_t *s, int intr) {
    size_t room = ringbuf_free(&s->rx);
    if (room == 0) {
        return;
    }
    uint8_t tmp[RX_CHUNK];
    UINTN asked = room < sizeof(tmp) ? room : sizeof(tmp);
    UINTN n = asked;
    EFI_STATUS st = s->io->Read(s->io, &n, tmp);
    if (EFI_ERROR(st) && st != EFI_TIMEOUT) {
        return;
    }
    if (n > asked) {
        n = asked;      // never trust a driver to report more than we asked for
    }
    bool put = false;
    for (UINTN i = 0; i < n; i++) {
        if ((int)tmp[i] == intr) {
            #if MICROPY_KBD_EXCEPTION
            mp_sched_keyboard_interrupt();
            #endif
            continue;   // the interrupt char itself is not data
        }
        ringbuf_put(&s->rx, tmp[i]);
        put = true;
    }
    if (put) {
        mp_uefi_wake();
    }
}

static bool driver_has_data(mp_uefi_serial_t *s) {
    UINT32 c;
    return s->io->GetControl(s->io, &c) == EFI_SUCCESS && !(c & EFI_SERIAL_INPUT_BUFFER_EMPTY);
}

void mp_uefi_serial_poll(int intr) {
    for (size_t i = 0; i < mp_uefi_serial_nslots; i++) {
        mp_uefi_serial_t *s = &mp_uefi_serial_slots[i];
        if (!s->owned || !mp_uefi_serial_check(s)) {
            continue;
        }
        if (!s->repl && s->rx_gated) {
            // The driver buffers in the background: just wake waiters when data arrives.
            bool has = driver_has_data(s);
            if (has && !s->drv_data) {
                mp_uefi_wake();
            }
            s->drv_data = has;
            continue;
        }
        if (!s->rx_gated || driver_has_data(s)) {
            fill(s, s->repl ? intr : -1);
        }
    }
}

// Foreground: pull from the driver on demand for a port the timer doesn't read.
static void fetch(mp_uefi_serial_t *s) {
    if (!s->repl && s->rx_gated && ringbuf_free(&s->rx) > 0 && driver_has_data(s)) {
        fill(s, -1);
    }
}

size_t mp_uefi_serial_read(mp_uefi_serial_t *s, uint8_t *buf, size_t n) {
    if (mp_uefi_serial_usable(s)) {
        fetch(s);
    }
    size_t got = 0;
    int c;
    while (got < n && (c = ringbuf_get(&s->rx)) >= 0) {
        buf[got++] = (uint8_t)c;
    }
    return got;
}

size_t mp_uefi_serial_any(mp_uefi_serial_t *s) {
    if (mp_uefi_serial_usable(s)) {
        fetch(s);
    }
    return ringbuf_avail(&s->rx);
}

// --- TX ---

size_t mp_uefi_serial_write(mp_uefi_serial_t *s, const uint8_t *buf, size_t len,
    uint32_t stall_ms, EFI_STATUS *err) {
    size_t done = 0;
    *err = EFI_SUCCESS;
    mp_uint_t last = mp_hal_ticks_ms();
    while (done < len) {
        if (!mp_uefi_serial_usable(s)) {
            *err = EFI_NOT_FOUND;
            break;
        }
        // Bulk: hand over the whole remainder; BufferSize comes back as what was written.
        UINTN sz = len - done;
        EFI_STATUS st = s->io->Write(s->io, &sz, (void *)(buf + done));
        if (sz > len - done) {
            sz = len - done;
        }
        done += sz;
        if (sz > 0) {
            last = mp_hal_ticks_ms();
            continue;
        }
        *err = st;
        if ((EFI_ERROR(st) && st != EFI_TIMEOUT) || mp_hal_ticks_ms() - last >= stall_ms) {
            break;
        }
    }
    return done;
}

bool mp_uefi_serial_writable(mp_uefi_serial_t *s) {
    UINT32 c;
    return !s->rtscts || (s->io->GetControl(s->io, &c) == EFI_SUCCESS && (c & EFI_SERIAL_CLEAR_TO_SEND));
}

bool mp_uefi_serial_txdone(mp_uefi_serial_t *s) {
    UINT32 c;
    if (s->io->Mode == NULL || !(s->io->Mode->ControlMask & EFI_SERIAL_OUTPUT_BUFFER_EMPTY)) {
        return true;
    }
    return s->io->GetControl(s->io, &c) == EFI_SUCCESS && (c & EFI_SERIAL_OUTPUT_BUFFER_EMPTY);
}
