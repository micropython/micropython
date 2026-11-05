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

## Using peripherals

```python
from machine import Pin, UART, SPI, I2C, ADC, Timer, WDT

# ---- UART ----
u = UART(1, baudrate=115200, bits=8, parity=None, stop=1,
         tx=Pin("PA2"), rx=Pin("PA3"))
u.write(b"hello\r\n")
n = u.any()
data = u.read(10)          # blocks up to timeout

# ---- SPI (master, software NSS) ----
spi = SPI(0, baudrate=10_000_000, polarity=0, phase=0,
          sck=Pin("PA5"), mosi=Pin("PA7"), miso=Pin("PA6"))
spi.write(b"\x9f")
val = spi.read(3)

# ---- I2C ----
i2c = I2C(0, freq=400000, scl=Pin("PB6"), sda=Pin("PB7"))
i2c.writeto(0x68, b"\x00")
raw = i2c.readfrom(0x68, 6)

# ---- ADC ----
adc = ADC(Pin("PA0"))      # PA0..PA7 -> ch0..7, PB0/1 -> ch8/9, PC0..5 -> ch10..15
print(adc.read_u16())      # 0..4095 (12-bit)

# ---- Timer (software timer, 1 kHz tick) ----
def tick(t):
    print("tick")

tim = Timer(0, mode=Timer.PERIODIC, callback=tick, period=500)   # every 500 ms
tim.deinit()

# ---- Watchdog ----
wdt = WDT(timeout=8000)    # 8 s
wdt.feed()

# ---- DAC ----
dac = DAC(Pin("PA4"))      # PA4 -> DAC0, PA5 -> DAC1
dac.write(2048)            # mid-scale (12-bit)
print(dac.read())

# ---- RTC ----
rtc = RTC()
rtc.init((2026, 11, 5, 3, 20, 15, 0, 0))   # y, m, d, wday, h, min, s, us
print(rtc.datetime())                       # (2026, 11, 5, 3, 20, 15, 0, 0)

# ---- CAN ----
can = CAN(0, bitrate=500_000)
can.set_filters([(0x123, 0x7FF, CAN.STD)])   # accept one standard id
can.send(bytes([1, 2, 3]), 0x123)
if can.recv(0):                              # blocking
    pass
print(can.state())                           # CAN.STATE_ACTIVE etc.
print(can.get_counters())
```

`machine.CAN` uses the extmod CAN protocol and supports classic CAN
(8-byte frames) on CAN0/CAN1. Loopback, silent and silent-loopback modes
are mapped to the bxCAN working modes. Timeout handling and FD frames are
not implemented.

### Pin encoding

Pins are identified either by name (from `pins.csv`) or by an encoded
integer `port << 4 | num`, e.g. `PA0` = `0`, `PB7` = `1 << 4 | 7` = `23`.

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
├── machine_uart_port.c      machine.UART (extmod includefile)
├── machine_spi_port.c       machine.SPI
├── machine_i2c_port.c       machine.I2C
├── machine_adc_port.c       machine.ADC (extmod includefile)
├── machine_wdt_port.c       machine.WDT (extmod includefile)
├── machine_dac_port.c       machine.DAC
├── machine_rtc_port.c       machine.RTC
├── machine_can_port.c       machine.CAN (extmod includefile)
├── modmachine_port.c        port hooks + peripheral registration
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

| Feature | Status |
|---------|--------|
| Boot + REPL (UART) | ✅ working |
| `sys`, `gc`, `struct`, `array`, full compiler | ✅ |
| `machine.Pin` — get/set, on/off/toggle, named + numeric, IN/OUT/OPEN_DRAIN/PULL_* | ✅ working |
| `machine.Timer` — soft timer, ONE_SHOT / PERIODIC, callback | ✅ working |
| `machine.WDT` — independent watchdog (FWDGT), configurable timeout | ✅ working |
| `machine.UART` — 6 instances, baudrate/bits/parity/stop, read/write/any/txdone | ✅ working |
| `machine.SPI` — hardware SPI0-5, polarity/phase/bits/firstbit, transfer | ✅ working |
| `machine.I2C` — hardware I2C0-2, master transfer via `modmachine` protocol | ✅ working |
| `machine.ADC` — ADC0 channels 0-15, `read_u16()` | ✅ working |
| `machine.DAC` — DAC0/DAC1 on PA4/PA5, `write()`/`read()`, 12-bit | ✅ working |
| `machine.RTC` — calendar RTC, `init()` / `datetime()` | ✅ working |
| `machine.CAN` — bxCAN, 2 instances, classic CAN, filters, irq | ✅ working |
| `machine.freq()`, `machine.reset()`, `machine.unique_id()` | ✅ |
| Hardware `machine.PWM` (timer output compare) | ⬜ next |

| USB device / SD card / filesystem | ⬜ |
| Frozen modules | ⬜ |

The peripheral set is modelled on `ports/stm32` for STM32F4. Modules are
implemented as either extmod `INCLUDEFILE` hooks or standalone files
registered through `MICROPY_PY_MACHINE_EXTRA_GLOBALS`.

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
