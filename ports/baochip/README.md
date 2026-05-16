# MicroPython port for Baochip-1x

Port targeting the Baochip-1x RISC-V SoC. First supported board: Dabao evaluation board (Crowd Supply).

## Status

In bring-up. Currently provides a UART2 REPL, `machine.Pin`, and
`machine.bootloader()`.  Native USB-CDC, filesystem, and most peripheral
drivers are still to come.

## Toolchain

Either toolchain works; the Makefile auto-detects whichever is on `$PATH`,
preferring the Debian package when both are present:

* `riscv64-unknown-elf-gcc` from `gcc-riscv64-unknown-elf` (Debian/Ubuntu).
  This is what upstream CI installs via `ci_gcc_riscv_setup`.
* `riscv-none-elf-gcc` 15.2.0 or newer from xpack
  (https://github.com/xpack-dev-tools/riscv-none-elf-gcc-xpack/releases).

The Baochip-1x needs the `Zicsr` and `Zifencei` extensions explicitly enabled
in `-march`, which older GCC versions do not recognise.

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

## REPL UART limitations

The REPL UART (UART2 at 115200 8N1 by default) uses the PULP UDMA UART
in IRQ mode with a per-byte RX_CHAR interrupt.  The hardware DC FIFO is
4 bytes deep and the software ring buffer is 256 bytes; the ISR has
~30 ms of headroom at 115200 baud before incoming bytes are dropped.

Overflow is detected via the UART ERR event and increments an internal
counter (`uart_repl_rx_overflow_count`) but does not currently raise an
error to user code.  Very large pastes (>>256 bytes per ISR-service
window) may lose bytes silently.

When driving boot0/boot1 (1 Mbaud, polling RX), send char-by-char with
~2 ms gaps to avoid drops at the boot loader's polling receiver.
