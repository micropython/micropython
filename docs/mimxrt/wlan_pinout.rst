.. _mimxrt_wlan_pinout:

Managing external WLAN/BLE adapters
===================================

Connection scheme for the i.MXRT WLAN adapters
----------------------------------------------

Mapping between the NINA/esp_hosted signals and board pins by name.

======== ========== =======  ========  ======  ======  ======= ========
NINAW10  esp_hosted Airlift   MIMXRT   Teensy  OLIMEX  Seeed   METRO M7
 Name    Name        Name    10xx_EVK   4.x    RT1010  ArchMix Airlift
======== ========== =======  ========  ======  ======  ======= ========
MOSI/RTS MOSI       MOSI       D11      D11      A2    J4-14   MOSI
MISO     MISO       MISO       D12      D12      A1    J4-13   MISO
SCK      SCK        SCK        D13      D13      A4    J4-15   SCK
CS       CS         CS         D10      D5       A3    J4-12   ESP_CS
ACK/CTS  HANDSHAKE  Busy       D7       D9       SCL2  J4-11   ESP_BUSY
RESET    RESET      Reset      D5       D6       SDA2  J4-10   ESP_RESET
GPIO0    DATAREADY  GP0        D6       D10      (*)   J4-09   ESP_GP0
SPI#                           0        0        0       0       0
TX       TX         TX         D0       D0       D8    J4-06   D0
RX       RX         RX         D1       D1       D7    J4-07   D1
RTS      MOSI/RTS   RTS        D11      D11      A2    J4-14   MOSI
CTS      CTS        CTS        D12      D12      SCL2  J4-13   ESP_BUSY
UART#                          1        1        2       3     1
======== ========== =======  ========  ======  ======  ======= ========

(*) GPIO0 of the WiFi module can be shared e.g. with SCK if they are
    decoupled with a resistor from SCK (Host) to the GPIO input module.

Mapping between firmware signal names and ESP32 pins for the NINA firmware
and esp_hosted firmware

========  ========== ========  =======  =======
NINAW10   esp_hosted   NINA    Airlift  Airlift
Name      Name       W102 pin   Name      pin
========  ========== ========  =======  =======
MOSI      MOSI         12      MOSI       14
MISO      MISO         23      MISO       23
SCK       SCK          18      SCK        18
GPIO1/CS  CS            5      CS          5
ACK       HANDSHAKE    33      Busy       33
RESET     RESET        EN      Reset      EN
GPIO0     DATAREADY     0      GP0         0
TX        TX            1      TX          1
RX        TX            3      RX          3
RTS       MOSI/RTS     12      -          14
CTS       CTS          33      -          33
========  ========== ========  =======  =======

Mapping between the NINA/esp_hosted signals and adapter pins by name.

========  ========== ======= =========  ========  ========= ==========
NINAW10   esp_hosted Airlift ItsyBitsy  Feather   METRO M7   Airlift
Name      Name        Name    Add-on    Add-on    Airlift   Uno Shield
========  ========== ======= =========  ========  ========= ==========
MOSI      MOSI       MOSI      MOSI     MOSI      MOSI        MOSI
MISO      MISO       MISO      MISO     MISO      MISO        MISO
SCK       SCK        SCK       SCK      SCK       SCK         SCK
GPIO1/CS  CS         CS        ECS      ESPCS     ESP_CS      CS
ACK       HANDSHAKE  Busy      EBSY     ESPBUSY   ESP_BUSY    BUSY
RESET     RESET      Reset     RST      ESPRESET  ESP_RESET   RST
GPIO0     DATAREADY  GP0       GPIO0    ESPGPIO0  ESP_GP0     G0
TX        TX         TX        ETX      ESPTX     ESP_TX      TX
RX        RX         RX        ERX      ESPRX     ESP_RX      RX
========  ========== ======= =========  ========  ========= ==========

Signals required for WiFi:

- NinaW10:    MISO/MOSI/SCK (SPI), CS, Busy (ACK), Reset
- esp_hosted: MISO/MOSI/SCK (SPI), CS, Handshake, Dataready, Reset

Signals required for Bluetooth:

- NinaW10:    RX/TX (UART), RTS (MOSI), CS, RESET
- esp_hosted: RX/TX (UART), RTS (MOSI), CS, RESET

