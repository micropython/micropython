.. _renesas-ra_reset:

Reset and boot mode
===================

Resetting the board
-------------------

If something goes wrong, you can reset the board in two ways. The first is
to press CTRL-D at the MicroPython prompt, which performs a soft reset.
You will see a message something like ::

    MPY: sync filesystems
    MPY: soft reboot
    MicroPython v1.18-293-g339aa09b8-dirty on 2022-03-26; RA6M2_EK with RA6M2
    Type "help()" for more information.
    >>>

If that isn't working you can perform a hard reset (turn-it-off-and-on-again)
by pressing the RESET button. This will end your session, disconnecting
whatever program (PuTTY, screen, etc) that you used to connect to the board.

boot mode
---------

There are 3 boot modes:

  * normal boot mode
  * safe boot mode
  * factory filesystem boot mode

boot.py and main.py are executed on "normal boot mode".

boot.py and main.py are *NOT* executed on "safe boot mode".

The file system of internal flash is initialized and *all files are erased* on "factory filesystem boot mode".

For changing boot mode, please push the RESET button with pressing USER SW1
on the board:

  * For normal boot mode, release the USER SW1 after LED1 flashes 4 times or more

  * For safe boot mode, release the USER SW1 after LED1 flashes 2 times

  * For factory file system boot mode, release the USER SW1 after LED1 flashes 3 times.

You have created the main.py which executes LED1 blinking in the previous part.
If you change the boot mode to safe boot mode, the MicroPython starts without
the execution of main.py. Then you can remove the main.py by following
command or change the boot mode to factory file system boot mode.::

    import os
    os.remove('main.py')

or change the boot mode to factory file system boot mode.

You can confirm that the initialized file system that there are only boot.py and main.py files.::

    import os
    os.listdir()

