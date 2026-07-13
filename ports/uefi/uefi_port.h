// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Nicko van Someren

// Shared port globals/helpers (the live UEFI tables + the test-exit path).
#ifndef MPY_UEFI_PORT_H
#define MPY_UEFI_PORT_H

#include "efi.h"

extern EFI_SYSTEM_TABLE *mp_uefi_st;
extern EFI_HANDLE mp_uefi_image;

// Signal the test harness and leave: prints nothing itself; on x86-64 writes the
// QEMU isa-debug-exit port, on aarch64 performs a clean firmware shutdown.
void mp_uefi_exit(int success) __attribute__((noreturn));

// --- async/timing framework (uefi_time.c, uefi_event.c) ---
// Initialise the timebase (counter calibration) and the event machinery (stdin
// poll timer, wakeup + per-wait timeout events). Call once, after mp_init().
void mp_uefi_time_init(void);
void mp_uefi_async_init(void);

// Teardown, before mp_deinit(): stop all timers/notifies so nothing fires into a
// torn-down interpreter. machine_timer_deinit_all() is defined in modmachine.c.
void mp_uefi_async_deinit(void);
void machine_timer_deinit_all(void);
void uefi_event_obj_deinit_all(void); // defined in uefi_event_obj.c

// Mount the boot volume at "/" (uefi_vfs.c). Called once, after mp_init().
void mp_uefi_vfs_mount_boot(void);

// Byte-clean serial REPL (uefi_event.c). mp_uefi_serial_init() switches REPL I/O from
// the ConOut/ConIn console (TerminalDxe, which mangles the raw-REPL protocol) to a
// directly-driven EFI_SERIAL_IO_PROTOCOL; when the flag is set the stdin/stdout HAL
// uses the serial. Returns true on success.
extern bool mp_uefi_serial_repl;
bool mp_uefi_serial_init(void);
void mp_uefi_serial_tx(const char *str, size_t len);

// Build a VFS object (mountable via os.mount) from an EFI_SIMPLE_FILE_SYSTEM_PROTOCOL
// interface pointer: opens the volume root. Backs uefi.fs multi-volume mounting.
// Returns an mp_obj_t (typed void* to keep this header free of py/obj.h).
void *mp_uefi_vfs_from_simple_fs(void *fs_iface);

// The single blocking primitive (see MICROPY_INTERNAL_WFE). Suspends in
// BootServices->WaitForEvent until an event fires or timeout_ms elapse; a
// negative timeout waits indefinitely. Must run at TPL_APPLICATION.
void mp_uefi_wfe(int64_t timeout_ms);

// Monotonic counter frequency in Hz (machine.freq()).
uint64_t mp_uefi_counter_hz(void);

// Wall clock (uefi_time.c). The wall time is a single anchored clock: the RTC
// (RuntimeServices->GetTime) is sampled only to (re)establish the anchor, and the
// sub-second part is interpolated from the monotonic counter — so seconds and
// sub-seconds always come from one monotone source (no second/sub-second skew at a
// tick roll-over). mp_uefi_wall_ns() returns nanoseconds since 1970; time(), time_ns(),
// localtime() and machine.RTC all derive from it. Re-anchor after any in-band RTC set
// (machine.RTC.datetime(), uefi.raw.set_time()); a manual RS->SetTime is not seen.
void mp_uefi_time_anchor(void);
uint64_t mp_uefi_wall_ns(void);
uint64_t mp_uefi_ticks_ns(void); // monotonic counter, scaled to ns (arbitrary origin)

// Atomic section = raise to TPL_HIGH_LEVEL / restore. Returns the previous TPL.
uintptr_t mp_uefi_begin_atomic(void);
void mp_uefi_end_atomic(uintptr_t old_tpl);

// L2 timer primitives: a UEFI timer event whose expiry runs `notify` (hard-ISR
// context, TPL_CALLBACK). Used by machine.Timer and, later, uefi.Timer/uefi.Event.
EFI_STATUS mp_uefi_create_timer(EFI_EVENT_NOTIFY notify, void *context, EFI_EVENT *out);
void mp_uefi_arm_timer(EFI_EVENT ev, int periodic, uint64_t period_us);
void mp_uefi_cancel_timer(EFI_EVENT ev);
void mp_uefi_close_event(EFI_EVENT ev);

// L2 signal-event primitives: a UEFI EVT_NOTIFY_SIGNAL event (uefi.Event, protocol
// completion tokens). The notify runs hard-ISR at TPL_CALLBACK. mp_uefi_wake()
// nudges a blocked foreground to re-poll.
EFI_STATUS mp_uefi_create_signal_event(EFI_EVENT_NOTIFY notify, void *context, EFI_EVENT *out);
void mp_uefi_signal_event(EFI_EVENT ev);
void mp_uefi_wake(void);

#endif // MPY_UEFI_PORT_H
