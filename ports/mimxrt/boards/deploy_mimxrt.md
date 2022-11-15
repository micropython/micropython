Firmware can be loaded to the MIMXRT development boards in various ways. The most convenient
one is using the built-in support MCU. When a PC is connected to the debug USB port, a drive
icon will appear. Firmware can be uploaded to the board by copying it to this drive. The copy
and flashing will take a few moments. At the end of the upload, the drive icon will disappear
and reappear again. Then the reset button has to be pushed, which starts the MicroPython firmware.

Depending on the power jumper settings, both the debug USB and OTG USB port have to be powered
during firmware upload.

You may as well load the firmware using the JLink port or openSDA interface with the appropriate tools.
For more options, consult the user guide of the board.
