Firmware upload to the Seed ARCH MIX board can be done using the J-Link interface
For that, follow the instructions given by Seed in their Wiki at
https://wiki.seeedstudio.com/Arch_Mix/#flashing-arduino-bootloader-to-arch-mix.
You will need a J-Link debug probe and software. What has been tested was the
Segger JLlink edu or Segger JLink edu mini. As matching loader tool you can use
Segger JFlashLite. The target address for loading is 0x60000000.
