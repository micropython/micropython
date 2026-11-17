## 1. Deploy the MicroPython firmware to  the Metro M7 board.

The Metro M7 board comes pre-installed with a UF2 bootloader. It can
be started by pushing reset twice. Then the bootloader drive will
appear. If that does not happen or the bootloader was lost, you can
reinstall the bootloader using the instructions by Adafruit
here: https://learn.adafruit.com/adafruit-metro-m7-microsd/installing-the-bootloader

Once the bootloader is installed and started, you can install MicroPython
by copying the .uf2 version of the firmware file to the bootloader
drive. A LED on the board will start flickering, indicating that the
upload is ongoing. Once the upload is complete, the drive icon will
disappear. Wait until the LED stops flickering. In rare cases there
may be an error message coming up, especially when there are only few
or no changes in the firmware file. Then just repeat the upload.