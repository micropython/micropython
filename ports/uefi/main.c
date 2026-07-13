// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Nicko van Someren

// MicroPython UEFI port — entry point.
//
// efi_main sets up the heap + GC and reaches mp_init(), then runs the interactive REPL
// (on the ConOut console, or the byte-clean serial port with `--serial`) or a script
// passed on the command line (`app.efi foo.py`). The test suite drives the serial REPL
// (see tests/run_uefi_tests.py); there is no baked-in self-test.

#include <stdint.h>
#include <string.h>

#include "py/compile.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/gc.h"
#include "py/stackctrl.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "shared/runtime/pyexec.h"
#include "extmod/vfs.h"

#include "efi.h"
#include "uefi_port.h"

// Live UEFI tables, set on entry.
EFI_SYSTEM_TABLE *mp_uefi_st;
EFI_HANDLE mp_uefi_image;

static char *stack_top;

// Heap: page allocation owned entirely by MicroPython's GC. Sized at runtime to
// half of free conventional RAM (see efi_main), floored to MPY_UEFI_HEAP_MIN and
// retried smaller if the firmware can't satisfy the request.
#define MPY_UEFI_HEAP_MIN (2 * 1024 * 1024)
// Interpreter C-stack limit for MICROPY_STACK_CHECK: the point at which deep Python
// recursion raises RuntimeError instead of overrunning the real C stack (a hard CPU
// fault). The real stack size is discovered at runtime from the firmware's HOB list
// (uefi_detect_stack_limit); we set the limit to (available below stack_top) minus a
// guard, so C frames below the limit — including a shallow-depth mbedTLS call — have
// room. If the HOB isn't available we fall back to a value safe on the UEFI-guaranteed
// 128 KiB minimum. This is unrelated to the heap, which is megabytes (MPY_UEFI_HEAP_MIN).
#define MPY_UEFI_STACK_GUARD          (32 * 1024)
#define MPY_UEFI_STACK_LIMIT_FALLBACK (96 * 1024)


static void run_repl(void) {
    mp_hal_stdout_tx_str("\r\nMicroPython on UEFI\r\n");
    mp_hal_stdout_tx_str("Type help() for more information; Ctrl-D to exit.\r\n");
    mp_hal_stdout_tx_str("Samples: exec(open('/samples/sysinfo.py').read())  "
        "(see /samples/README.md)\r\n");
    for (;;) {
        int ret = (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL)
            ? pyexec_raw_repl() : pyexec_friendly_repl();
        if (ret != 0) {
            // A forced exit (SystemExit / sys.exit() / Ctrl-D on an empty line). WHERE it
            // came from decides what to do:
            //  * From the RAW REPL, in serial mode: a *test* raised it (e.g. the inlineasm.py
            //    feature check does `raise SystemExit` on a target without inline asm). The
            //    automated harness is mid raw-REPL session and expects the interpreter to stay
            //    alive and in raw REPL, so loop back into a fresh raw REPL rather than
            //    returning. (Per-test isolation still holds: the harness relaunches the app
            //    between tests — see below.) Returning here instead would drop us to a fresh
            //    friendly REPL out from under the harness, desyncing it.
            //  * From the FRIENDLY REPL: the real request to quit. Return to the caller (the
            //    clean-exit contract: an application returns control to whoever launched it, it
            //    does not power off). This is how the harness ends each test — it drops out of
            //    raw REPL with Ctrl-B, then sends Ctrl-D at the friendly prompt — so the EFI
            //    shell's startup.nsh loop relaunches us as a fresh, hermetic interpreter with
            //    no QEMU reset (far faster, and it sidesteps a wedge that repeated resets hit).
            if (mp_uefi_serial_repl && pyexec_mode_kind == PYEXEC_MODE_RAW_REPL) {
                continue;
            }
            break;
        }
    }
}


// Stop all background activity before tearing down the interpreter: machine
// timers first (their notifies schedule into the VM), then the core async
// machinery (ConIn poll timer + wait events).
static void mpy_uefi_teardown(void) {
    #if MICROPY_PY_MACHINE
    machine_timer_deinit_all();
    #endif
    efi_event_deinit_all();
    mp_uefi_async_deinit();
}

