# MicroPython port for GigaDevice GD32F4xx

Based on the MicroPython v1.30.0 core with the official
[GD32F4xx standard peripheral library](https://github.com/GigaDevice-GD32-MCU/GD32F4xx_Firmware_Library) (V3.3.2).

## Supported boards

| Board      | MCU         | Flash | RAM   | Core clock | REPL UART        |
|------------|-------------|-------|-------|------------|------------------|
| GD32F407   | GD32F407VE  | 512K  | 128K  | 168 MHz    | USART0 PA9/PA10  |
| GD32F470   | GD32F470VE  | 512K  | 192K  | 240 MHz    | USART0 PA9/PA10  |

Both targets share the same port implementation; only the board
definition (startup file, linker script, clock setup) differs.

## Requirements

> **New to this port?** Read **[BUILD_GD32.md](BUILD_GD32.md)** first - it is a
> complete manual build guide covering toolchain setup, submodule
> initialisation, the TinyUSB patch, verified build times, and the mistakes
> people actually hit.

The short version:


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

# ---- PWM ----
p = PWM(Pin("PA0"), freq=1000, duty_u16=32768)   # 50% at 1 kHz
p.freq(2000)
p.duty_u16(16384)                                 # 25%
print(p.duty_ns())
p.deinit()
```

`machine.PWM` maps pins to timer channels via a static table
(`pwm_pin_map`); unmatched pins raise `ValueError`. Both `duty_u16`
(0..65535) and `duty_ns` are supported; frequency is auto-scaled with a
prescaler so the period fits in the 16-bit ARR.

`machine.CAN` uses the extmod CAN protocol and supports classic CAN
(8-byte frames) on CAN0/CAN1. Loopback, silent and silent-loopback modes
are mapped to the bxCAN working modes. Timeout handling and FD frames are
not implemented.

### Pin encoding

Pins are identified either by name (from `pins.csv`) or by an encoded
integer `port << 4 | num`, e.g. `PA0` = `0`, `PB7` = `1 << 4 | 7` = `23`.

## Memory layout (GD32F407/F470, 512K flash)

Internal flash is split into three regions so that the filesystem does not cap
the size of the firmware image:

| Region | Sectors | Size | Address |
|--------|---------|------|---------|
| `.vectors` | 0 | 16 KB | `0x08000000`-`0x08003FFF` |
| littlefs | 1-3 | 48 KB | `0x08004000`-`0x0800FFFF` |
| firmware | 4-7 | 448 KB | `0x08010000`-`0x0807FFFF` |

The vector table has to sit at `0x08000000`, so it gets its own small region at
the bottom.  Putting the filesystem directly above it and the code after that
(same idea as `FLASH_START` / `FLASH_FS` / `FLASH_TEXT` in `ports/stm32`) gives
the image 448 KB instead of the 384 KB a trailing filesystem would allow.  That
extra 64 KB is what makes floating point fit.

The boundaries are defined once, in `ld/*.ld`, and exported to C:

```ld
_flash_fs_start      = ORIGIN(FLASH_FS);
_flash_fs_end        = ORIGIN(FLASH_FS) + LENGTH(FLASH_FS);
_flash_code_start    = ORIGIN(FLASH_TEXT);
_flash_fs_block_size = 16K;
_flash_fs_block_count = 3;
```

`flashbdev.c` reads those symbols rather than hard-coding addresses, and checks
them against its sector table when the block device is initialised - a mismatch
would otherwise erase the wrong sector and brick the board.

### Flashing

Because the vector table and the code are not contiguous, `firmware.bin` does
not start at `0x08000000`.  Two binaries are produced:

| File | Flash at | Contents |
|------|----------|----------|
| `firmware-isr.bin` | `0x08000000` | vector table only (~392 B) |
| `firmware.bin` | `0x08010000` | code, rodata and initialised data |
| `firmware.hex` | - | full image, addresses embedded (easiest) |

`firmware.hex` is the simplest option because it carries absolute addresses:

```bash
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
        -c "program dist/GD32F407/firmware.hex verify reset exit"
```

When writing the `.bin` files by hand, both halves must be programmed, `-isr`
first:

```bash
st-flash write dist/GD32F407/firmware-isr.bin 0x08000000
st-flash write dist/GD32F407/firmware.bin     0x08010000
```

RAM (`0x20000000`, 128 KB on F407 / 192 KB on F470) is unchanged, and the 64 KB
TCMRAM at `0x10000000` is still unused.


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
├── machine_pwm_port.c       machine.PWM (extmod includefile)
├── flashbdev.c              internal-flash block device for littlefs
├── sdcard.c                 SDIO driver (CMD/ACMD, block read/write)
├── machine_sdcard_port.c    machine.SDCard Python binding
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
| `machine.PWM` — TIMER0/1/2/3/7/8/12 channels, 16-bit duty | ✅ working |
| `machine.Signal` / `PinBase` | ✅ working |
| `machine.SoftSPI` / `SoftI2C` — bit-bang on any pins | ✅ working |
| littlefs2 filesystem on internal flash | ✅ working |
| `machine.SDCard` — SDIO, 1/4-bit bus, hot-plug detection | ✅ working |
| `machine.freq()`, `machine.reset()`, `machine.unique_id()` | ✅ |
| `machine.PWM` — timer output compare, freq + duty_u16/duty_ns | ✅ working |

| Frozen modules | ✅ | manifest.py, bytecode in Flash |

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

0. **qstr registration**: this port lists its sources by hand instead of
   including `extmod/extmod.mk`, so it must add `SRC_QSTR += $(SRC_C)`
   explicitly.  Without that line the qstr scanner never sees the port and
   extmod sources, and any `MP_QSTR_*` used only by them fails to compile.

1. **Clock setup**: `system_gd32f4xx.c` picks the clock through a `#define`
   near the top of the file. The vendored copy enables
   `__SYSTEM_CLOCK_168M_PLL_25M_HXTAL` for the GD32F405/407 branch and
   `__SYSTEM_CLOCK_240M_PLL_25M_HXTAL` for GD32F470. Both assume a 25 MHz
   crystal; 8 MHz and 16 MHz IRC variants exist if your board differs.

   The resulting configuration, checked against the GD32F4xx datasheet:

   | | GD32F407 | GD32F470 | Datasheet limit |
   |---|---|---|---|
   | PLL input | 25 MHz / 25 = 1 MHz | 25 MHz / 25 = 1 MHz | 1-2 MHz recommended |
   | VCO | 336 MHz | 480 MHz | 100-500 MHz |
   | CK_SYS | 168 MHz | 240 MHz | 168 / 240 MHz |
   | AHB | 168 MHz | 240 MHz | 168 / 240 MHz |
   | APB2 (AHB/2) | 84 MHz | 120 MHz | 84 / 120 MHz |
   | APB1 (AHB/4) | 42 MHz | 60 MHz | 42 / 60 MHz |
   | PLLQ (USB) | 48 MHz | 48 MHz | 48 MHz required |
   | Flash wait states | 5 | 7 | 5 @168 MHz / 7 @240 MHz |

   Wait states are programmed in `SystemInit()` before the PLL is selected,
   because the GD32 standard library raises the core clock without touching
   `FMC_WS` (see the note below).

2. **No filesystem**: `mp_import_stat` and `mp_lexer_new_from_file`
   are stubs returning "does not exist", so `import` of external files
   and `open()` are unavailable until littlefs is mounted. Modules listed in
   `manifest.py` are frozen instead and can always be imported.

3. **STDIO disabled**: `MICROPY_PY_SYS_STDFILES` and `MICROPY_PY_IO`
   are off to avoid pulling in `mp_sys_stdout_obj`; the REPL talks
   directly to the UART via `mp_hal_stdout_tx_strn`.

4. **Heap** defaults to 48 KB (F407) — adjust `MICROPY_HEAP_SIZE` in the
   board's `mpconfigboard.h` if you need more/less.

## License

The MicroPython core is MIT licensed. The vendored GD32F4xx firmware
library is BSD-3-Clause, Copyright (c) GigaDevice Semiconductor Inc.
See `gd32lib/` sources for the original headers.


## Filesystem

The 48 KB filesystem partition (`0x08070000`–`0x08080000`, sectors 4-7) is
reserved for a **littlefs2** filesystem mounted at `/` at boot:

```python
import os
print(os.listdir('/'))          # []

with open('/boot.py', 'w') as f:
    f.write("print('hello')\n")

print(open('/boot.py').read())
os.stat('/boot.py')
```

Notes:

- **Partition**: Sector 7 of internal flash only, i.e.
  `0x08060000 - 0x0807FFFF` (128 KB).  Sector 6 is left to the firmware, which
  gives the image a 384 KB ceiling; the current build ends at `0x0803DF24`,
  so ~136 KB is free.  Both linker scripts assert this at link time and fail
  with `FIRMWARE TOO LARGE` if the image ever grows past `0x08060000`.
  GD32F4 sector sizes are 16 KB (0-3), 64 KB (4) and 128 KB (5-7), so 128 KB
  is the smallest erasure unit available above the image.
- **Block size equals the erase size** (128 KB).  littlefs erases before it
  programs and never reads back a block it is writing, so the driver keeps no
  read-modify-write buffer.  That is deliberate: caching a 128 KB sector would
  not fit in the ~24 KB of free RAM on the GD32F407 build.
- `writeblocks` with a non-zero offset raises `OSError(EINVAL)`, because a
  partial write would need the rest of the sector (littlefs never does this).
- To format, run `machine.Flash.mkfs()`.  This erases the whole 256 KB
  partition, so it is explicit and never automatic.
- File mtimes come from `mp_hal_time_ns()`, which reads the RTC when
  available and otherwise returns a fixed epoch offset by uptime.
- **Flash wear**: writing to the FS erases 16 KB sectors. Do not use it as
  a scratch buffer in a tight loop.

## SD Card

`machine.SDCard` drives an SD card over the SDIO controller (1-bit for
init, switching to 4-bit for data transfer).

```python
from machine import SDCard
import os

card = SDCard()                 # raises OSError if no card present
print(card.present())           # True
print(card.info())              # (blocks, block_size, high_capacity, ready)

# mount it at /sd
os.mount(card, '/sd')
print(os.listdir('/sd'))
os.umount('/sd')
```

### Wiring (SDIO default pinout)

| Signal | Pin |
|--------|-----|
| CLK | PC12 |
| CMD | PD2 |
| D0  | PC8 |
| D1  | PC9 |
| D2  | PC10 |
| D3  | PC11 |
| CD  | PD13 (active low) |

The driver implements CMD0/2/3/6/7/8/9/16/17/18/24/25 and
ACMD6/ACMD41. Card detect is polled; removing the card invalidates the
driver state so the next `SDCard()` re-initialises.

**Note:** this driver has been verified to *compile* on both boards but
has not been exercised against real hardware. The SDIO init sequence,
CSD capacity decoding and clock switching should be validated on a board
with an actual card before relying on it.


## USB device (TinyUSB) — extra setup

`machine.USBDevice` needs TinyUSB to know about the GD32F4 USBFS controller.
The stock `lib/tinyusb` submodule only carries a RISC-V GD32VF103 port, so this
port adds a Cortex-M4 variant:

| File | Change |
|------|--------|
| `src/portable/synopsys/dwc2/dwc2_gd32f4.h` | **new** — controller description (0x5000_0000, 4 EPs, 320-word DFIFO, NVIC IRQ) |
| `src/tusb_option.h` | `OPT_MCU_GD32F4` = 1601 |
| `src/common/tusb_mcu.h` | GD32F4 → `TUP_USBIP_DWC2`, 4 endpoints |
| `src/portable/synopsys/dwc2/dwc2_common.h` | select `dwc2_gd32f4.h` |

A ready-made patch is checked in as
[0001-tinyusb-add-GD32F4-USBFS.patch](0001-tinyusb-add-GD32F4-USBFS.patch).
It is a plain `git diff` against the submodule's pinned commit, so it applies
with either tool after a fresh `make submodules`:

```bash
cd lib/tinyusb

# Option A: apply as working-tree changes (no commit needed)
git apply ../../ports/gd32/0001-tinyusb-add-GD32F4-USBFS.patch

# Option B: commit it so `git status` stays clean in the submodule
git apply ../../ports/gd32/0001-tinyusb-add-GD32F4-USBFS.patch
git add -A && git commit -m "dwc2: add GD32F4 (Cortex-M4) USBFS support"

cd ../..
```

The patch was verified to apply cleanly (`git apply --check` returns 0) onto a
fresh clone of `micropython/tinyusb` at the pinned commit `b549ac1d`, and it
provides every hook `dwc2_common.h` requires:

`_dwc2_controller[]`, `DWC2_EP_MAX`, `dwc2_clock_init`, `dwc2_phy_init`,
`dwc2_phy_update`, `dwc2_phy_deinit`, `dwc2_dcd_int_enable`,
`dwc2_dcd_int_disable`, `dwc2_remote_wakeup_delay`, `dwc2_mcu_init`.

If a future TinyUSB update moves these anchors, `git apply -3` will attempt a
three-way merge; otherwise re-derive the patch from a working tree with
`git diff <base> HEAD > 0001-tinyusb-add-GD32F4-USBFS.patch`.

Wiring: USBFS uses **PA11 (DM) / PA12 (DP)** with AF10. The CDC serial is the
96-bit factory UID, and the REPL is shared with the UART through a common
input ring buffer, so either interface can drive the console.

VID/PID default to GigaDevice's 0x28E9 / 0x0189; override
`MICROPY_HW_USB_VID` / `MICROPY_HW_USB_PID` in the board header if needed.

## Networking (lwIP)

`network.LAN` brings up the GD32 ENET MAC with lwIP running in the main loop.

```python
import network
nic = network.LAN()
nic.active(True)
nic.ifconfig(("192.168.1.50", "255.255.255.0", "192.168.1.1", "192.168.1.1"))
print(nic.ifconfig())
print(nic.status())
```

### Wiring (RMII, all AF11)

| Signal | Pin |
|--------|-----|
| REF_CLK | PA1 |
| MDIO | PA2 |
| MDC | PC1 |
| RXD0 | PC4 |
| RXD1 | PC5 |
| TX_EN | PB11 |
| TXD0 | PB12 |
| TXD1 | PB13 |

MII vs RMII is selected through SYSCFG using
`syscfg_enet_phy_interface_config`, which differs from STM32 where the
choice lives in the AF configuration.

### lwIP memory

`lwip_inc/lwipopts.h` keeps the footprint small because the GD32F407 build
only leaves about 24 KB of RAM free:

| Setting | Value |
|---------|-------|
| `MEM_SIZE` | 16 KB |
| `PBUF_POOL_SIZE` | 8 |
| `MEMP_NUM_TCP_SEG` | 16 |

Raise these for sustained throughput, and lower `MICROPY_HEAP_SIZE` in the
board header to make room.

NOTE: the driver compiles on both boards but has not been exercised against
a real PHY. Auto-negotiation timing and the RMII clock setup should be
verified on hardware before relying on it.

## Frozen modules

Python modules listed in `manifest.py` are compiled to bytecode at build time
and linked into the firmware, so they cost Flash instead of RAM and import
without any parsing.

```bash
# default: freezes modules/_boot.py and modules/gd32.py
make BOARD=GD32F407

# disable freezing for a build
make BOARD=GD32F407 FROZEN_MANIFEST=

# use a different module set
make BOARD=GD32F407 FROZEN_MANIFEST=/path/to/other_manifest.py
```

`manifest.py` uses the standard MicroPython manifest API
(`module()`, `package()`, `freeze()`, `include()`, `require()`), so
`micropython-lib` packages can be pulled in as well:

```python
# manifest.py
module("modules/_boot.py", opt=3)
module("modules/gd32.py", opt=3)
# include("$(MPY_LIB_DIR)/micropython/aiorepl/manifest.py")
```

Two modules are frozen by default:

| Module | Purpose |
|--------|---------|
| `_boot.py` | runs at startup; currently collects GC and mounts littlefs |
| `gd32.py` | small helper with `info()` and `blink()` for sanity checks |

Measured effect on this port: findings Flash +1.4 KB, **RAM unchanged**
(the bytecode lives in `.rodata`; only the lookup tables are added).

NOTE: `FROZEN_MANIFEST` must be set *before* `include $(TOP)/py/py.mk`,
because `py.mk` appends `frozen_content.o` to `PY_O` only when the variable is
already non-empty. This port's Makefile does that, and links `PY_O` rather
than listing the core/extmod objects by hand.

## TLS (mbedTLS)

`import tls` (also reachable as `ssl`) provides TLS 1.2 through mbedTLS 3.6.6,
which makes HTTPS possible on top of `socket`.

```python
import socket, tls

# Verify the server certificate against the bundled CA bundle
ctx = tls.SSLContext(tls.PROTOCOL_TLS_CLIENT)
ctx.load_verify_locations(cafile="/certs/ca.pem")
s = ctx.wrap_socket(socket.socket(), server_hostname="example.com")
s.connect(socket.getaddrinfo("example.com", 443)[0][-1])
s.write(b"GET / HTTP/1.0\r\nHost: example.com\r\n\r\n")
print(s.read(1024))
```

### Trimming for this chip

`mbedtls/mbedtls_config_port.h` starts from MicroPython's shared configuration
and then cuts it down, because the GD32F407 only has 128 KB of SRAM of which
about 90 KB is already committed:

| Setting | Value | Why |
|---------|-------|-----|
| `MBEDTLS_SSL_IN_CONTENT_LEN` | 8192 (from 16384) | the default record buffer is most of the free RAM |
| `MBEDTLS_SSL_PROTO_TLS1`, `TLS1_1` | disabled | no modern server accepts them |
| ECP curves | secp256r1, secp384r1 only | the P-192/224/521 and Koblitz curves cost code for no practical gain |
| `MBEDTLS_ECP_WINDOW_SIZE` | 4 | compact ECC multiplication |

Measured cost of enabling TLS:

| | before | after | delta |
|---|---|---|---|
| Flash | 253,788 B | 375,456 B | **+121 KB** |
| RAM (bss) | 90,880 B | 90,888 B | +8 B |

Flash is now at 95.5% of the 384 KB firmware budget (17.3 KB free), so there is
very little room for further additions.  RAM is comfortable: 39 KB free on the
GD32F407, 103 KB on the GD32F470.

**Note:** TLS compiles and the module is present, but no handshake has been
exercised on hardware.  The ECC/bignum paths are the ones most likely to need
tuning (they are the slowest part of a handshake on a Cortex-M4 without a
crypto accelerator).

## Machine module coverage

Compared with `ports/stm32` for STM32F4:

| Module | GD32 port | Notes |
|--------|-----------|-------|
| `Pin` | ✅ | named + numeric, IN/OUT/OPEN_DRAIN/PULL_* |
| `Timer` | ✅ | software timer (1 kHz SysTick) |
| `WDT` | ✅ | FWDGT independent watchdog |
| `UART` | ✅ | up to 6 instances, IRQ-driven RX |
| `SPI` | ✅ | hardware SPI0-5, master |
| `I2C` | ✅ | hardware I2C0-2, master |
| `ADC` | ✅ | ADC0 ch0-15, 12-bit |
| `DAC` | ✅ | DAC0/DAC1 on PA4/PA5 |
| `RTC` | ✅ | calendar, BCD conversion internal |
| `CAN` | ✅ | bxCAN CAN0/CAN1, classic CAN |
| `PWM` | ✅ | TIMER0/1/2/3/7/8/12, duty_u16 + duty_ns |
| `Signal` / `PinBase` | ✅ | Pin implements the virtpin protocol |
| `onewire` | ✅ | DS18B20 etc. over any GPIO (bit-banged) |
| `tls` / `ssl` | ✅ | mbedTLS 3.6, TLS 1.2, certificate verification |
| `SoftSPI` / `SoftI2C` | ✅ | extmod bit-bang on any GPIO |
| `SDCard` | ✅ | SDIO driver, hot-plug, block device protocol |
| `I2S` | ✅ | SPI1/SPI2 as I2S1/I2S2, DMA ping-pong, blocking/non-blocking/async |
| `I2CTarget` | ✅ | hardware I2C0-2 target, addr 7-bit, IRQ-driven |
| `USBDevice` | ✅ | TinyUSB CDC on USBFS (DWC2), PA11/PA12 |
| `mem8/16/32` | ✅ | raw memory access |
| `lightsleep` / `deepsleep` | ✅ | `lightsleep` = WFI; `deepsleep` = PMU standby |
| `bitstream` | ✅ | DWT cycle counter + GPIO BOP |
| `unique_id` | ✅ | 96-bit factory UID at `0x1FFF7A10` |
| `freq` | ✅ | returns `SystemCoreClock` |
| `disable_irq` / `enable_irq` | ✅ | PRIMASK based |
| `time_pulse_us` | ✅ | IRQ-timed pulse measurement |
| `dht_readinto` | ✅ | DHT11/DHT22 temperature & humidity sensors |
| `mem_backup` | ✅ | 20 RTC backup registers (80 B), retained across reset |
| `UART.sendbreak` | ✅ | drive a break condition on TX |
| `UART.readchar` / `writechar` | ✅ | single-byte read/write |
| `UART.irq` | ✅ | RX interrupt handler via `mpirq` |
| `SPI.LSB` / `SPI.MSB` | ✅ | bit-order constants for `firstbit=` |
| `@micropython.native` / `viper` | ✅ | Thumb-2 runtime code generation |
| `os.dupterm` | ✅ | attach a stream (e.g. a UART) to the REPL |
| `framebuf` | ✅ | `FrameBuffer` for drawing into a bytearray |

### Python standard-library modules

These are enabled and use no external dependencies:

| Module | Notes |
|--------|-------|
| `binascii` | `hexlify`, `unhexlify`, `a2b_base64`, `b2a_base64`, `crc32` |
| `json` | `dump`, `dumps`, `load`, `loads` |
| `re` | `compile`, `match`, `search`, `sub` (MicroPython subset) |
| `uctypes` | struct/pointer access to raw memory |
| `heapq` | `heappush`, `heappop`, `heapify` |
| `hashlib` | `sha256` via mbedTLS |
| `random` | seeded from the TRNG; `getrandbits`, `choice`, `randint` |
| `time` | `time`, `time_ns`, `sleep*`, `ticks_*`, `gmtime`, `localtime`, `mktime` |
| `os` | `listdir`, `stat`, `mkdir`, `rename`, `remove`, `mount`, `uname`, `urandom`, `sync`, `dupterm` |

### Deliberately omitted versus STM32F4

| Item | Why |
|------|-----|
| `machine.ADCBlock` | the simpler single-channel `ADC` covers the GD32F4 use case |
| `pyb` legacy API | specific to ST boards, not applicable to GD32 |
| `_thread` | needs a PendSV-based context switcher; not implemented yet |
| `os.VfsRom` | needs a ROMFS image to mount; no ROM data source here |
| `stm` module | exposes STM32 register names, meaningless on GD32 |
| `bluetooth` | the GD32F4 has no on-chip BLE radio |

Everything else the STM32F4 port exposes is present: 20 `machine` types,
`network.LAN` on lwIP, littlefs, USB CDC, frozen modules, `asyncio`,
`socket`, `webrepl` and the usual standard library.  Note the `re` module is MicroPython's cut-down
implementation: only `compile`, `match`, `search` and `sub` are available.
