LEGO Hub No.7
=============

This board definition is for the LEGO Hub No. 7, a LEGO control unit with 1 button,
2 RGB LEDs, 2 Powered Up ports, 6-DOF sensor, Bluetooth, USB, 4MiB external SPI
flash storage, and a rechargeable battery.

Features that are currently supported:
- standard MicroPython
- machine and bluetooth modules
- filesystem
- USB VCP, MSC and HID

The Hub has a bootloader preinstalled at 0x08000000 (which is 32kiB in size) which
cannot be erased.  This bootloader is entered by holding down the button for 5 seconds,
at which point the USB DFU device appears.  If the battery is installed then the
RGB LED will flash purple. If the battery is not installed, the LED will flash orange
briefly and then the hub will turn off (so having the battery installed is required).
When this bootloader is active, the flash from 0x08008000 and up can be erased
and programmed via USB DFU.

The built-in bootloader has some drawbacks: it cannot be entered programmatically,
and it does not keep the Hub powered up when running from battery (which requires
keeping BAT_PWR_EN high).  As such, this board is configured to work with mboot as
a secondary bootloader: mboot is placed at 0x08008000 and the main application
firmware at 0x08010000.  When mboot is installed it can be entered programatically
via machine.bootloader().

Backing up original Hub firmware
--------------------------------

Before installing MicroPython it is advised to backup the original LEGO firmware that
the Hub comes installed with.  To do this, enter the built-in bootloader by holding
down the power button for 5 seconds while powering up the Hub via USB (you may
need to take out the battery and disconnect USB to power off the Hub first).  Then
run the following command from the root of this repository:

    $ cd ports/stm32
    $ make BOARD=LEGO_HUB_NO7 backup-hub-firmware

This will create a file called `lego_hub_firmware.dfu`.  Put this file in a safe
location.  To restore it, enter the built-in bootloader again and run:

    $ make BOARD=LEGO_HUB_NO7 restore-hub-firmware

This will restore the original firmware but not the filesystem.  To recreate the
original filesystem the Hub must be updated using the appropriate LEGO PC
application.

Installing MicroPython
----------------------

You first need to build and install mboot, which only needs to be done once.  From
the root of this repository run:

    $ cd ports/stm32/mboot
    $ make BOARD=LEGO_HUB_NO7

Now enter the built-in bootloader by holding down the power button for 5
seconds while powering up the Hub via USB (you may need to take out the battery
and disconnect USB to power off the Hub first).  Then run:

    $ make BOARD=LEGO_HUB_NO7 deploy

mboot should now be installed.  To enter mboot, remove USB and the battery.
Connect the USB cable (the other end of the USB cable must be connected to
something that provides power). The status light should start cycling through
different colors. Replace the battery (the button will not work without the
battery present). Press the button to activate the desired boot mode:

- Status light is red - run application (normal boot).
- Status light is green - run application in factory file system mode.
- Status light is blue - run application in safe mode.
- Status light is white - start DFU on the USB port.


Now build MicroPython (start at the root of this repository):

    $ cd mpy-cross
    $ make
    $ cd ../ports/stm32
    $ make submodules
    $ make BOARD=LEGO_HUB_NO7

And deploy to the Hub (making sure mboot DFU is active, the center button is
blinking red):

    $ make BOARD=LEGO_HUB_NO7 deploy

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

Updating MicroPython from the Hub's filesystem
----------------------------------------------

You can update the MicroPython application firmware using the instructions above
for installing the firmware for the first time.  The Hub also supports updating
the application firmware from within MicroPython itself, using the on-board
filesystem.

To use this feature, build the firmware (see above for details) then gzip it and
copy the resulting file to the Hub (eg using mpremote):

    $ make BOARD=LEGO_HUB_NO7
    $ gzip build-LEGO_HUB_NO7/firmware.dfu
    $ mpremote cp build-LEGO_HUB_NO7/firmware.dfu.gz :

Then get a REPL on the Hub and execute:

    >>> import appupdate
    >>> appupdate.update_app("firmware.dfu.gz")

You can alternatively run this REPL command using mpremote:

    $ mpremote exec --no-follow "import appupdate; appupdate.update_app('firmware.dfu.gz')"

At that point the Hub should restart and the LED on the central button will flash
different colours.  Once the update is complete the LED will stop flashing and the
Hub will appear again as a USB device.  The application firmware is now updated
and you can remove the firmware.dfu.gz file if desired.
