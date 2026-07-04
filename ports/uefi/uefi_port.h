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
void efi_event_deinit_all(void); // defined in modefi.c

// Mount the boot volume at "/" (uefi_vfs.c). Called once, after mp_init().
void mp_uefi_vfs_mount_boot(void);

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

// Atomic section = raise to TPL_HIGH_LEVEL / restore. Returns the previous TPL.
uintptr_t mp_uefi_begin_atomic(void);
void mp_uefi_end_atomic(uintptr_t old_tpl);

// L2 timer primitives: a UEFI timer event whose expiry runs `notify` (hard-ISR
// context, TPL_CALLBACK). Used by machine.Timer and, later, efi.Timer/efi.Event.
EFI_STATUS mp_uefi_create_timer(EFI_EVENT_NOTIFY notify, void *context, EFI_EVENT *out);
void mp_uefi_arm_timer(EFI_EVENT ev, int periodic, uint64_t period_us);
void mp_uefi_cancel_timer(EFI_EVENT ev);
void mp_uefi_close_event(EFI_EVENT ev);

// L2 signal-event primitives: a UEFI EVT_NOTIFY_SIGNAL event (efi.Event, protocol
// completion tokens). The notify runs hard-ISR at TPL_CALLBACK. mp_uefi_wake()
// nudges a blocked foreground to re-poll.
EFI_STATUS mp_uefi_create_signal_event(EFI_EVENT_NOTIFY notify, void *context, EFI_EVENT *out);
void mp_uefi_signal_event(EFI_EVENT ev);
void mp_uefi_wake(void);

#endif // MPY_UEFI_PORT_H
