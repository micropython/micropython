# MicroPython port for GigaDevice GD32F4xx

Based on the MicroPython v1.30.0 core with the official
[GD32F4xx standard peripheral library](https://github.com/GigaDevice-GD32-MCU/GD32F4xx_Firmware_Library) (V3.3.2).

## Supported boards

| Board      | MCU         | Flash | RAM   | Core clock | REPL UART        |
|------------|-------------|-------|-------|------------|------------------|
| GD32F407   | GD32F407VE  | 512K  | 128K  | 168 MHz    | USART0 PA9/PA10  |
| GD32F470   | GD32F470VE  | 512K  | 192K  | 200 MHz    | USART0 PA9/PA10  |

Both targets share the same port implementation; only the board
definition (startup file, linker script, clock setup) differs.

## Requirements

```bash
sudo apt-get install -y gcc-arm-none-eabi binutils-arm-none-eabi \
                        libnewlib-arm-none-eabi libnewlib-dev
```

## Build

```bash
cd ports/gd32

# build + export artifacts into dist/<BOARD>/
make BOARD=GD32F407 -j1 dist
make BOARD=GD32F470 -j1 dist
```

Artifacts land in `dist/GD32F407/` and `dist/GD32F470/`:

- `firmware.elf` — ELF with debug info
- `firmware.bin` — raw binary, flash at `0x08000000`
- `firmware.hex` — Intel HEX

> **Note on `-j1`**: builds are serial because this port has been validated
> on a 2 GB host. On a larger machine you can raise the job count.

## Flashing

Any SWD probe works (ST-Link, J-Link, DAPLink):

```bash
# OpenOCD example
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
        -c "program dist/GD32F407/firmware.elf verify reset exit"
```

GD32F4 is largely register-compatible with STM32F4, so the `stm32f4x`
OpenOCD target generally works for flashing and debugging.

## Wiring the REPL

Connect a USB-serial adapter to the board:

| Adapter | MCU pin | Function   |
|---------|---------|------------|
| RX      | PA9     | USART0 TX  |
| TX      | PA10    | USART0 RX  |
| GND     | GND     | ground     |

Open the port at **115200 8N1**. You should see:

```
GD32F407 with GD32F407VE
MicroPython v1.30.0 on ...
>>>
```

## Using machine.Pin

Pin names come from `boards/<BOARD>/pins.csv` and are also addressable
by encoded integer (`port << 4 | num`).

```python
from machine import Pin

led = Pin("PA0", Pin.OUT)      # or Pin(0, Pin.OUT)
led.value(1)                    # drive high
led.on()                        # alias
led.off()
led.toggle()

btn = Pin("PC13", Pin.IN, Pin.PULL_UP)
print(btn.value())              # 0 when pressed (active low)

print(Pin("PA0"))               # Pin(PA0)
```

Available constants: `Pin.IN`, `Pin.OUT`, `Pin.OPEN_DRAIN`,
`Pin.PULL_UP`, `Pin.PULL_DOWN`, `Pin.PULL_NONE`.

## Memory layout (GD32F407, 512K/128K)

```
FLASH 0x08000000  512K   firmware
RAM   0x20000000  128K   data + bss + heap + stack
TCMRAM 0x10000000  64K   (unused by default)
```

Linker scripts live in `ld/` and are selected per board via
`LD_SCRIPT` in `boards/<BOARD>/mpconfigboard.mk`.

## Port layout

```
ports/gd32/
├── Makefile                 build system
├── main.c                   entry point, REPL loop
├── mphalport.c/.h           HAL: UART REPL, SysTick, delays
├── machine_pin.c            machine.Pin implementation
├── modmachine_port.c        port hooks for extmod/modmachine.c
├── pin_defs.h               pin encoding helpers
├── gen_pins.py              pins.csv -> C table generator
├── mpconfigport.h           MicroPython configuration
├── board.h                  board hooks
├── boards/
│   ├── GD32F407/            mpconfigboard.mk/.h, pins.csv
│   └── GD32F470/
├── gd32lib/                 vendored GD32F4xx firmware library
│   ├── CMSIS_include/       core_cm4.h, gd32f4xx.h, libopt
│   ├── peripheral_include/  29 peripheral headers
│   ├── peripheral_source/   29 peripheral sources
│   ├── GCC/                 startup_gd32f407_427.S, startup_gd32f450_470.S
│   └── system_gd32f4xx.c    clock/system init
└── ld/                      linker scripts (xE/xG/xI variants)
```

## Current feature status

| Feature            | Status |
|--------------------|--------|
| Boot + REPL (UART) | ✅ working |
| `sys`, `gc`, `struct`, `array`, full compiler | ✅ |
| `machine.Pin` (get/set, on/off/toggle, named + numeric) | ✅ working |
| `machine.freq()`, `machine.reset()`, `machine.unique_id()` | ✅ |
| Timers / PWM / ADC / I2C / SPI / UART objects | ⬜ |
| USB / SD card / filesystem | ⬜ |
| Frozen modules     | ⬜ |

This is a **bootable, REPL-capable port with working GPIO** intended
as a solid base. `machine.Pin` is implemented against the GD32
standard peripheral library (`gpio_mode_set()`, `gpio_bit_write()`),
so it drives real hardware.

Adding further peripherals follows the same pattern:

1. Add the peripheral source to `GD32_PERIPH` in the Makefile
   (e.g. `tim timer`, `spi`, `i2c`, `adc`).
2. Write `machine_<periph>.c` against the `gd32f4xx_<periph>.h` API.
3. Register it in `modmachine_port.c` via `MICROPY_PY_MACHINE_EXTRA_GLOBALS`.

## Notes and gotchas

1. **Clock setup**: `system_gd32f4xx.c` selects the clock source via a
   `#define` near the top of the file. The vendored copy defaults to
   `__SYSTEM_CLOCK_200M_PLL_25M_HXTAL`. Adjust if your board uses a
   different crystal (8 MHz and 16 MHz IRC variants are also provided).

2. **No filesystem**: `mp_import_stat` and `mp_lexer_new_from_file`
   are stubs returning "does not exist", so `import` of external files
   and `open()` are unavailable. Frozen modules can be enabled later.

3. **STDIO disabled**: `MICROPY_PY_SYS_STDFILES` and `MICROPY_PY_IO`
   are off to avoid pulling in `mp_sys_stdout_obj`; the REPL talks
   directly to the UART via `mp_hal_stdout_tx_strn`.

4. **Heap** defaults to 48 KB (F407) — adjust `MICROPY_HEAP_SIZE` in the
   board's `mpconfigboard.h` if you need more/less.

## License

The MicroPython core is MIT licensed. The vendored GD32F4xx firmware
library is BSD-3-Clause, Copyright (c) GigaDevice Semiconductor Inc.
See `gd32lib/` sources for the original headers.