// ── Script-on-the-command-line support (`app.efi foo.py [args]`) ────────────
// Narrow a null-terminated CHAR16 into `out` (ASCII; non-ASCII -> '?'). Returns
// the length written (excluding the terminator).
static size_t char16_to_cstr(const CHAR16 *w, char *out, size_t cap) {
    size_t n = 0;
    while (w[n] != 0 && n + 1 < cap) {
        CHAR16 c = w[n];
        out[n] = (c < 0x80) ? (char)c : '?';
        n++;
    }
    out[n] = 0;
    return n;
}

// The shell installs EFI_SHELL_PARAMETERS_PROTOCOL on our image handle to pass
// argc/argv. Absent when launched by the boot manager (no args) -> NULL.
static EFI_SHELL_PARAMETERS_PROTOCOL *uefi_shell_params(void) {
    static const EFI_GUID g = EFI_SHELL_PARAMETERS_PROTOCOL_GUID;
    void *p = NULL;
    if (EFI_ERROR(mp_uefi_st->BootServices->HandleProtocol(mp_uefi_image, (EFI_GUID *)&g, &p))) {
        return NULL;
    }
    return (EFI_SHELL_PARAMETERS_PROTOCOL *)p;
}

// Split a CHAR16 command line into argv in place (whitespace-separated; no quoting):
// writes NULs between tokens in `cmd` and fills `argv[]` up to `max`. Returns the
// token count.
static UINTN tokenize_cmdline(CHAR16 *cmd, CHAR16 **argv, UINTN max) {
    UINTN n = 0;
    for (CHAR16 *p = cmd; *p && n < max;) {
        while (*p == 0x20 || *p == 0x09) {  // skip leading whitespace
            p++;
        }
        if (*p == 0) {
            break;
        }
        argv[n++] = p;
        while (*p && *p != 0x20 && *p != 0x09) {
            p++;
        }
        if (*p) {
            *p++ = 0;   // terminate this token
        }
    }
    return n;
}

// Compare a null-terminated CHAR16 token against an ASCII literal.
static bool char16_eq_ascii(const CHAR16 *w, const char *a) {
    while (*a) {
        if (*w != (CHAR16)(uint8_t)*a) {
            return false;
        }
        w++;
        a++;
    }
    return *w == 0;
}

// Arguments the app was launched with, as a CHAR16 argv where argv[0] is the script.
// The EFI shell pre-parses them into ShellParameters (with argv[0] = our image name,
// which we skip); the boot manager instead passes them raw in LoadedImage->LoadOptions
// (a CHAR16 command line, script first) — which the shell path never sees.
//
// Leading `--serial`/`--console` options are consumed here to select the REPL transport
// (*serial_out; defaults to the build's MPY_UEFI_SERIAL_REPL setting) so they don't reach
// sys.argv or get mistaken for a script. Returns the remaining token count and sets
// *argv_out to the first non-option token, or 0 for a bare launch (-> REPL).
static UINTN uefi_launch_argv(CHAR16 ***argv_out, bool *serial_out) {
    static CHAR16 lo_buf[512];
    static CHAR16 *lo_argv[16];

    CHAR16 **argv;
    UINTN argc;
    EFI_SHELL_PARAMETERS_PROTOCOL *sp = uefi_shell_params();
    if (sp != NULL && sp->Argc >= 2) {
        argv = &sp->Argv[1];            // skip Argv[0] (our image name)
        argc = sp->Argc - 1;
    } else {
        EFI_LOADED_IMAGE_PROTOCOL *li = NULL;
        static const EFI_GUID g_li = EFI_LOADED_IMAGE_PROTOCOL_GUID;
        if (EFI_ERROR(mp_uefi_st->BootServices->HandleProtocol(mp_uefi_image, (EFI_GUID *)&g_li, (void **)&li)) ||
            li == NULL || li->LoadOptions == NULL || li->LoadOptionsSize < sizeof(CHAR16)) {
            argv = lo_argv;
            argc = 0;
        } else {
            UINTN n = li->LoadOptionsSize / sizeof(CHAR16);
            if (n > MP_ARRAY_SIZE(lo_buf) - 1) {
                n = MP_ARRAY_SIZE(lo_buf) - 1;
            }
            memcpy(lo_buf, li->LoadOptions, n * sizeof(CHAR16));
            lo_buf[n] = 0;
            argv = lo_argv;
            argc = tokenize_cmdline(lo_buf, lo_argv, MP_ARRAY_SIZE(lo_argv));
        }
    }

    #ifdef MPY_UEFI_SERIAL_REPL
    bool serial = true;
    #else
    bool serial = false;
    #endif
    // Strip leading transport options; the last one on the line wins.
    while (argc > 0) {
        if (char16_eq_ascii(argv[0], "--serial")) {
            serial = true;
        } else if (char16_eq_ascii(argv[0], "--console")) {
            serial = false;
        } else {
            break;
        }
        argv++;
        argc--;
    }
    if (serial_out != NULL) {
        *serial_out = serial;
    }
    *argv_out = argv;
    return argc;
}

