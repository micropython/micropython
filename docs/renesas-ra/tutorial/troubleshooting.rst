.. _renesas-ra_troubleshooting:

Trouble Shooting
===================

Flash file system
-------------------

* MicroPython REPL prompt is not displayed.

  - Re-program the MicroPython image file.

  - Confirm serial port connection.
    The connection must be crossed that the board TXD pin is connected with
    USB-serial conversion's RXD signal pin.

  - If the prompt is not displayed suddenly, try to do factory file
    system boot mode as the final method.  Please note that all files are removed forever.
