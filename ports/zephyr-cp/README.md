# Zephyr

This is an initial port of CircuitPython onto Zephyr. We intend on migrating all
existing boards onto Zephyr. To start, we'll only support new boards. Existing
boards will be switched as the Zephyr port reaches parity with the existing
implementation.

## Getting Started

First, install Zephyr tools (see [Zephyr's Getting Started Guide](https://docs.zephyrproject.org/4.0.0/develop/getting_started/index.html)). (These are `fish` commands because that's what Scott uses.)


```sh
pip install west
west init -l zephyr-config
west update
west zephyr-export
pip install -r zephyr/scripts/requirements.txt
west sdk install
```

Now to build from `ports/zephyr-cp`:

```sh
make BOARD=nordic_nrf5340dk
```

This uses Zephyr's cmake to generate Makefiles that then delegate to
`tools/cpbuild/build_circuitpython.py` to build the CircuitPython bits in parallel.

`DEBUG=1` adds the debug Kconfig settings in `debug.conf`. `CONSOLE=1` (implied
by `DEBUG=1`) applies a board's `console.overlay`, if it has one, together with
the port's `console.conf`, which turns the Zephyr UART console back on. Boards
whose console UART pins are also user pins, such as the Feather nRF52840, have
the Zephyr UART console only in those builds.

## Native simulator build container

Building the native sim requires `libsdl2-dev:i386` and other 32bit dependencies that
can cause conflicts on 64bit systems resulting in the removal of 64bit versions of critical
software such as the display manager and network manager. A Containerfile and a few scripts
are provided to set up a container to make the native sim build inside without affecting the
host system.

The container automatically mounts this instance of the circuitpython repo inside at
`/home/dev/circuitpython`. Changes made in the repo inside the container and on the host PC
will sync automatically between host and container.

To use the container file:

1. Build the container with `podman build -t zephyr-cp-dev -f native_sim_build_Containerfile .`
2. Run/Start the container by running `./native_sim_build_run_container.sh` on the host PC.
   The script will automatically run or start based on whether the container has been run before.
3. Init requirements inside the container with `./native_sim_build_init_container.sh`

`ZCP_CONTAINER` and `ZCP_IMAGE` override the container and image names, and `ZCP_PODMAN_ARGS`
adds options to `podman run` when the container is first created.

If `west sdk install` fails with `Cannot change mode to ...: Operation not permitted`, the
container runtime is rejecting the `fchmodat2` syscall that Ubuntu 24.04's tar uses. This
happens with older runtimes such as crun 0.17 from Ubuntu 22.04. Remove the container with
`podman rm -f zcp` and create it again with `ZCP_PODMAN_ARGS="--runtime runc"` if runc is
installed, or with `ZCP_PODMAN_ARGS="--security-opt seccomp=unconfined"` otherwise.

To delete the container and cleanup associated files:
```sh
podman ps -a --filter ancestor=zephyr-cp-dev -q | xargs -r podman rm -f
podman rmi zephyr-cp-dev
podman image prune -f
podman rm -f zcp
```

## Running the native simulator

From `ports/zephyr-cp`, run:

```sh
make run-sim
```

`run-sim` starts the native simulator in realtime.
It prints the PTY path to connect to the simulator REPL.
If a local `./CIRCUITPY/` folder exists, its files are used as the simulator's CIRCUITPY drive.

Edit files in `./CIRCUITPY` (for example `code.py`) and rerun `make run-sim` to test changes.

## Shields

Board defaults can be set in `boards/<vendor>/<board>/circuitpython.toml`:

```toml
SHIELDS = ["shield1", "shield2"]
```

For example, `boards/renesas/ek_ra8d1/circuitpython.toml` enables:

```toml
SHIELDS = ["rtkmipilcdb00000be"]
```

You can override shield selection from the command line:

```sh
# Single shield
make BOARD=renesas_ek_ra8d1 SHIELD=rtkmipilcdb00000be

# Multiple shields (comma, semicolon, or space separated)
make BOARD=my_vendor_my_board SHIELDS="shield1,shield2"
```

Behavior and precedence:

- If `SHIELD` or `SHIELDS` is explicitly provided, it overrides board defaults.
- If neither is provided, defaults from `circuitpython.toml` are used.
- Use `SHIELD=` (empty) to disable a board default shield for one build.

## Pin names

Human readable pin names (the `board` module) come from the devicetree by
default: `gpio-leds` and `gpio-keys` labels, node aliases, and connector
`gpio-map`s. Boards can add names without any devicetree involvement by
listing them in `boards/<vendor>/<board>/circuitpython.toml` under `[pins]`.
Each entry maps a board module name to the pin number exposed by the board's
hardware: the SoC package pin (or, for ball grid array packages, the
datasheet's ball id, e.g. `"B2"`), or the castellated module pin when the
board uses a module like the Raytac MDBT50Q:

```toml
[pins]
LED = 17      # QFN package pin number
SDA = "B2"   # ball id for BGA/CSP packages
D13 = 8      # MDBT50Q-1MV2 module pin number
```

The build resolves each package pin to a SoC pad using the iobroker package
pin map selected by `CONFIG_IOBROKER_PACKAGE_<PACKAGE>`
(`modules/iobroker/packages/<package>.toml`) and exposes the name on the
matching pad in the `board` module. A name that already maps to the same pin
(from the devicetree or an earlier entry) is deduplicated; a name that maps
to two different pins is a build error. The package map must not be `CUSTOM`
or missing, and the pad must be on an enabled GPIO controller; otherwise the
build fails with an error naming the offending entry.

## Connector names

Devicetree connector nodes (`gpio-map`) get their names from a generic
per-compatible list in `cptools/zephyr2cp.py`. A board whose silkscreen
differs can override them per position in `circuitpython.toml` under
`[connectors.<node label>]`, keying the gpio-map position (the header pin
number, as a string) to a name:

```toml
[connectors.nordic_expansion_header]
0 = "EXP_00"   # header GPIO 00
21 = "EXP_21"  # header GPIO 21 (QSPI CS)
```

Positions left out of the table get no name.

## OTA firmware updates over BLE

Boards with Bluetooth and a `slot1_partition` can update CircuitPython
itself over the air. A board opts in by shipping a `sysbuild.conf` in its
board folder:

```
boards/<vendor>/<board>/sysbuild.conf:

SB_CONFIG_BOOTLOADER_MCUBOOT=y
```

That makes sysbuild build the MCUboot bootloader into the board's
`boot_partition` and link the application into `slot0_partition` (it must
link into slot0 — see `nrf54lm20dk` for an example of a board that does
not). The app image is wrapped with an MCUboot header using imgtool —
signed, or hash-only when the board sets `SB_CONFIG_BOOT_SIGNATURE_TYPE_NONE`
(like `nordic_nrf54l15dk` does). The port Kconfig then enables Zephyr's
MCUmgr/SMP server over the Bluetooth SMP service:

- `img upload` writes the uploaded image into slot 1
- `img test`/`img confirm` plus `os reset` reboot into the new image
- the app confirms the running image once it has booted, so an update that
  fails to start is automatically reverted (the `TEST_AND_CONFIRM` flow in
  the reference clients)

The service requires an encrypted connection. CircuitPython has no
display or keyboard IO capability, so authenticated (MITM) pairing can
never complete; the OTA boards pair with Just Works instead
(`CONFIG_BT_SMP_ENFORCE_MITM=n` and the SMP service's GATT permissions set
to encryption-only, in the OTA boards' `board.conf`). Once paired and
bonded, the client can upload.

To update a device, flash it with `make BOARD=<board> flash` (which flashes
both MCUboot and the app) and then use an SMP client such as Nordic's open
source [nRF Connect Device Manager](https://github.com/nordicsemi/Android-nRF-Connect-Device-Manager)
apps (the sample app is what's published on the app stores) or the `mcumgr`
CLI, uploading the signed app image (`zephyr.signed.bin` from the build
directory).

### Resetting into the bootloader

Code (and update tools) can enter the bootloader's update mode
programmatically, the way a double-tap of the reset button does:
`supervisor/port.c`'s `reset_to_bootloader()` (invoked by the 1200-bps CDC
touch) and `microcontroller.OnNextReset(RunMode.UF2 / RunMode.BOOTLOADER)`
set a request before resetting, via the adaboot fork's
`adaboot/update_mode.h` helpers.

The mechanism depends on which bootloader the board boots: fork adaboot
(MCUboot) boards read the boot-mode retention flag, so the app needs
`RETENTION_BOOT_MODE` (defaulted in the port Kconfig for boards whose layout
provides a `zephyr,boot-mode` region) and the board's bootloader conf
(`bootloader/mcuboot/conf/<vendor>/<board>.conf`) needs the matching entrance
option (`MCUBOOT_UF2_ENTRANCE_BOOT_MODE`, or `BOOT_SERIAL_BOOT_MODE` on USB-less
boards like `nrf54l15dk`). Boards that boot the stock Adafruit nRF52
bootloader (the Adafruit boards) read the request from the raw GPREGRET
register instead and need no bootloader-side changes.

`nordic_nrf54l15dk` is the first MCUboot board with this enabled.

RP2 boards have no MCUboot. The request is a flag in a few bytes of RAM that
survive the reset. Early in the next boot Zephyr sees the flag and jumps to
the chip's built-in ROM bootloader, which shows up as the RPI-RP2 (RP2040) or
RP2350 UF2 drive. adaboot sets up that RAM region for the RP2 boards.

## Testing other boards

[Any Zephyr board](https://docs.zephyrproject.org/latest/boards/index.html#) can
be used with CircuitPython. To test a different board, use `west` directly to
build the board. The build will do its best to support as much as possible. By
default the Zephyr console will be used for output. USB support is limited by
initialization support in `supervisor/usb.c`. Only flash regions not used by
Zephyr are used for CIRCUITPY. A manual `circuitpython` partition can be
specified instead.

For example, to test the `nrf52840dk` board:

```sh
west build -b nrf52840dk/nrf52840
```

This is already supported in `ports/nordic` as `pca10056`.

The manifest in `zephyr-config/west.yml` skips the HALs of vendors that no board
here uses, so a board from one of them needs its entry removed from the
`name-blocklist` and a `west update` to fetch it. For the
`esp32s3_devkitc` board, that is `hal_espressif`:

```sh
west update
west build -b esp32s3_devkitc/esp32s3/procpu
```
