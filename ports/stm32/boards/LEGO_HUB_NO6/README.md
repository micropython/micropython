LEGO Hub No.6
=============

This board definition is for the LEGO Hub No. 6, a LEGO control unit with a 5x5
LED display, 6 Powered Up ports, speaker, 6-DOF sensor, Bluetooth, external SPI
flash storage, and a rechargeable battery.  The Hub can work without the battery if
it is plugged in to, and powered by, its USB port.  But without the battery the LEDs
and power on the Powered Up ports will not function.

Features that are currently supported:
- standard MicroPython
- machine and bluetooth modules
- filesystem
- USB VCP, MSC and HID

The Hub has a bootloader preinstalled at 0x08000000 (which is 32kiB in size) which
cannot be erased.  This bootloader is entered by holding down the Bluetooth button,
plugging in USB to power it up, then releasing the Bluetooth button after 5 seconds,
at which point the USB DFU device appears.  If the battery is installed then the
Bluetooth button's RGB LED will cycle colours.  When this bootloader is active, the
flash from 0x08008000 and up can be erased and programmed via USB DFU.

The built-in bootloader has some drawbacks: it cannot be entered programmatically,
and it does not keep the Hub powered up when running from battery (which requires
keeping BAT_PWR_EN high).  As such, this board is configured to work with mboot as
a secondary bootloader: mboot is placed at 0x08008000 and the main application
firmware at 0x08010000.  When mboot is installed it can be entered programatically
via machine.bootloader(), or by holding down the left arrow button when powering
on the Hub and waiting until the display says "B" before releasing the button.

Backing up original Hub firmware
--------------------------------

Before install MicroPython it is advised to backup the original LEGO firmware that
the Hub comes installed with.  To do this, enter the built-in bootloader by holding
down the Bluetooth button for 5 seconds while powering up the Hub via USB.  Then
run the following command from the root of this repository:

    $ cd ports/stm32
    $ make BOARD=LEGO_HUB_NO6 backup-hub-firmware

This will create a file called `lego_hub_firmware.dfu`.  Put this file in a safe
location.  To restore it, enter the built-in bootloader again and run:

    $ make BOARD=LEGO_HUB_NO6 restore-hub-firmware

This will restore the original firmware but not the filesystem.  To recreate the
original filesystem the Hub must be updated using the appropriate LEGO PC
application.

Installing MicroPython
----------------------

You first need to build and install mboot, which only needs to be done once.  From
the root of this repository run:

    $ cd ports/stm32/mboot
    $ make BOARD=LEGO_HUB_NO6

Now enter the built-in bootloader by holding down the Bluetooth button for 5
seconds while powering up the Hub via USB.  Then run:

    $ make BOARD=LEGO_HUB_NO6 deploy

mboot should now be installed.  Enter mboot by holding down the left arrow
button when powering up the Hub.  The display will cycle the letters: N, S, F, B.
When it gets to "B" release the left arrow and it will start mboot.  The Hub then
blinks the centre button red once per second, and appears as a USB DFU device.

Now build MicroPython (start at the root of this repository):

    $ cd mpy-cross
    $ make
    $ cd ../ports/stm32
    $ make submodules
    $ make BOARD=LEGO_HUB_NO6

And deploy to the Hub (making sure mboot is active, the centre button is blinking
red):

    $ make BOARD=LEGO_HUB_NO6 deploy

If successful, the Hub should now appear as a USB serial and mass storage device.

Using MicroPython on the Hub
----------------------------

Access the MicroPython REPL using mpremote (pip install mpremote), or with any
serial terminal program.

To scan for BLE devices:

    >>> import bluetooth
    >>> ble = bluetooth.BLE()
    >>> ble.irq(lambda *x: print(*x))
    >>> ble.active(1)
    >>> ble.gap_scan(2000, 625, 625)

Use help("modules") to see available built-in modules.