// If launched with a script argument (`app.efi foo.py [args]`, from the shell OR a
// boot option), set sys.argv and run that file — like `python foo.py` — and return 1.
// Otherwise return 0 (REPL).
static int uefi_run_script(CHAR16 **argv, UINTN argc) {
    if (argc == 0) {
        return 0;
    }
    char buf[260];
    // sys.argv = [script, arg1, ...].
    mp_obj_list_init(MP_OBJ_TO_PTR(mp_sys_argv), 0);
    for (UINTN i = 0; i < argc; i++) {
        size_t n = char16_to_cstr(argv[i], buf, sizeof(buf));
        mp_obj_list_append(mp_sys_argv, mp_obj_new_str(buf, n));
    }
    // Open via the VFS (mounted at "/"); accept shell-style backslashes too.
    char path[260];
    char16_to_cstr(argv[0], path, sizeof(path));
    for (char *c = path; *c; c++) {
        if (*c == '\\') {
            *c = '/';
        }
    }
    pyexec_file(path);
    return 1;
}

// Total free conventional RAM (bytes), from the UEFI memory map. 0 if unavailable.
static uint64_t uefi_conventional_free(EFI_BOOT_SERVICES *bs) {
    UINTN size = 0, mapkey = 0, descsize = 0;
    UINT32 descver = 0;
    if (bs->GetMemoryMap(&size, NULL, &mapkey, &descsize, &descver) != EFI_BUFFER_TOO_SMALL) {
        return 0;
    }
    size += descsize * 8; // slack: allocating the buffer can grow the map
    EFI_MEMORY_DESCRIPTOR *map = NULL;
    if (EFI_ERROR(bs->AllocatePool(EfiLoaderData, size, (void **)&map)) || map == NULL) {
        return 0;
    }
    uint64_t free_pages = 0;
    if (!EFI_ERROR(bs->GetMemoryMap(&size, map, &mapkey, &descsize, &descver)) && descsize != 0) {
        for (UINT8 *p = (UINT8 *)map; p + descsize <= (UINT8 *)map + size; p += descsize) {
            EFI_MEMORY_DESCRIPTOR *d = (EFI_MEMORY_DESCRIPTOR *)p; // iterate by descsize, not sizeof
            if (d->Type == EfiConventionalMemory) {
                free_pages += d->NumberOfPages;
            }
        }
    }
    bs->FreePool(map);
    return free_pages * EFI_PAGE_SIZE;
}

// --- Runtime stack-size discovery via the HOB list ---------------------------------
// EDK2 publishes the Phase Handoff Information Block (HOB) list as a configuration
// table, and it contains a memory-allocation HOB describing the boot stack region.
// Reading it lets us set the MicroPython stack limit to the *actual* stack we were
// given (minus a guard) rather than assuming the 128 KiB spec minimum.
#define EFI_HOB_LIST_GUID \
    {0x7739F24C, 0x93D7, 0x11D4, {0x9A, 0x3A, 0x00, 0x90, 0x27, 0x3F, 0xC1, 0x4D}}
#define EFI_HOB_MEMORY_ALLOC_STACK_GUID \
    {0x4ED4BF27, 0x4092, 0x42E9, {0x80, 0x7D, 0x52, 0x7B, 0x1D, 0x00, 0xC9, 0xBD}}
#define EFI_HOB_TYPE_MEMORY_ALLOCATION 0x0002
#define EFI_HOB_TYPE_END_OF_HOB_LIST   0xFFFF

typedef struct {
    UINT16 HobType;
    UINT16 HobLength;
    UINT32 Reserved;
} EFI_HOB_GENERIC_HEADER;

typedef struct {
    EFI_HOB_GENERIC_HEADER Header;
    EFI_GUID Name;
    EFI_PHYSICAL_ADDRESS MemoryBaseAddress;
    UINT64 MemoryLength;
    UINT32 MemoryType;
    UINT8 Reserved[4];
} EFI_HOB_MEMORY_ALLOCATION;

