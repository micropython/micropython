# Flashing the Espruino Pico

The Espruino Pico is normally updated via a bootloader activated by the Espruino web app. This approach is not practical for Circuitpython as it takes too much space from the internal filesystem - thus, you will need to follow the instructions for advanced reflashing using the built-in ROM bootloader on all STM32F4 MCUs (instructions sourced from https://www.espruino.com/Pico#advanced-reflashing)

 - Short out the BOOT0/BTN solder jumper on the back of the board - you can do this by drawing over it with a pencil.
 - Install ST's DFU utility on Windows, or dfu-util for Mac or Linux
 	- **Mac**: install with Homebrew: `brew install dfu-util`
 	- **Linux**: install with apt-get: `sudo apt-get install dfu-util`
 	- **Windows**: download [ST's application](https://www.st.com/en/development-tools/stsw-stm32080.html) or install the Linux subsystem for Windows 10 and follow the linux instructions.
 - Hold down the Pico's button while plugging it into USB (when overwriting Espruino's default firmware)
 - Navigate to the same directory as your firmware.bin file for Circuitpython and run the following command: `sudo dfu-util -a 0 -s 0x08000000 -D firmware.bin` or use the ST utility on Windows.
 - Restart the board.


To reinstall Espruino, follow the same steps with the latest Espruino Pico binary from espruino.com/binaries. This will reinstall the usual Espruino bootloader. You must un-short the BOOT0/BTN jumper to re-use the original Espruino Bootloader again. If you used a Pencil mark then you may need to use cleaning fluid and a small brush to totally clear out the graphite.
