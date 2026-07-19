# MicroPython on UEFI

A port of MicroPython to run as a **UEFI application** (pre-`ExitBootServices`), so the
firmware environment — console, filesystem, timers, networking — is scriptable in Python.

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
`bash tools/build-ovmf.sh --release --net`.

The dev image itself (`mpy-uefi-dev`) is built with:
```
docker build -f tools/Dockerfile -t mpy-uefi-dev tools
```
CI instead builds only the leaner `--target build` stage (toolchain only, no QEMU/venv/baked
firmware) since it drives QEMU on the runner host directly rather than inside the container.

## Layout

Everything lives under `ports/uefi/`: the C core (`main.c`, `mphalport.c`, `uefi_*.c`, `mod*.c`,
`efi*.h`), the frozen Python (`modules/`), the test harness (`tests/`), and the build
(`Makefile`). Two scaffolding dirs stay clearly separate from that source: `tools/`
(`Dockerfile`, `build-ovmf.sh`, `build-test-os.sh`) builds the toolchain/firmware and is Docker-
only; `scripts/` (`run.sh`, `debug.sh`, `run-gfx.sh`, `env.sh`) launches QEMU directly on the host
and needs no Docker. Sample scripts live in the top-level `examples/uefi/` (matching every other
port's convention), staged onto the boot volume by `make stage`. Port work stays here; the
shared core (`py/`, `extmod/`, `shared/`, `lib/`, `mpy-cross/`) is not edited.

### File naming convention

`uefi_*.c`/`uefi_port.h` is port infrastructure, not Python-visible on its own. `efi*.h` is a
hand-rolled firmware ABI header transcribing UEFI/EDK2 spec structures and protocol GUIDs.
A Python-visible module shows up as one of three shapes, matching upstream MicroPython's own
conventions (compare any other port's `modmachine.c`/`machine_wdt.c`):

- **Self-registering** — calls `MP_REGISTER_MODULE` itself: `moduefi_raw.c` -> `_uefi`,
  `modnetwork.c` -> `network`, `modsocket.c` -> `socket`, `modtls_efitls.c` -> `tls`.
- **`INCLUDEFILE` fragment** — never compiled standalone; textually `#include`d into a
  generic `extmod/*.c` file via a `MICROPY_PY_*_INCLUDEFILE` macro, which does the actual
  registration: `modmachine.c` -> `extmod/modmachine.c`, `modtime.c` -> `extmod/modtime.c`,
  `machine_wdt.c` -> `extmod/machine_wdt.c`. Every upstream port names these the same way
  (`modmachine.c`/`modtime.c` keep the `mod` prefix, `machine_wdt.c` doesn't) — it's
  precedent, not a rule tied to the mechanism.
- **Satellite type file** — compiled standalone with no `MP_REGISTER_MODULE` of its own,
  wired into a sibling module's globals table instead: `machine_timer.c`/`machine_rtc.c` ->
  `modmachine.c`'s `MICROPY_PY_MACHINE_EXTRA_GLOBALS`, `uefi_event_obj.c` ->
  `moduefi_raw.c`'s globals table.

### File map

The C sources fall into a few layers; within each, files are listed innermost/lowest-level
first.

**Entry point & port configuration** — how the port itself is set up and built.
- `main.c` — entry point: heap/GC/stack setup, launch-arg parsing, REPL dispatch, clean-exit contract
- `mpconfigport.h` — `MICROPY_PY_*` feature flags, ABI/GC/scheduler hooks
- `qstrdefsport.h` — port-specific qstrs (none needed yet)

**Hardware abstraction** - standard interfaces for required hardware
- `mphalport.h` / `mphalport.c` — the HAL: console supporting UEFI 16-bit IO, `Stall`-based delay

**Standard MicroPython module implementations** - the modules Python code actually imports.
- `modmachine.c` (+ `machine_timer.c`, `machine_rtc.c`, `machine_wdt.c`) — `machine`: idle/reset/freq/sleep/unique_id, wires in `Timer`/`RTC`/`WDT`
- `modnetwork.c` — `network`: interface up/down, static/DHCP IP config
- `modsocket.c` — `socket`: address constants, `getaddrinfo`, the socket stream type
- `modtime.c` — `time` wall-clock fragment (`#include`d into `extmod/modtime.c`, not compiled standalone)
- `modtls_efitls.c` — `tls`, `EFI_TLS_PROTOCOL` backend (sibling to `extmod/modtls_{mbedtls,axtls}.c`)

**EFI wrappers** — low-level firmware ABI up to the high-level Python-facing types built on it.
- `efi.h`, `efi_net.h`, `efi_tls.h` — hand-rolled UEFI/EDK2 protocol & struct definitions
- `moduefi_raw.c` — `_uefi`: the raw Boot/Runtime Services surface + the `call` EFIAPI trampoline
- `uefi_event_obj.c` — `uefi.Event`: the pollable high-level event type built on it

**UEFI-environment-to-library-API glue** — turning firmware services into what MicroPython's
core/HAL expects (`time`, VFS, RNG, GC, the scheduler's blocking primitive).
- `uefi_time.c` — hardware timebase behind `time`/`machine.Timer`
- `uefi_event.c` — the WFE blocking primitive, timer/signal-event primitives, byte-clean serial REPL
- `uefi_vfs.c` — VFS backend over `EFI_SIMPLE_FILE_SYSTEM_PROTOCOL`
- `uefi_net.c` — shared plumbing behind `network`/`socket` (async completion helpers)
- `uefi_rng.c` — entropy behind `os.urandom()`
- `uefi_gccollect.c` — conservative-GC root scan (register dump is ABI-specific)
- `uefi_port.h` — shared declarations for the above (live UEFI tables, test-exit path)

**Toolchain / freestanding-build scaffolding** — gaps from targeting `*-unknown-windows` with
no OS underneath.
- `libc_extra.c`, `math_extra.c`, `uefi_stubs.c` — libc/compiler-support functions the shared core needs
- `include/*.h` — libc/Win headers the clang triple expects (`alloca`, `assert`, `errno`, `inttypes`, `math`, `stdio`, `stdlib`, `string`, `time`, `unistd`, `windows`, `winsock2`, `ws2tcpip`)
- `mbedtls/mbedtls_config_port.h`, `uefi_mbedtls_port.c` — mbedTLS build config + platform hooks (`TLS=mbedtls`)
