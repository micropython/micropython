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

// Configuration for the MicroPython UEFI port (aarch64 + x86-64/MS-ABI).
//
// Based on CORE_FEATURES (below), with double-precision float, MPZ ints, the
// async/timing framework, a VFS over EFI_SIMPLE_FILE_SYSTEM, frozen asyncio, and
// the host-independent stdlib modules switched on. See README.md.

#include <stdint.h>

// Base on the CORE feature set (a usable Python: full builtins, async/await, the
// standard library surface), then disable what the freestanding UEFI environment
// can't support yet and enable the port-specific extras below.
#define MICROPY_CONFIG_ROM_LEVEL            (MICROPY_CONFIG_ROM_LEVEL_CORE_FEATURES)

// Named errors/tracebacks (the MINIMUM default is terse, which hides identifiers).
#define MICROPY_ERROR_REPORTING            (MICROPY_ERROR_REPORTING_NORMAL)

// GC is off by default; the REPL helpers + keyboard-interrupt are EXTRA-level.
#define MICROPY_ENABLE_GC                   (1)
#define MICROPY_HELPER_REPL                 (1)
#define MICROPY_REPL_AUTO_INDENT            (1)
#define MICROPY_KBD_EXCEPTION               (1)

// ConOut is a UTF-16 terminal-emulation API, not a byte-clean pipe: it does not
// interpret raw VT100 escapes (e.g. erase-to-EOL "\x1b[K"). Keep VT100 off and let
// readline drive cursor moves / line erases through ConOut's own API (see mphalport.c).
#define MICROPY_HAL_HAS_VT100               (0)

// Frozen modules let us ship asyncio (extmod/asyncio) without a filesystem.
// MICROPY_MODULE_FROZEN_MPY is defined on the command line by the frozen-manifest
// rules in py/mkrules.mk (driven by FROZEN_MANIFEST in the Makefile). The external
// import path is required to search frozen modules: with sys.path enabled, mp_init
// seeds sys.path with ".frozen", and our mp_import_stat stub misses the (absent)
// filesystem so only frozen modules resolve.
#define MICROPY_ENABLE_EXTERNAL_IMPORT      (1)

// No OS errno / no toolchain libc errno — MicroPython supplies its own.
#define MICROPY_USE_INTERNAL_ERRNO          (1)

// Provide printf/snprintf/vsnprintf (used by pyexec/readline) via
// shared/libc/printf.c routed to mp_hal — no toolchain stdio in this build.
#define MICROPY_USE_INTERNAL_PRINTF         (1)

// We provide a tiny libc subset; use the compiler builtin for alloca.
#define alloca(n)                           __builtin_alloca(n)

// Single-threaded, cooperative (UEFI boot services).
#define MICROPY_PY_THREAD                   (0)

// Float: double precision (decided default). Backed by the bundled lib/libm_dbl
// (no system libm in this freestanding build).
#define MICROPY_FLOAT_IMPL                  (MICROPY_FLOAT_IMPL_DOUBLE)

// Use the portable bit-twiddling half-float (typecode 'e') path, not _Float16:
// baseline x86-64 has no FP16 hardware, so the native path needs compiler-rt
// (__truncsfhf2) which a freestanding build doesn't link. This keeps both arches
// identical and dependency-free.
#define MICROPY_FLOAT_USE_NATIVE_FLT16      (0)

#define MICROPY_ALLOC_PATH_MAX              (256)
#define MICROPY_ALLOC_PARSE_CHUNK_INIT      (16)

// Arbitrary-precision integers (MPZ). Real Python ints, and required to match the
// mpy-cross config baked into frozen .mpy modules (the frozen content guards on
// MICROPY_LONGINT_IMPL / MPZ_DIG_SIZE).
#define MICROPY_LONGINT_IMPL                (MICROPY_LONGINT_IMPL_MPZ)

// --- async / timing framework (see README.md) ---
// Cooperative scheduler: mp_sched_schedule (soft callbacks) + the pending-
// exception path used for Ctrl-C. Static nodes give allocation-free C drainers
// for the UEFI-event bridge. UEFI event notifies are treated as hard ISRs and
// may only touch the scheduler via these async-safe entry points.
#define MICROPY_ENABLE_SCHEDULER            (1)
#define MICROPY_SCHEDULER_STATIC_NODES      (1)

