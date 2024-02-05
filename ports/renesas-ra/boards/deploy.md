### Renesas RA Family Board via J-Flash Lite

You can download [J-Link Software and Documentation pack](https://www.segger.com/downloads/jlink/) that includes a flash programming tool `J-Flash Lite`.

A `.hex` file can be flashed using `J-Flash Lite`.

1. Start `J-Flash Lite`.
2. Select device name for board.
  - EK-RA4M1: R7FA4M1AB
  - EK-RA4W1: R7FA4W1AD2CNG
  - EK-RA6M1: R7FA6M1AD
  - EK-RA6M2: R7FA6M2AF
  - RA4M1 CLICKER: R7FA4M1AB
3. press `OK`.
4. Select `firmware.hex` file for Data File.
5. Press `Program Device`.

Please refer to the [Getting started with MicroPython on the Renesas RA](https://docs.micropython.org/en/latest/renesas-ra/tutorial/intro.html) about cable connection between the board and your PC.