// Compute the MicroPython C-stack limit from the real stack region. Walks the HOB list
// for the stack allocation HOB; returns (stack_top - stack_base - guard) when the
// current stack pointer lies within that region, else the conservative fallback.
static size_t uefi_detect_stack_limit(const char *sp) {
    static const EFI_GUID g_hoblist = EFI_HOB_LIST_GUID;
    static const EFI_GUID g_stack = EFI_HOB_MEMORY_ALLOC_STACK_GUID;

    const uint8_t *hob = NULL;
    for (UINTN i = 0; i < mp_uefi_st->NumberOfTableEntries; i++) {
        EFI_CONFIGURATION_TABLE *t = &mp_uefi_st->ConfigurationTable[i];
        if (memcmp(&t->VendorGuid, &g_hoblist, sizeof(EFI_GUID)) == 0) {
            hob = (const uint8_t *)t->VendorTable;
            break;
        }
    }
    while (hob != NULL) {
        const EFI_HOB_GENERIC_HEADER *h = (const EFI_HOB_GENERIC_HEADER *)hob;
        if (h->HobLength == 0 || h->HobType == EFI_HOB_TYPE_END_OF_HOB_LIST) {
            break;
        }
        if (h->HobType == EFI_HOB_TYPE_MEMORY_ALLOCATION) {
            const EFI_HOB_MEMORY_ALLOCATION *m = (const EFI_HOB_MEMORY_ALLOCATION *)hob;
            if (memcmp(&m->Name, &g_stack, sizeof(EFI_GUID)) == 0) {
                const char *base = (const char *)(uintptr_t)m->MemoryBaseAddress;
                const char *end = base + m->MemoryLength;
                // Only trust it if our stack pointer is actually inside this region.
                if (sp > base && sp <= end) {
                    size_t avail = (size_t)(sp - base);
                    if (avail > MPY_UEFI_STACK_GUARD + 16 * 1024) {
                        return avail - MPY_UEFI_STACK_GUARD;
                    }
                }
                break;
            }
        }
        hob += h->HobLength;
    }
    return MPY_UEFI_STACK_LIMIT_FALLBACK;
}

EFI_STATUS EFIAPI efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *system_table) {
    mp_uefi_st = system_table;
    mp_uefi_image = image_handle;
    EFI_BOOT_SERVICES *bs = system_table->BootServices;

    // We are a guest holding the platform watchdog — disarm it.
    bs->SetWatchdogTimer(0, 0, 0, NULL);

    int stack_dummy;
    stack_top = (char *)&stack_dummy;

    // Size the GC heap from real memory: half of free conventional RAM, leaving the
    // rest for the firmware / driver loads. Fall back to 64 MiB if the map is
    // unreadable, then retry-halve until the firmware can satisfy the request.
    uint64_t freebytes = uefi_conventional_free(bs);
    uint64_t want = freebytes ? (freebytes / 2) : (64ULL * 1024 * 1024);
    if (want < MPY_UEFI_HEAP_MIN) {
        want = MPY_UEFI_HEAP_MIN;
    }
    UINTN pages = (UINTN)(want / EFI_PAGE_SIZE);
    EFI_PHYSICAL_ADDRESS addr = 0;
    EFI_STATUS s = EFI_OUT_OF_RESOURCES;
    while (pages >= MPY_UEFI_HEAP_MIN / EFI_PAGE_SIZE) {
        s = bs->AllocatePages(AllocateAnyPages, EfiLoaderData, pages, &addr);
        if (!EFI_ERROR(s)) {
            break;
        }
        pages /= 2;
    }
    if (EFI_ERROR(s) || addr == 0) {
        mp_hal_stdout_tx_str("MPY-UEFI: heap AllocatePages failed\r\n");
        mp_uefi_exit(0);
    }
    void *heap = (void *)(uintptr_t)addr;
    UINTN heap_size = pages * EFI_PAGE_SIZE;

    mp_stack_set_top(stack_top);
    mp_stack_set_limit(uefi_detect_stack_limit(stack_top));
    gc_init(heap, (char *)heap + heap_size);
    mp_init();

    // Bring up the timebase + event machinery (stdin poll timer, wait/wakeup
    // events) now that the VM is live and can take scheduled callbacks.
    mp_uefi_async_init();

    // Mount the boot volume at "/" (best-effort; non-fatal if absent).
    mp_uefi_vfs_mount_boot();

    // Resolve the launch args and the REPL transport (serial vs. ConOut console) in one
    // pass: `--serial`/`--console` are stripped here, the rest is the optional script.
    CHAR16 **argv;
    bool serial;
    UINTN argc = uefi_launch_argv(&argv, &serial);
    if (serial) {
        // The interactive REPL is about to move to the serial port: ConOut is byte-lossy
        // for the raw-REPL protocol (its TerminalDxe layer mangles control bytes), so we
        // drive the UART directly. Leave a note on the ConOut console first — before we
        // take the serial away from TerminalDxe — so a user who booted this build on a
        // machine with a display isn't left staring at a dead prompt, and knows how to
        // force the REPL back here. The wording deliberately avoids the interpreter's own
        // "MicroPython on UEFI" banner text: on single-UART targets (e.g. aarch64 `virt`)
        // this console and the serial are the same wire, and a test harness keys on that
        // exact banner to detect the live interpreter.
        EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *out = mp_uefi_st->ConOut;
        out->OutputString(out, L"\r\n[MicroPython REPL is on the serial port; this console "
            L"is inactive.\r\nRe-launch with `--console` to use the REPL here instead.]\r\n");
        // If no serial hardware is present, mp_uefi_serial_init leaves mp_uefi_serial_repl
        // false and the HAL stays on this ConOut console (the banner above notwithstanding).
        mp_uefi_serial_init();
    }
    // `app.efi foo.py [args]` runs the script and exits; bare launch -> REPL.
    if (!uefi_run_script(argv, argc)) {
        run_repl();
    }
    mpy_uefi_teardown();
    mp_deinit();
    // Return the heap pages to the firmware now the interpreter is gone (it was
    // AllocatePages, so this must be FreePages, not FreePool). Safe: the teardown
    // above closed every UEFI event/timer, so nothing can fire into it.
    bs->FreePages(addr, pages);
    // Clean exit: an application returns control to its caller (the shell, or the
    // boot manager) — it must NOT power the machine off. The marker lets the test
    // harness detect the exit without depending on what the caller does next.
    mp_hal_stdout_tx_str("MPY-UEFI: exit\r\n");
    return EFI_SUCCESS;
}

