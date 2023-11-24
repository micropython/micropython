# PCTEL WSC-1450

The PCTEL Wireless Sensor Core (WSC) is a versatile Industrial IoT product line
that offers multiple radio connectivity options including cellular, LoRa,
Bluetooth 5, NFC as well as 802.15.4 support.

In addition to several radios, the PCTEL WSC includes several sensors to
monitor a variety of physical conditions. These sensors can detect gas, air
quality, temperature, relative humidity, acceleration, angular rate of change,
magnetic field, range, and sound. For solution optimization, the PCTEL WSC can
be ordered with a subset of radios and sensors.

For more details about this board, and its variants, navigate to
https://pctel.com

# Build instructions

Configure your build environment according to the Adafruit instructions. To
build circuitpython for WSC-1450 do:

    cd ports/nrf make BOARD=pctel_wsc_1450

This will create a number of firmware files in the `build-pctel_wsc_1450`
directory.


# Installing

The WSC-1450 do not feature an Adafruit-provided bootloader (supporting
niceties like UF2 flashing). Instead, WSC-1450 uses DAPLink. DAPLink handles
everything from debugging to programming the device, as well as the boot
sequence.

1. Connect the WSC-1450 dev kit using a USB cable to the `DHD USB` port. This
   will power up the board and open a file browser showing the contents of the
target. (DAPlink magic)
2. Copy the newly built firmware to the WSC-1450 target using drag-n-drop or
   other method. The file to upload is
`circuitpython/ports/nrf/build-pctel_wsc_1450/firmware.combined.hex`
3. Wait for the file to upload.
4. Install is complete. Reset the board.

# Running

Connect an additional USB cable to the `Target USB` port on the development
board and open a terminal like `screen` on Mac or `TeraTerm` on Windows. Serial
settings are 115200,n,8,1.

Don't forget to install/update the supporting python libraries.

Happy hacking.
