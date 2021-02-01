Getting a MicroPython REPL prompt
=================================

REPL stands for Read Evaluate Print Loop, and is the name given to the
interactive MicroPython prompt that you can access on the pyboard.  Using
the REPL is by far the easiest way to test out your code and run commands.
You can use the REPL in addition to writing scripts in ``main.py``.

To use the REPL, you must connect to the serial USB device on the pyboard.
How you do this depends on your operating system.

Windows
-------

You need to install the pyboard driver to use the serial USB device.
The driver is on the pyboard's USB flash drive, and is called ``pybcdc.inf``.

To install this driver you need to go to Device Manager
for your computer, find the pyboard in the list of devices (it should have
a warning sign next to it because it's not working yet), right click on
the pyboard device, select Properties, then Install Driver.  You need to
then select the option to find the driver manually (don't use Windows auto update),
navigate to the pyboard's USB drive, and select that.  It should then install.
After installing, go back to the Device Manager to find the installed pyboard,
and see which COM port it is (eg COM4).
More comprehensive instructions can be found in the
`Guide for pyboard on Windows (PDF) <http://micropython.org/resources/Micro-Python-Windows-setup.pdf>`_.
Please consult this guide if you are having problems installing the driver.

You now need to run your terminal program.  You can use HyperTerminal if you
have it installed, or download the free program PuTTY:
`putty.exe <http://www.chiark.greenend.org.uk/~sgtatham/putty/download.html>`_.
Using your serial program you must connect to the COM port that you found in the
previous step.  With PuTTY, click on "Session" in the left-hand panel, then click
the "Serial" radio button on the right, then enter you COM port (eg COM4) in the
"Serial Line" box.  Finally, click the "Open" button.

Mac OS X
--------

Open a terminal and run::

    screen /dev/tty.usbmodem*

When you are finished and want to exit screen, type CTRL-A CTRL-\\.

Linux
-----

Open a terminal and run::

    screen /dev/ttyACM0

You can also try ``picocom`` or ``minicom`` instead of screen.  You may have to
use ``/dev/ttyACM1`` or a higher number for ``ttyACM``.  And, you may need to give
yourself the correct permissions to access this devices (eg group ``uucp`` or ``dialout``,
or use sudo).

Using the REPL prompt
---------------------

Now let's try running some MicroPython code directly on the pyboard.

With your serial program open (PuTTY, screen, picocom, etc) you may see a blank
screen with a flashing cursor.  Press Enter and you should be presented with a
MicroPython prompt, i.e. ``>>>``.  Let's make sure it is working with the obligatory test::

    >>> print("hello pyboard!")
    hello pyboard!

In the above, you should not type in the ``>>>`` characters.  They are there to
indicate that you should type the text after it at the prompt.  In the end, once
you have entered the text ``print("hello pyboard!")`` and pressed Enter, the output
on your screen should look like it does above.

If you already know some python you can now try some basic commands here. 

If any of this is not working you can try either a hard reset or a soft reset;
see below.

Go ahead and try typing in some other commands.  For example::

    >>> pyb.LED(1).on()
    >>> pyb.LED(2).on()
    >>> 1 + 2
    3
    >>> 1 / 2
    0.5
    >>> 20 * 'py'
    'pypypypypypypypypypypypypypypypypypypypy'

Resetting the board
-------------------

If something goes wrong, you can reset the board in two ways. The first is to press CTRL-D
at the MicroPython prompt, which performs a soft reset.  You will see a message something like ::

    >>> 
    MPY: sync filesystems
    MPY: soft reboot
    Micro Python v1.0 on 2014-05-03; PYBv1.0 with STM32F405RG
    Type "help()" for more information.
    >>>

If that isn't working you can perform a hard reset (turn-it-off-and-on-again) by pressing the RST
switch (the small black button closest to the micro-USB socket on the board). This will end your
session, disconnecting whatever program (PuTTY, screen, etc) that you used to connect to the pyboard.

If you are going to do a hard-reset, it's recommended to first close your serial program and eject/unmount
the pyboard drive.
