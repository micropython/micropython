### Renesas RA Family Board via J-Flash Lite

You can download [J-Link Software and Documentation pack](https://www.segger.com/downloads/jlink/) that includes a flash programming tool `J-Flash Lite`.

A `.hex` file can be flashed using `J-Flash Lite`.

1. Start `J-Flash Lite`.
2. Select devce name for board.

    |      BOARD	       |      DEVICE NAME       |
    | :------------------: | :--------------------: |
    |   RA4M1 Clicker	   |       R7FA4M1AB        |
    |   EK-RA6M2	       |       R7FA6M2AF        |
    |   EK-RA4M1	       |       R7FA4M1AB        |
    |   EK-RA6M1	       |       R7FA6M1AD        |
    |   EK-RA4W1	       |       R7FA4W1AD2CNG    |

3. press `OK`.  
4. Select `firmware.hex` file for Data File .
5. Press `Program Device`.

Please refer to the [Renesas MicroPython Wiki](https://github.com/renesas/micropython/wiki) about cable connection between the board and your PC and more information.