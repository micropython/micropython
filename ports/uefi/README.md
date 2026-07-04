# MicroPython on UEFI

A port of MicroPython to run as a **UEFI application** (pre-`ExitBootServices`), so the
firmware environment — console, filesystem, timers, networking — is scriptable in Python.
Developed **in-tree** at `ports/uefi/` in a fork of MicroPython, with the intent to upstream.

This is the single high-level description of the port. For the agent operating manual (build
rules, the ABI landmine, what-not-to-touch) see `CLAUDE.md`; for the running list of deferred
work see `TODO.md`.

## Targets & toolchain

- **Two architectures:** x86-64 (Microsoft x64 ABI) and aarch64 (standard AAPCS64). aarch64
  runs natively under QEMU on Apple Silicon, so it is the fast inner-loop target; x86-64 is
  exercised via TCG emulation / CI and is the real-hardware target.
- **Toolchain:** standalone `clang` cross-targeting PE32+, linked by `lld-link` into an EFI
  application. No EDK2 `.inf`; the optional `edk2` submodule is used for headers/firmware only.
- **Validated under** QEMU + OVMF (x64) / AAVMF (aarch64). Deployment target: System76 Lemur
  Pro (coreboot + EDK2/UefiPayloadPkg).
- **Double-precision float** via the bundled `lib/libm_dbl`.

### The x86-64 ABI landmine
The x64 core is compiled `-mabi=ms`. NLR (`nlrx64.c`) and the GC register scan
(`uefi_gccollect.c`) therefore use the **Microsoft x64 callee-saved set** (RBX/RBP/RDI/RSI/
R12–R15, XMM6–XMM15). A mismatch causes non-deterministic GC corruption that masquerades as an
interpreter bug — suspect the ABI first when GC misbehaves on x64. aarch64 is ordinary AAPCS64
with no such quirk. This constraint also blocks the native code emitter (see below).

## Execution & concurrency model

A UEFI app runs ring-0, flat-mapped, single-threaded and event-driven. The port bridges this to
MicroPython's event/poll model: completion notifies run at raised TPL (ISR-like — no heap/VM
access), flip a flag and nudge an idle loop (`mp_uefi_wake`); the interpreter sleeps in
`WaitForEvent` (`mp_uefi_wfe`) and drains scheduled work via `mp_handle_pending`. This gives
`asyncio`, non-blocking I/O, `machine.Timer` callbacks, and Ctrl-C that breaks into running code.

## What's implemented (all green on aa64 + x64)

- **Core + REPL** — the interpreter, GC, NLR; an interactive REPL over the UEFI console
  (`ConIn`/`ConOut`) and serial, with history/editing.
- **Filesystem** — a VFS over `EFI_SIMPLE_FILE_SYSTEM`/`EFI_FILE_PROTOCOL` mounted at `/`, plus
  `os`, `open()`, and multi-volume mounting.
- **`machine` / `uefi`** — the Pythonic UEFI API: protocols, handles, events, GUIDs, device
  paths, driver/image loading, NVRAM variables, boot options, system services, console, GOP as
  a `framebuf` (`uefi.Display`), and an `EFI_LOAD_FILE2` file-provider (`uefi.load_file`, enough
  to build a boot loader).
- **Networking** — `network` + `socket` over the native EFI transports (TCP4/UDP4/IP4 + DNS4/
  DHCP/IP4Config2), with `asyncio` integration (client + server, async `accept`). Samples cover
  HTTPS fetch, a network boot loader, and a disk boot loader.
- **TLS / `ssl`** — two backends chosen at **compile time** (`TLS=mbedtls|efi|none`, default
  `mbedtls`), presenting the same `tls`/`ssl` API:
  - `TLS=mbedtls` — vendored `lib/mbedtls`, freestanding.
  - `TLS=efi` — drives the firmware `EFI_TLS_PROTOCOL` (TlsDxe/OpenSSL), ~187 KB smaller since it
    ships no crypto. Needs the network firmware; `wrap_socket` raises `OSError(ENODEV)` without it.
  - Full handshake + verify parity is proven live (client echo / verify-OK / hostname-reject)
    against a host TLS peer, plus a hermetic suite, under both backends on both arches.
- **Compiled bytecode** — frozen `.mpy`, runtime `import` of external `.mpy` off the VFS, and the
  `marshal` module (`compile → dumps → loads → exec`). All arch-neutral.
- **stdlib** — `re`, `json`, `struct`, `binascii`, `hashlib`, `heapq`, `cmath`, `random`,
  `framebuf`, `deflate`, `platform`, `asyncio`, `time`, `select`, etc.

## What's left

Nothing structural — no missing core capability and no known correctness gap on either arch. The
remaining items are validation, process, and self-contained features. See `TODO.md` for detail.

- **Real-hardware validation** — everything is QEMU-validated; the Lemur Pro (and Secure Boot on
  metal) is the outstanding validation step.
- **Upstreaming** — a clean PR against current `micropython/master`, wiring into upstream CI, and
  running the full upstream `tests/` corpus on-device (the port runs a curated selftest today).
- **`network.WLAN` (WiFi)** — hardware-gated; QEMU has no 802.11, so `connect()` needs a rig.
- **Native machine code** — `@micropython.native` / `@micropython.viper`, native-code `.mpy`
  (natmod), and inline assembly are **deliberately off**: MicroPython has no aarch64 emitter, and
  its x64 emitter is System-V-ABI vs our `-mabi=ms` core (the landmine). Bytecode is the portable
  speed story. Unblock paths are in `TODO.md`.
- **Smaller deferrals** — extended TLS live tests (server / mutual-TLS / asyncio-`ssl` /
  real-internet) and an mbedTLS config size-trim; DHCP hostname option 12; a couple of variable-
  policy modes; volume-label and EFI-compression helpers; installing protocols backed by a *live*
  Python callback (only data-backed providers are done); library API docs (`docs/library/uefi*`).

## Build & test

All commands run in the canonical Docker dev image (`mpy-uefi-dev`); from the host, wrap with
`make docker-<target>` or `docker run … mpy-uefi-dev make <target>`. `ARCH` is `aa64` (default)
or `x64`.

| Command | What it does |
|---|---|
| `make test` | Build + boot the headless selftest in QEMU; the authoritative pass/fail signal. |
| `make test-net` | `make test` on the network firmware (virtio-net on SLIRP) — runs the DHCP/DNS/socket/TLS tests and the host-driven server + TLS-peer tests. |
| `make test-gfx` | `make test` with a display adapter so the GOP path runs (headless). |
| `make test-repl` | Drive the live serial REPL with pexpect. |
| `make test-tls` | Build + run the selftest under each TLS backend in turn (parity gate). |
| `make run` / `run-gfx` | Interactive serial REPL / a real graphics window (host-only). |

Determinism: tests are hermetic (frozen selftest + an exit device — x64 `isa-debug-exit`, aa64 a
serial sentinel), never timing-dependent; always pass a timeout. The network firmware is built
separately with `bash docker/build-ovmf-net.sh` (adds the DNS/HTTP/**TLS** stack + VirtioNetDxe).

## Layout

Everything lives under `ports/uefi/`: the C core (`main.c`, `mphalport.c`, `uefi_*.c`, `mod*.c`,
`efi*.h`), the frozen Python (`modules/`), samples (`samples/`), the test harness (`tests/`), the
firmware/dev scaffolding (`Dockerfile`, `docker/`, `scripts/`), and the build (`Makefile`). Port
work stays here; the shared core (`py/`, `extmod/`, `shared/`, `lib/`, `mpy-cross/`) is not edited.
