.. _samd_wlan_pinout:

Managing external WLAN/BLE adapters
===================================

Connection scheme for the SAMD WLAN adapters
--------------------------------------------

Mapping between the NINA/esp_hosted signals and board pins by name and number.

======== ========== ======== ========= ========= ============== =========  ==========
NINAW10  esp_hosted Airlift  ItsyBitsy Feather   METRO M4       Sparkfun   Grand Ctrl
Name     Name       Name     M4        M4        Airlift        SAMD51     M4 Express
======== ========== ======== ========= ========= ============== =========  ==========
MOSI/RTS MOSI       MOSI     MOSI (0)  MOSI (55) MOSI (12)      MOSI (44)  D11 (55)
MISO     MISO       MISO     MISO (55) MISO (54) MISO (14)      MISO (43)  D12 (32)
SCK      SCK        SCK      SCK (1)   SCK (17)  SCK (13)       SCK (45)   D13 (33)
CS       CS         CS       D13 (22)  D13 (23)  ESP_CS (15)    D13 (17)   D10 (54)
ACK/CTS  HANDSHAKE  Busy     D11 (21)  D11 (21)  ESP_BUSY (36)  D11 (16)   D7 (117)
RESET    RESET      Reset    D12 (23)  D12 (22)  ESP_RESET (37) D12 (19)   D5 (85)
GPIO0    DATAREADY  GP0      D10 (20)  D10 (20)  ESP_GPIO0 (33) D10 (18)   D6 (116)
SPI #                        1         1         1              4          5
TX       TX         TX       D0  (16)  D0 (49)   D0 (7)         D0 (13)    D1 (56)
RX       TX         RX       D1 ( 17)  D1 (48)   D1 (4)         D1 (12)    D0 (57)
RTS      MOSI/RTS   RTS      MOSI (0)  MOSI (55) MOSI (12)      MOSI (44)  MOSI (55)
CTS      CTS        CTS      D11 (21)  D11 (21)  ESP_BUSY (36)  D11 (16)   D7 (117)
UART #                       3         5         0              2          ?
======== ========== ======== ========= ========= ============== =========  ==========

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
NINAW10   esp_hosted Airlift ItsyBitsy  Feather   METRO M4   Airlift
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

CTS seems not to be used for the NINA firmware, but specified.

Signals required for NINA/esp_hosted firmware upload:

- RX/TX (UART), GPIO0, Reset.

If the hardware supports it, pull RESET low (!). That keeps the
ESP32 with the NINA/esp_hosted firmware silent and at low power
when not being used.


.. _samd_wlan_firmware_upload:

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

Depending on the firmware for the SAMD boards you need the file
NINA_FW_v1.5.0_Airlift.bin or esp_hosted_airlift.bin.

For firmware upload, the following connections to the WiFi module are required:

- Pin Reset (as above)
- Pin GPIO0
- UART RX
- UART TX

The GPIO pins and UART device id varies between boards. See the tables above.
At the Adafruit Metro M4 board for instance, the UART is UART(5), and the Pin names
for reset and GPIO0 are ESP_RESET and ESP_GPIO0.
The firmware can be uploaded, using the espflash.py module, a short script
using espflash.py and mpremote. espflash.py is available at
https://github.com/micropython/micropython-lib/tree/master/micropython/espflash.
This place also holds the example script.::

    import espflash
    from machine import Pin
    from machine import UART
    import sys
    sys.path.append("/")

    reset = Pin(37, Pin.OUT)
    gpio0 = Pin(33, Pin.OUT)
    uart = UART(0, 115200, tx=4, rx=7, timeout=350)

    md5sum = b"b0b9ab23da820a469e597c41364acb3a"
    path = "/remote/NINA_FW_v1.5.0_Airlift.bin"
    # md5sum = b"28ab84372ff4f07551b984671f7f9ff9"
    # path = "/remote/esp_hosted_airlift.bin"

    esp = espflash.ESPFlash(reset, gpio0, uart)
    # Enter bootloader download mode, at 115200
    esp.bootloader()
    # Can now change to higher/lower baudrate
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

The script shows the set-up for the Metro M4 board.
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
