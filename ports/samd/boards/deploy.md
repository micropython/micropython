# Deploy firmware to the SAMD boards and a NINA/ESP32 WiFi module.

## Deploy the MicroPython firmware

For deploying the MicroPython firmware to the SAMD module, follow
the usual procedure:

1. Push reset twice or call machine.bootloader(), if there was already
   MicroPyhton installed at the board. A drive icon should appear
   representing a virtual drive.
2. Copy the .uf2 file with the required firmware to that drive. As
   soon as the drive disappears, the firmware is loaded and started.

If a MicroPython firmware is already installed at the board, it is also
possible to enable the bootloader by shortly switching the USB Serial
to 1200 baud and back, like it is done in the short script below. That
allows for scripted update.

```Py
import serial
import time
import sys
com=serial.Serial(sys.argv[1], 1200, dsrdtr=True)
com.dtr=False
com.rts=False
time.sleep(0.2)
com.close()
time.sleep(4)
# then copy the firmware file to the drive
```

## Deploy the Wifi firmware.

The WiFi module uses the NINA firmware, which works on any ESP32 module,
like the u.Blox NINA W102, the Adafruit Airlink modules or a generic
ESP32 device with at least 2 MB flash. Before the first use the appropriate
firmware has to be loaded to the device. A pre-existing firmware might
not work. The procedure differs between device types.

### Firmware source and types.

There exist two firmware versions, which only differ in the connection
for the MOSI connection between host MCU and WiFi module. The connections
for Wifi use are:
```
    Driver    NINA    Airlift  Airlift
    Name    W102 pin   Name      pin
    ----------------------------------
    MOSI      12      MOSI       14
    MISO      23      MISO       23
    SCK       18      SCK        18
    GPIO1      5      CS          5
    ACK       33      Busy       33
    RESET     EN      Reset      EN
```
The firmware binaries are available at https://github.com/micropython/micropython-lib/tree/master/micropython/espflash or https://github.com/robert-hh/Shared-Stuff.
For a generic ESP32 device you can use either version, given that the
MOSI signal is wired properly. If possible, add a **pull-down** resistor to
the RESET pin, value 1.5 to 10k, such that RESET is kept low as long as
WiFi is not used. That keeps the ESP32 in inactive state and it's GPIO pins
at high impedance.

### Firmware upload to a Generic ESP32 device. 

That is the easiest task, if the USB port of the device is available. Just
follow the standard ESP procedure for uploading firmware using esptool.py with
the command:

```
esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 460800 write_flash -z 0 NINA_FW_v1.5.0_W102.bin
```

Note that the load address is 0. The port name will be different depending on
the PC set-up and it's operating system. If the USB port is not available, follow
the procedure for built-in modules.

### Firmware upload for built-in or breakout modules

For firmware upload, the following connections to the WiFi module are required:

- Pin Reset (as above)
- Pin GPIO0
- UART RX
- UART TX

The GPIO pins and UART device id varies between boards. If the WiFi module is hardwired
to a MCU or Add-on board, the Pin and UART properties can be found in the documentation,
otherwise any suitable pins can be used. On some add-on boards like the Adafruit ItsyBitsy,
Feather or Arduino Shield modules solder bridges may have to be closed to RX, TX and GPIO0.
RX and TX are needed for bluetooth as well.
Once the wiring is done, the firmware can be uploaded, using the espflash.py module, a short script
using espflash.py and mpremote. espflash.py is usually embedded into the MicroPython firmware
of the SAMD device as frozen bytecode. Otherwise, it is available at https://github.com/micropython/micropython-lib/tree/master/micropython/espflash. This place also holds the example script.

```
import espflash
from machine import Pin
from machine import UART
import sys
sys.path.append("/")

if True:
    reset = Pin("ESP_RESET", Pin.OUT)
    gpio0 = Pin("ESP_GPIO0", Pin.OUT)
    uart = UART(0, 115200, tx=Pin("ESP_TX"), rx=Pin("ESP_RX"), timeout=350)

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
```

The script shows the set-up for the Metro M4 Express Airlift board. 
The md5sum is the one of the WiFi firmware. It may change and
can be recalculated using e.g. the Linux `md5sum` command. It is used to
verify the firmware upload. To upload the firmware, place the firmware
and the above script (let's call it ninaflash.py) into the same directory
on your PC, and run the command:
```
mprememote connect <port> mount . run ninaflash.py
```
After a while the upload will start. A typical start sequence looks like:
```
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
```
The initial messages `Failed to read response to command 8.`
can be ignored.

Once the firmware upload is finished, the connection to RX, TX and GPIO0
can be removed, unless the modules is used to Bluetooth communication,
which needs RX and TX.