CTS seems not to be used, but specified.

Signals required for NINA/esp_hosted firmware upload:

- RX/TX (UART), GPIO0, Reset.

If the hardware supports it, pull RESET low (!). That keeps the
ESP32 with the NINA/esp_hosted firmware silent and at low power
when not being used.


.. _mimxrt_wlan_firmware_upload:

Instructions for WLAN/BLE firmware upload ESP32 module
------------------------------------------------------

The NINA firmware in the NINA module has to be updated for use with MicroPython. That can be done
using MicroPython and two small Python scripts.

The firmware binaries are available at
https://github.com/micropython/micropython-lib/tree/master/micropython/espflash
There are the firmware files available for different esp32 modules.

1. Adafruit Airlift modules with NINA firmware: NINA_FW_v1.5.0_Airlift.bin.
2. Adafruit Airlift modules with esp_hosted firmware: esp_hosted_airlift.bin
3. Arduino Nano RP2040 connect: NINA_FW_v1.5.0_W102.bin

For the mimxrt boards with i.MX RT 101x MCU, you need the file NINA_FW_v1.5.0_Airlift.bin, for
all other boards esp_hosted_airlift.bin.

For firmware upload, the following connections to the WiFi module are required:

- Pin Reset (as above)
- Pin GPIO0
- UART RX
- UART TX

The GPIO pins and UART device id varies between boards. See the tables above.
At the Adafruit Metro M7 board, the UART is UART(1), and the Pin names
for reset and GPIO0 are ESP_RESET and ESP_GPIO0.
The firmware can be uploaded, using the espflash.py module, a short script
using espflash.py and mpremote. espflash.py is available at
https://github.com/micropython/micropython-lib/tree/master/micropython/espflash.
This place also holds the example script.::

    import espflash
    from machine import Pin
    from machine import UART
    import sys
    sys.path.append("/flash")

    reset = Pin("ESP_RESET", Pin.OUT)
    gpio0 = Pin("ESP_GPIO0", Pin.OUT)
    uart = UART(0, 115200, timeout=350)

    md5sum = b"b0b9ab23da820a469e597c41364acb3a"
    path = "/remote/NINA_FW_v1.5.0_Airlift.bin"

    esp = espflash.ESPFlash(reset, gpio0, uart)
    # Enter bootloader download mode, at 115200
    esp.bootloader()
    # Can now change to higher/lower baud rate
    esp.set_baudrate(921600)
    # Must call this first before any flash functions.
    esp.flash_attach()
    # Read flash size
    size = esp.flash_read_size()
    # Configure flash parameters.
    esp.flash_config(size)
    # Write firmware image from internal storage.
    esp.flash_write_file(path)
    # Compares file and flash MD5 checksum.
    esp.flash_verify_file(path, md5sum)
    # Resets the ESP32 chip.
    esp.reboot()

The script shows the set-up for the Metro M7 board.
The md5sum is the one of the WiFi firmware. It may change and
can be recalculated using e.g. the Linux *md5sum* command. It is used to
verify the firmware upload. To upload the firmware, place the firmware
and the above script (let's call it ninaflash.py) into the same directory
on your PC, and run the command::

    mpremote connect <port> mount . run ninaflash.py

After a while, the upload will start. A typical start sequence looks like::

    Local directory . is mounted at /remote
    Failed to read response to command 8.
    Failed to read response to command 8.
    Changing baudrate => 921600
    Flash attached
    Flash size 2.0 MBytes
    Flash write size: 1310720 total_blocks: 320 block size: 4096
    Writing sequence number 0/320...
    Writing sequence number 1/320...
    Writing sequence number 2/320...
    Writing sequence number 3/320...
    Writing sequence number 4/320...
    ....
    ....
    Writing sequence number 317/320...
    Writing sequence number 318/320...
    Writing sequence number 319/320...
    Flash write finished
    Flash verify: File  MD5 b'b0b9ab23da820a469e597c41364acb3a'
    Flash verify: Flash MD5 b'b0b9ab23da820a469e597c41364acb3a'
    Firmware verified.

The initial messages *Failed to read response to command 8.*
can be ignored.
