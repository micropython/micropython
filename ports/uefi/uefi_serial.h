// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Nicko van Someren

// Serial port driver layer over EFI_SERIAL_IO_PROTOCOL (uefi_serial.c). Knows nothing
// about the REPL beyond filtering one interrupt character on the port flagged `repl`.
// See docs/serial-uart-design.md.

#ifndef MICROPY_INCLUDED_UEFI_SERIAL_H
#define MICROPY_INCLUDED_UEFI_SERIAL_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "py/ringbuf.h"
#include "efi.h"

#define MP_UEFI_SERIAL_MAX (8)          // slots per session
#define MP_UEFI_SERIAL_RXBUF_DEFAULT (1024)

// One slot per port ever seen this session. A slot keeps its index when its port is
// unplugged, so ids never shift; a port that comes back with the same identity path
// (same socket) gets the same slot.
typedef struct {
    EFI_HANDLE handle;
    EFI_SERIAL_IO_PROTOCOL *io;
    uint8_t *ident;             // device path up to the Uart node (pool copy): identity
    size_t ident_len;
    bool present;               // handle last seen valid
    bool owned;                 // we took it from TerminalDxe and configured it
    bool repl;                  // the REPL's port: filter the interrupt char, auto re-attach
    bool rx_gated;              // driver reports INPUT_BUFFER_EMPTY (fills in the background)
    bool drv_data;              // last poll saw data waiting in the driver (wake on edge)
    uint32_t gen;               // bumped on every open; stale objects compare against it
    // Settings found on open, restored on close.
    UINT64 saved_baud;
    UINT32 saved_timeout, saved_parity, saved_bits, saved_stop, saved_control;
    bool saved_valid;
    // Current settings (read back from the driver).
    uint32_t baud;
    uint8_t bits, parity, stop; // parity/stop as EFI_SERIAL_* enum values
    bool rtscts;
    ringbuf_t rx;               // pool-allocated; survives loss of the port
} mp_uefi_serial_t;

extern mp_uefi_serial_t mp_uefi_serial_slots[MP_UEFI_SERIAL_MAX];
extern size_t mp_uefi_serial_nslots;

// Match ports to slots (call from foreground code). Cheap; safe to call often.
void mp_uefi_serial_scan(void);
// True once a SerialIo install was seen and not yet scanned for.
bool mp_uefi_serial_rescan_pending(void);
// Slot for an index, or -1. Slot for the first present port whose device-path text
// contains `text`, -1 if none, -2 if the firmware can't convert paths to text.
int mp_uefi_serial_find_index(long idx);
int mp_uefi_serial_find_text(const char *text);
// Device-path text of a port into buf (empty string if unavailable).
void mp_uefi_serial_path_text(mp_uefi_serial_t *s, char *buf, size_t cap);

// Is the port's handle still carrying the same SerialIo interface? Marks the slot
// absent (and not owned) if not. Never call into s->io without this.
bool mp_uefi_serial_check(mp_uefi_serial_t *s);
// Usable: present, owned and the handle still valid.
static inline bool mp_uefi_serial_usable(mp_uefi_serial_t *s) {
    return s->present && s->owned && mp_uefi_serial_check(s);
}

// Take the port (evict TerminalDxe, save settings, allocate the RX ring). No-op if
// already owned, except resizing the ring when rxbuf differs (never on the REPL port).
EFI_STATUS mp_uefi_serial_open(mp_uefi_serial_t *s, size_t rxbuf);
// flow: -1 = on if supported, 0 = off, 1 = on (EFI_UNSUPPORTED if not supported).
EFI_STATUS mp_uefi_serial_configure(mp_uefi_serial_t *s, uint32_t baud, uint8_t bits,
    uint8_t parity, uint8_t stop, int flow);
bool mp_uefi_serial_flow_supported(mp_uefi_serial_t *s);
// Give the port back: drain TX, restore settings, reconnect TerminalDxe.
void mp_uefi_serial_close(mp_uefi_serial_t *s);
// Close every owned port (the REPL's too if include_repl) and, when include_repl,
// release all memory. Call with the poll timer stopped.
void mp_uefi_serial_close_all(bool include_repl);

// Arrival notification (RegisterProtocolNotify); call once at start-up / teardown.
void mp_uefi_serial_events_init(void);
void mp_uefi_serial_events_deinit(void);

// Timer notify body (TPL_CALLBACK): service every owned port. intr is the REPL's
// interrupt character (-1 = none), applied only to the port flagged `repl`.
void mp_uefi_serial_poll(int intr);

// Foreground I/O. read: non-blocking, returns bytes copied (fetches from the driver
// on demand for a background-filled port). write: until done, a hard error, or
// stall_ms without progress; returns bytes written, *err the last failure status.
size_t mp_uefi_serial_read(mp_uefi_serial_t *s, uint8_t *buf, size_t n);
size_t mp_uefi_serial_any(mp_uefi_serial_t *s);
size_t mp_uefi_serial_write(mp_uefi_serial_t *s, const uint8_t *buf, size_t len,
    uint32_t stall_ms, EFI_STATUS *err);
bool mp_uefi_serial_writable(mp_uefi_serial_t *s);
bool mp_uefi_serial_txdone(mp_uefi_serial_t *s);
// Bits per character on the wire (start + data + parity + stop).
uint32_t mp_uefi_serial_char_bits(mp_uefi_serial_t *s);

// The REPL's port (REPL glue, uefi_event.c), or NULL if the REPL isn't on serial.
mp_uefi_serial_t *mp_uefi_serial_repl_port(void);

#endif // MICROPY_INCLUDED_UEFI_SERIAL_H
