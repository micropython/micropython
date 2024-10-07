## 1. Deploy the MicroPython firmware to  the Metro M7 board.

The Metro M7 board comes pre-installed with a UF2 bootloader. It can
be started by pushing reset twice. Then the bootloader drive will
appear. If that does not happen or the bootloader was lost, you can
reinstall the bootloader using the instructions by Adafruit
here: https://learn.adafruit.com/adafruit-metro-m7-microsd/installing-the-bootloader

Once the bootloader is installed and started, you can install MicroPython
by copying the .uf2 version of the firmware file to the bootloader
drive. When the firmware is installed, the drive will disappear.
