This is the board description folder of the Nucleo G474RE.

Please note: as Micropython does not yet support the LPUART, a hardware mod is needed to get MicroPython REPL on the USB port on the board.

The DEBUG connector CN4 has some additional solder pads. Solder pad 13 and 14 are the UART connection to the STLINK processor.

To get REPL, connect 
* pin CN.13 to CN10.18 (signal T_VCP_RX)
* pin CN.14 to CN9.7 (See bottom) (signal T_VCP_TX)
