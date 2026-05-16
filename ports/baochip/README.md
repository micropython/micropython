# MicroPython port for Baochip-1x

Port targeting the Baochip-1x RISC-V SoC. First supported board: Dabao evaluation board (Crowd Supply).

## Status

In bring-up. Currently provides a UART2 REPL, `machine.Pin`, and
`machine.bootloader()`.  Native USB-CDC, filesystem, and most peripheral
drivers are still to come.

## Toolchain

xpack `riscv-none-elf-gcc` 15.2.0 must be on `$PATH`. The Baochip-1x needs the `Zicsr` and `Zifencei` extensions explicitly enabled in `-march`, which older GCC versions do not recognise.

Get the toolchain from:
https://github.com/xpack-dev-tools/riscv-none-elf-gcc-xpack/releases

## Submodule

Fetch the vendor SDK before the first build:

```
make BOARD=DABAO submodules
```

This populates `lib/dabao-sdk` (Apache-2.0).

## Build

```
make BOARD=DABAO
```

Produces `build-DABAO/firmware.elf`, `build-DABAO/firmware.bin`, and the signed `build-DABAO/firmware.uf2` (signed with the SDK developer key).

## Flash

See `boards/DABAO/deploy.md` for full flashing instructions.

The quickest path from a running MicroPython REPL:

```bash
import machine; machine.bootloader()
```

Then, once the board is in boot1 mode:

```bash
DABAO_PORT=/dev/ttyACM0 make BOARD=DABAO deploy
```
