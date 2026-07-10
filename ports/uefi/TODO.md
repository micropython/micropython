# Deferred work and missing features — MicroPython UEFI port

## Native machine code

`@micropython.native`, `@micropython.viper`, native-code `.mpy` modules (natmod), and inline
assembly are all missing. For the **aarch64** platform there is currently no native code emitter
for the CPU architecture and we will need to also implement `mpy_ld.py` (the natmod linker) for
the aarch64 backend. For **x86-64** the main blocker is that UEFI uses the Microsoft API and the
existing `asmx64.h` hardcodes the System-V ABI.

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