// Text printed by help() (MICROPY_PY_BUILTINS_HELP_TEXT).
const char uefi_help_text[] =
    "Welcome to MicroPython on UEFI!\n"
    "\n"
    "For online docs please visit http://docs.micropython.org/\n"
    "\n"
    "Control commands:\n"
    "  CTRL-C        -- interrupt a running program\n"
    "  CTRL-D        -- on a blank line, exit\n"
    "\n"
    "The filesystem of the boot volume is mounted at '/'. Useful modules:\n"
    "  os, time, machine, efi, asyncio, select, json, re, struct, hashlib\n"
    "\n"
    "For a list of available modules, type help('modules').\n";

// gc_collect() lives in uefi_gccollect.c (register dump is ABI-specific).

// mp_import_stat() and the open() builtin are provided inline by py/builtin.h
// (routing to the VFS) when MICROPY_VFS is enabled; the lexer's file reader comes
// from extmod/vfs_reader.c via MICROPY_READER_VFS. The boot volume is mounted in
// efi_main() by mp_uefi_vfs_mount_boot().

void nlr_jump_fail(void *val) {
    (void)val;
    mp_hal_stdout_tx_str("MPY-UEFI: FATAL nlr_jump_fail\r\n");
    mp_uefi_exit(0);
}

void MP_NORETURN __fatal_error(const char *msg) {
    mp_hal_stdout_tx_str("MPY-UEFI: FATAL ");
    mp_hal_stdout_tx_str(msg);
    mp_hal_stdout_tx_str("\r\n");
    mp_uefi_exit(0);
}

// ---- test-harness exit path (mirrors tests/sanity) ----
#if defined(__x86_64__)
static inline void qemu_isa_debug_exit(uint8_t code) {
    __asm__ __volatile__ ("outb %0, %1" : : "a" (code), "Nd" ((uint16_t)0xf4)); // codespell:ignore
}
#endif

void mp_uefi_exit(int success) {
    #if defined(__x86_64__)
    qemu_isa_debug_exit(success ? 0x10 : 0x11); // -> host exit 33 (pass) / 35 (fail)
    #endif
    mp_uefi_st->RuntimeServices->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);
    for (;;) {
        __asm__ __volatile__ ("");
    }
}