// Atomic section == RaiseTPL(TPL_HIGH_LEVEL) / RestoreTPL (uefi_event.c): masks
// our event notifies so scheduler/ringbuf state stays consistent. Returns the
// previous TPL, which the END macro restores.
uintptr_t mp_uefi_begin_atomic(void);
void mp_uefi_end_atomic(uintptr_t old_tpl);
#define MICROPY_BEGIN_ATOMIC_SECTION()      mp_uefi_begin_atomic()
#define MICROPY_END_ATOMIC_SECTION(state)   mp_uefi_end_atomic(state)

// Idle: every blocking path (REPL input, sleep, select.poll, asyncio) funnels
// through MICROPY_INTERNAL_WFE, which parks in BootServices->WaitForEvent. The
// non-blocking hook has nothing to do — the ConIn poll timer feeds stdin.
void mp_uefi_wfe(int64_t timeout_ms);
#define MICROPY_INTERNAL_WFE(TIMEOUT_MS)    mp_uefi_wfe(TIMEOUT_MS)
#define MICROPY_INTERNAL_EVENT_HOOK         do { } while (0)

// Wake a blocked WaitForEvent when work is scheduled (e.g. from a timer notify),
// so callbacks run promptly while the foreground is idle waiting on input.
void mp_uefi_sched_signal(void);
#define MICROPY_SCHED_HOOK_SCHEDULED        mp_uefi_sched_signal()

// time module: ticks_*/sleep* over the hardware timebase (uefi_time.c). It's a
// BASIC-level feature (above CORE), so enable it explicitly. Wall clock
// (time()/time_ns()/gmtime/localtime/mktime) comes from RuntimeServices->GetTime
// via modtime_uefi.c. Use the Unix (1970) epoch for CPython compatibility.
#define MICROPY_PY_TIME                          (1)
#define MICROPY_PY_TIME_TIME_TIME_NS            (1)
#define MICROPY_PY_TIME_GMTIME_LOCALTIME_MKTIME (1)
#define MICROPY_PY_TIME_INCLUDEFILE             "ports/uefi/modtime_uefi.c"
#define MICROPY_EPOCH_IS_1970                   (1)

// Finalisers (EXTRA-level): let a dropped machine.Timer's __del__ stop its UEFI
// timer event (the GC-typical "lose the reference -> it stops" behaviour).
#define MICROPY_ENABLE_FINALISER            (1)

// machine module: machine.idle() (-> WFE) + machine.Timer (period/oneshot with a
// soft Python callback over a UEFI timer event; see modmachine.c). The standard
// extmod skeleton needs sys.exit for machine.soft_reset.
#define MICROPY_PY_MACHINE                  (1)
#define MICROPY_PY_MACHINE_INCLUDEFILE      "ports/uefi/modmachine.c"
// machine basics. mem8/16/32 (flat-mapped: identity address, no port hook), reset,
// the bare-metal funcs (unique_id from SMBIOS, freq from the counter, light/deep
// sleep), and WDT over SetWatchdogTimer. Signal/Pin (hardware) stay off.
#define MICROPY_PY_MACHINE_MEMX             (1)
#define MICROPY_PY_MACHINE_RESET            (1)
#define MICROPY_PY_MACHINE_BARE_METAL_FUNCS (1)
#define MICROPY_PY_MACHINE_WDT              (1)
#define MICROPY_PY_MACHINE_WDT_INCLUDEFILE  "ports/uefi/modmachine_wdt.c"
#define MICROPY_PY_MACHINE_SIGNAL           (0)

// help() + help('modules'). Custom intro text defined in main.c.
extern const char uefi_help_text[];
#define MICROPY_PY_BUILTINS_HELP            (1)
#define MICROPY_PY_BUILTINS_HELP_TEXT       uefi_help_text
#define MICROPY_PY_BUILTINS_HELP_MODULES    (1)

// --- select.poll + pollable stdio (foundation for asyncio) ---
// Expose sys.stdin/stdout/stderr as stream objects (shared/runtime/sys_stdio_mphal.c).
// stdin's ioctl(MP_STREAM_POLL) routes to mp_hal_stdio_poll (ringbuf readiness),
// so select.poll — and hence asyncio — can wait on it.
#define MICROPY_PY_SYS_STDFILES             (1)
#define MICROPY_PY_SYS_STDIO_MPHAL          (1)
// select.poll via the ioctl(MP_STREAM_POLL) path (not POSIX fds). Its wait loop
// sleeps in mp_event_wait -> mp_uefi_wfe, waking on wakeup_event / timeout.
#define MICROPY_PY_SELECT                   (1)
// Non-blocking stream I/O returns None on EAGAIN instead of raising — required so
// asyncio's stream reader/writer (which check `if r is not None`) drive our socket.
#define MICROPY_STREAMS_NON_BLOCK           (1)

