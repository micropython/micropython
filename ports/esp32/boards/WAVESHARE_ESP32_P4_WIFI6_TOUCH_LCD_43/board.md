This firmware is for the Waveshare ESP32-P4-WIFI6-Touch-LCD-4.3, with
32 MiB flash, 32 MiB in-package PSRAM and an onboard ESP32-C6-MINI-1
wireless coprocessor. It also applies to the camera-equipped -C model,
but does not include camera support.

Hardware documentation and schematics:
https://docs.waveshare.com/ESP32-P4-WIFI6-Touch-LCD-4.3/Resources-And-Documents

Silicon revisions
-----------------

The default firmware targets ESP32-P4 Rev1.x silicon (including v1.3) and uses
200 MHz PSRAM. The REV3 variant targets Rev3.x silicon and uses 250 MHz PSRAM. These silicon
families are not binary-compatible. Select by the chip revision, not the PCB
revision or screen size. Query it before flashing with ``esptool --chip esp32p4
--port PORT chip-id``; do not overwrite the ESP32-C6 firmware when identifying
the ESP32-P4.

Build from the repository root using ESP-IDF v5.5.2 or a later version supported
by this MicroPython port. With mpbuild::

    mpbuild build WAVESHARE_ESP32_P4_WIFI6_TOUCH_LCD_43 --build-container espressif/idf:v5.5.2
    mpbuild build WAVESHARE_ESP32_P4_WIFI6_TOUCH_LCD_43 REV3 --build-container espressif/idf:v5.5.2

The standard partition layout reserves the first 2 MiB for system data and the
application; MicroPython uses the remaining 30 MiB for its filesystem. PSRAM is
required; boot will fail rather than silently continue without it.

Flashing and console
--------------------

Use the connector labelled USB TO UART for flashing and the UART0 REPL at
115200 baud. UART0 uses GPIO37 (TX) and GPIO38 (RX); do not repurpose these
header pins while using the console. The USB OTG connector is not configured
as a USB CDC console by this board definition.

Flash the combined ``firmware.bin`` at offset ``0x2000`` (not ``0x1000``)::

    esptool --chip esp32p4 --port PORT write-flash 0x2000 ports/esp32/build-WAVESHARE_ESP32_P4_WIFI6_TOUCH_LCD_43/firmware.bin

For REV3, the build directory ends in ``-REV3``. If automatic download entry
fails, hold BOOT, press and release RESET, then release BOOT. Use the generic
ESP32 deployment instructions below for erasing flash; erasing also destroys
any installed Python packages and user files.

Wi-Fi and Bluetooth
-------------------

The onboard C6 provides Wi-Fi and BLE through ESP-Hosted over four-bit SDIO
slot 1: CLK=18, CMD=19, D0=14, D1=15, D2=16, D3=17, reset/enable=54.
GPIO6 is not the C6 reset. Keep these pins free of application use.
ESP-NOW is not supported through this link.

The C6 must run ESP-Hosted slave firmware compatible with the version pinned
in ``ports/esp32/main/idf_component.yml`` (currently 2.7.0). Flashing the P4
does not update the C6. Do not assume a vendor demo's C6 firmware is compatible;
see https://github.com/espressif/esp-hosted-mcu for slave firmware and flashing
instructions. The C6 programming pads are separate from the P4 USB console.

Standard MicroPython APIs apply once the C6 firmware is compatible::

    import network
    wlan = network.WLAN(network.WLAN.IF_STA)
    wlan.active(True)
    wlan.connect("ssid", "password")

I2C and microSD
---------------

``machine.I2C(0)`` defaults to SDA=GPIO7, SCL=GPIO8. This bus is shared by
onboard peripherals and the expansion header. No peripheral driver is frozen
or run at boot.

The microSD socket uses SDMMC slot 0: CLK=43, CMD=44, D0=39, D1=40, D2=41,
D3=42. Its supply is on-chip LDO4, acquired by ``machine.SDCard``; slot 1 is
reserved for the C6. Mount an existing FAT-formatted card explicitly::

    import machine
    import vfs
    sd = machine.SDCard(slot=0, width=4)
    vfs.mount(sd, "/sd")

Unmount with ``vfs.umount("/sd")`` before ``sd.deinit()``. The firmware does
not format or automatically mount removable cards.

Optional peripheral packages
----------------------------

Display, touch and audio drivers belong in separate repositories with
``package.json`` manifests for installation using ``mip`` or ``mpremote mip``.
This board definition does not supply those packages or freeze vendor drivers.
After compatible Wi-Fi firmware is available, an external package can be
installed with ``mip.install``. Alternatively, ``mpremote mip install`` uses
the host's network connection and the serial console, without working C6 Wi-Fi.
Use the URL of an actual driver package; no board-specific package is bundled
or published by this definition.

Hardware boundaries for external drivers:

- The LCD is a 480 x 800 ST7701 panel on two-lane MIPI-DSI, with reset GPIO27
  and inverted PWM backlight GPIO26. Its DPHY requires LDO3 at 2.5 V.
  **The current MicroPython ESP32 port has no Python MIPI-DSI API. A working
  display cannot be added with mip alone.** It requires a native transport
  binding in a different firmware build; this definition does not add one.
- Touch uses a GT911 on the shared I2C bus, with addresses 0x5D or 0x14.
  The vendor's corrected BSP uses polling without assigning reset/interrupt
  GPIOs; older bundled examples disagree. Do not drive those signals without
  verifying the board revision and wiring.
- The ES8311 audio codec and ES7210 microphone ADC require external control
  drivers and application I2S configuration. ``machine.I2S`` is enabled, but
  no audio initialization is performed.
- No camera configuration or driver is included.

The vendor's hardware profiles and corrected peripheral configuration are
recorded at https://github.com/waveshareteam/ESP32-P4-WIFI6-Touch-LCD-4.3/blob/main/docs/HARDWARE.md.
