# Deferred work — MicroPython UEFI port

Scope consciously parked: what it is, why it's deferred, and what unblocking it takes.
Nothing here is a bug. See `README.md` for the port overview and what's implemented.

## Native machine code (blocked)

`@micropython.native`, `@micropython.viper`, native-code `.mpy` modules (natmod), and inline
assembly all emit or load **target machine code**, and share the same two blockers — so they
rise and fall together. Bytecode (`.mpy` load + `marshal`) is the portable speed/size story
until an emitter exists.

Blockers:
- **aarch64 — no emitter exists.** MicroPython has no 64-bit ARM (A64) native emitter; `py/asm*.c`
  covers x86 / x64 / thumb / arm32 / xtensa / rv32 only, and `mpy_ld.py` (the natmod linker) has
  no aarch64 backend either.
- **x86-64 — ABI conflict.** `asmx64.h` hardcodes the System-V ABI (`REG_ARG_1..4 = RDI/RSI/RDX/RCX`),
  but the core is compiled `-mabi=ms` (`RCX/RDX/R8/R9`, 32-byte shadow space, different callee-saved
  set). Emitted or loaded native code calling the runtime would pass args in the wrong registers →
  the non-deterministic GC-corruption ABI landmine (`CLAUDE.md`).

Unblock (any of these enables the whole family for the arch it covers):
- Fork `asmx64` + `emitnx64` to a Microsoft-x64 ABI variant (shadow space, MS callee-saved set) —
  x64 only; must re-validate the GC register scan.
- Write an A64 native emitter (`asmarm64.c` + `emitnarm64.c`) — covers aarch64; large.
- For natmod additionally: an `mpy_ld.py` backend for the chosen arch/ABI, plus executable-memory
  allocation for the loaded code (UEFI pages are typically RWX pre-`ExitBootServices`, but firmware
  that sets NX would need pages marked executable).

## network.WLAN (WiFi) — WPA3 / enterprise

Station-mode WPA2-PSK is implemented. Not yet supported: WPA3-SAE and enterprise/EAP
(`EFI_EAP_CONFIGURATION_PROTOCOL`) authentication, and AP/SoftAP mode (UEFI exposes no protocol
for it).

## TLS — extended tests

Wired but not yet covered: a server-side test, mutual TLS (client cert), an
`asyncio.open_connection(ssl=)` / `start_server(ssl=)` round-trip, and a real-internet smoke test.
An mbedTLS config size-trim (prune unused suites/curves) is also outstanding.

## Smaller items

- **DHCP hostname (option 12):** the hostname is stored only; not injected into the DHCP request.
- **Variable Policy:** `LOCK_ON_VAR_STATE` policies raise `NotImplementedError`.
- **Volume label / EFI compression** helpers are not exposed.
- **Live-callback protocols:** installing a protocol backed by a *live* Python callback is not
  supported (only data-backed providers, e.g. `uefi.load_file`).
- **Library API docs** (`docs/library/uefi*`) are drafted but not yet built into the Sphinx tree.