// --- Filesystem: a VFS over EFI_SIMPLE_FILE_SYSTEM (uefi_vfs.c), mounted at "/"
// on the boot volume. Gives open(), import-from-disk, and the os module. ---
#define MICROPY_VFS                         (1)
#define MICROPY_PY_OS                       (1)
#define MICROPY_READER_VFS                  (1)
// Import external .mpy bytecode off the VFS (not just frozen). Native-code .mpy is
// left off (no aarch64 emitter; the x64 emitter is SysV vs our -mabi=ms core).
#define MICROPY_PERSISTENT_CODE_LOAD        (1)
// marshal (dumps/loads a code object) + compile(), for an in-memory bytecode round-trip.
#define MICROPY_PY_MARSHAL                  (1)
#define MICROPY_PY_BUILTINS_COMPILE         (1)
// open() routes to the VFS (the builtins table references mp_builtin_open_obj).
#define mp_builtin_open_obj                 mp_vfs_open_obj
// The `vfs` module (modvfs.c) unconditionally includes the FatFs header, which
// pulls <windows.h> on our *-windows target triple. We don't use it (os.mount
// lives in the os module), so keep it off — our VFS infrastructure is unaffected.
#define MICROPY_PY_VFS                      (0)
// os extras: urandom (uefi_rng.c) + uname + sync (a no-op without a FAT VFS, but
// present so user code can call it). sys.platform names the environment.
#define MICROPY_PY_OS_URANDOM               (1)
#define MICROPY_PY_OS_UNAME                 (1)
#define MICROPY_PY_OS_SYNC                  (1)
#define MICROPY_PY_SYS_PLATFORM             "uefi"

// asyncio C accelerator (TaskQueue/Task); the frozen asyncio manifest omits
// task.py assuming it is present. (async/await + generator .throw() are on at the
// CORE level.)
#define MICROPY_PY_ASYNCIO                  (1)

// --- Standard-library modules with no host/OS dependency (EXTRA-level; off by
// default at CORE). Pure C / frozen-free, so they "just work". ---
#define MICROPY_PY_RE                       (1)
#define MICROPY_PY_RE_SUB                   (1)
#define MICROPY_PY_JSON                     (1)
#define MICROPY_PY_DEFLATE                  (1)
#define MICROPY_PY_BINASCII                 (1)
#define MICROPY_PY_HASHLIB                  (1)
// SHA1/MD5 come from mbedTLS (SHA256 is self-contained), so gate them on it.
#define MICROPY_PY_HASHLIB_MD5             (MICROPY_SSL_MBEDTLS)
#define MICROPY_PY_HASHLIB_SHA1           (MICROPY_SSL_MBEDTLS)
#define MICROPY_PY_HEAPQ                    (1)
#define MICROPY_PY_CMATH                    (1)
#define MICROPY_PY_FRAMEBUF                 (1)
#define MICROPY_PY_PLATFORM                 (1)
#define MICROPY_PY_ERRNO                    (1)
// random + os.urandom share the entropy source in uefi_rng.c (EFI_RNG -> RDRAND ->
// counter PRNG). The seed func backs random.seed() with no argument.
#define MICROPY_PY_RANDOM                   (1)
unsigned int mp_uefi_random_seed(void);
#define MICROPY_PY_RANDOM_SEED_INIT_FUNC    (mp_uefi_random_seed())

// Extra builtins beyond CORE: memoryview/frozenset/slice-attrs/round-to-int, and
// bytes.hex()/fromhex() (also used by binascii.hexlify/unhexlify).
#define MICROPY_PY_BUILTINS_MEMORYVIEW      (1)
#define MICROPY_PY_BUILTINS_FROZENSET       (1)
#define MICROPY_PY_BUILTINS_SLICE_ATTRS     (1)
#define MICROPY_PY_BUILTINS_ROUND_INT       (1)
#define MICROPY_PY_BUILTINS_BYTES_HEX       (1)

