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
- **Runs under** QEMU + OVMF (x64) / AAVMF (aarch64). Deployment target: coreboot +
  EDK2/UefiPayloadPkg firmware.
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

## What's implemented

- **Core + REPL** — the interpreter, GC, NLR; an interactive REPL over the UEFI console
  (`ConIn`/`ConOut`) and serial, with history/editing.
- **Filesystem** — a VFS over `EFI_SIMPLE_FILE_SYSTEM`/`EFI_FILE_PROTOCOL` mounted at `/`, plus
  `os`, `open()`, and multi-volume mounting.
- **`machine` / `uefi`** — the Pythonic UEFI API: protocols, handles, events, GUIDs, device
  paths, driver/image loading, NVRAM variables, boot options, system services, console, GOP as
  a `framebuf` (`uefi.Display`), and an `EFI_LOAD_FILE2` file-provider (`uefi.load_file`, enough
  to build a boot loader).
- **Networking** — `network` + `socket` over the native EFI transports (TCP4/UDP4/IP4 + DNS4/
  DHCP/IP4Config2), with `asyncio` integration (client + server, async `accept`). `network.WLAN`
  drives WiFi (station mode, WPA2-PSK) over EFI WiFi2 — `scan()`, non-blocking `connect()`,
  `disconnect()`, `status()` — reusing the same L3 (DHCP/sockets/TLS) path as the wired `LAN`.
  Samples cover HTTPS fetch, WiFi association, a network boot loader, and a disk boot loader.
- **TLS / `ssl`** — two backends chosen at **compile time** (`TLS=mbedtls|efi|none`, default
  `mbedtls`), presenting the same `tls`/`ssl` API:
  - `TLS=mbedtls` — vendored `lib/mbedtls`, freestanding.
  - `TLS=efi` — drives the firmware `EFI_TLS_PROTOCOL` (TlsDxe/OpenSSL), ~187 KB smaller since it
    ships no crypto. Needs the network firmware; `wrap_socket` raises `OSError(ENODEV)` without it.
  - Both backends do a full TLS handshake with certificate and hostname verification.
- **Compiled bytecode** — frozen `.mpy`, runtime `import` of external `.mpy` off the VFS, and the
  `marshal` module (`compile → dumps → loads → exec`). All arch-neutral.
- **stdlib** — `re`, `json`, `struct`, `binascii`, `hashlib`, `heapq`, `cmath`, `random`,
  `framebuf`, `deflate`, `platform`, `asyncio`, `time`, `select`, etc.

## Limitations

- **Native machine code is unavailable.** `@micropython.native`, `@micropython.viper`,
  native-code `.mpy` (natmod) and inline assembly are disabled: MicroPython has no aarch64 code
  emitter, and its x64 emitter assumes the System-V ABI while this core is `-mabi=ms`. Frozen
  `.mpy` / `marshal` bytecode is the portable speed path.
- **WiFi is station-only and IPv4.** `network.WLAN` has no AP/SoftAP mode (UEFI exposes no such
  protocol); scan results carry no BSSID or channel, and RSSI is reported as a 0–100 link quality
  rather than dBm. Association can take 10–20 s; poll `status()` until it leaves `STAT_CONNECTING`.
- **Secure Boot and aarch64-on-metal are untested.** The port runs under QEMU + OVMF/AAVMF on
  both architectures and on x86-64 platform firmware; running on aarch64 hardware and booting as a
  signed image under Secure Boot are not yet exercised.

Deferred features and finer gaps are tracked in `TODO.md`.

## Build & test

Build/dev commands run in the canonical Docker dev image (`mpy-uefi-dev`); from the host, wrap
with `make docker-<target>` or `docker run … mpy-uefi-dev make <target>`. `make test` is the
exception — it builds+stages in the container but drives QEMU **natively on the host** (so it
runs on the machine's own architecture, fast, no cross-emulation). `ARCH` is `aa64` (default)
or `x64`; `make test` picks the host's native arch automatically.

| Command | What it does |
|---|---|
| `make test` | Run the upstream test suite (including the port's own `tests/ports/uefi` checks) over the byte-clean raw REPL against QEMU on the host's **native** arch; the authoritative pass/fail signal. `TESTS=…` passes extra flags/files. |
| `make test-gfx` / `make test-net` | `make test` plus a headless display adapter (GOP/Display tests run) / a NIC + network firmware (network tests run). |
| `make test-repl` / `test-runfile` / `test-bootopt` | Interactive & launch-path smoke checks (`tests/harness.py`): drive the live REPL / a shell-launched script / a boot-option script. |
| `make run` / `run-gfx` | Interactive serial REPL / a real graphics window (host-only). |

Determinism: the port tests self-check with fixed output and a committed `.py.exp`; between
tests the runner cleanly exits the app and the EFI Shell's `startup.nsh` relaunches a fresh,
hermetic interpreter (no QEMU reset), and it meters uploads with the REPL's raw-paste flow
control (no timing guesswork). Always pass a timeout. The all-protocols network firmware
(RELEASE + the DNS/HTTP/**TLS** stack + VirtioNetDxe) is built with
`bash docker/build-ovmf-release-net.sh`.

## Layout

Everything lives under `ports/uefi/`: the C core (`main.c`, `mphalport.c`, `uefi_*.c`, `mod*.c`,
`efi*.h`), the frozen Python (`modules/`), samples (`samples/`), the test harness (`tests/`), the
firmware/dev scaffolding (`Dockerfile`, `docker/`, `scripts/`), and the build (`Makefile`). Port
work stays here; the shared core (`py/`, `extmod/`, `shared/`, `lib/`, `mpy-cross/`) is not edited.