// --- More host-independent language + library features ---
#define MICROPY_PY_FSTRINGS                 (1)  // f-strings
#define MICROPY_PY_ASSIGN_EXPR              (1)  // walrus :=
#define MICROPY_PY_UCTYPES                  (1)  // uctypes (struct-over-memory)
#define MICROPY_PY_MATH_SPECIAL_FUNCTIONS   (1)  // libm_dbl provides these; declared in include/math.h
#define MICROPY_PY_MATH_FACTORIAL           (1)
#define MICROPY_PY_MATH_ISCLOSE             (1)
#define MICROPY_PY_COLLECTIONS_DEQUE        (1)
#define MICROPY_PY_COLLECTIONS_ORDEREDDICT  (1)
#define MICROPY_PY_COLLECTIONS_NAMEDTUPLE__ASDICT (1)
#define MICROPY_PY_BUILTINS_STR_CENTER      (1)
#define MICROPY_PY_BUILTINS_STR_COUNT       (1)
#define MICROPY_PY_BUILTINS_STR_PARTITION   (1)
#define MICROPY_PY_BUILTINS_STR_SPLITLINES  (1)
#define MICROPY_PY_BUILTINS_SLICE_INDICES   (1)
#define MICROPY_PY_SYS_GETSIZEOF            (1)
#define MICROPY_PY_MICROPYTHON_MEM_INFO     (1)
#define MICROPY_PY_IO_IOBASE                (1)
#define MICROPY_PY_DEFLATE_COMPRESS         (1)
#define MICROPY_PY_BINASCII_CRC32           (1)
#define MICROPY_PY_RE_MATCH_GROUPS          (1)
#define MICROPY_PY_RE_MATCH_SPAN_START_END  (1)
#define MICROPY_PY_RE_DEBUG                 (1)
#define MICROPY_PY_RANDOM_EXTRA_FUNCS       (1)
// REPL line editing (emacs keys: Ctrl-A/E/B/F/K/U/W + history) — complements the
// arrow keys; and a handful more pure features.
#define MICROPY_REPL_EMACS_KEYS             (1)
#define MICROPY_PY_SELECT_SELECT            (1)  // select.select() alongside poll()
#define MICROPY_PY_IO_BUFFEREDWRITER        (1)
#define MICROPY_PY_MICROPYTHON_RINGIO       (1)
#define MICROPY_PY_BUILTINS_NEXT2           (1)  // next(iter, default)
#define MICROPY_PY_DELATTR_SETATTR          (1)  // __delattr__/__setattr__
#define MICROPY_PY_SYS_PS1_PS2              (1)  // sys.ps1 / sys.ps2 custom prompts
#define MICROPY_PY_SYS_TRACEBACKLIMIT       (1)
// weakref.ref()/finalize(): real weak references (the GC integration in gc.c is
// gated on this flag). EVERYTHING-level by default.
#define MICROPY_PY_WEAKREF                  (1)

// sys: path is on by default (seeds ".frozen" for frozen imports). Trim the rest.
#define MICROPY_PY_SYS_MODULES              (0)
#define MICROPY_PY_SYS_EXIT                 (1) // required by MICROPY_PY_MACHINE
#define MICROPY_PY_SYS_ARGV                 (1) // populated when launched as `app.efi script.py ...`

// TLS / ssl (MICROPY_PY_SSL / MICROPY_SSL_MBEDTLS set by the Makefile TLS= choice).
// mbedtls's bare-metal calloc/free route through the GC-tracked allocator.
#if MICROPY_PY_SSL
#define MICROPY_TRACKED_ALLOC               (1)
// Stream-only TLS here; pin DTLS off (no validated datagram transport).
#define MICROPY_PY_SSL_DTLS                 (0)
#endif

// cryptolib (aes) takes its AES impl from mbedTLS, so gate it on mbedTLS presence.
#define MICROPY_PY_CRYPTOLIB                (MICROPY_SSL_MBEDTLS)

// Type definitions for the machine.
typedef intptr_t mp_int_t;   // must be pointer-sized
typedef uintptr_t mp_uint_t; // must be pointer-sized
typedef long mp_off_t;

#define MICROPY_HW_BOARD_NAME "uefi"
#define MICROPY_HW_MCU_NAME   "x86-64/aarch64"

#define MP_STATE_PORT MP_STATE_VM
