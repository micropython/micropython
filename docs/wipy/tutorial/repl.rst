Getting a MicroPython REPL prompt
=================================

REPL stands for Read Evaluate Print Loop, and is the name given to the
interactive MicroPython prompt that you can access on the WiPy.  Using
the REPL is by far the easiest way to test out your code and run commands.
You can use the REPL in addition to writing scripts in ``main.py``.

.. _wipy_uart:

To use the REPL, you must connect to the WiPy either via :ref:`telnet <wipy_telnet>`,
or with a USB to serial converter wired to one of the two UARTs on the
WiPy. To enable REPL duplication on UART0 (the one accessible via the expansion board)
do::

   >>> from machine import UART
   >>> import os
   >>> uart = UART(0, 115200)
   >>> os.dupterm(uart)

Place this piece of code inside your ``boot.py`` so that it's done automatically after
reset.

Windows
-------

First you need to install the FTDI drivers for the expansion board's USB to serial
converter. Then you need a terminal software. The best option is to download the
free program PuTTY: `putty.exe <http://www.chiark.greenend.org.uk/~sgtatham/putty/download.html>`_.

**In order to get to the telnet REPL:**

Using putty, select ``Telnet`` as connection type, leave the default port (23)
and enter the IP address of your WiPy (192.168.1.1 when in ``WLAN.AP`` mode),
then click open.

**In order to get to the REPL UART:**

Using your serial program you must connect to the COM port that you found in the
previous step.  With PuTTY, click on "Session" in the left-hand panel, then click
the "Serial" radio button on the right, then enter you COM port (eg COM4) in the
"Serial Line" box.  Finally, click the "Open" button.

Mac OS X
--------

Open a terminal and run::

    $ telnet 192.168.1.1

or::

    $ screen /dev/tty.usbmodem* 115200

When you are finished and want to exit ``screen``, type CTRL-A CTRL-\\. If your keyboard does not have a \\-key (i.e. you need an obscure combination for \\ like ALT-SHIFT-7) you can remap the ``quit`` command:

- create ``~/.screenrc``
- add ``bind q quit``

This will allow you to quit ``screen`` by hitting CTRL-A Q.

Linux
-----

Open a terminal and run::

    $ telnet 192.168.1.1

or::

    $ screen /dev/ttyUSB0 115200

You can also try ``picocom`` or ``minicom`` instead of screen.  You may have to
use ``/dev/ttyUSB01`` or a higher number for ``ttyUSB``.  And, you may need to give
yourself the correct permissions to access this devices (eg group ``uucp`` or ``dialout``,
or use sudo).

Using the REPL prompt
---------------------

Now let's try running some MicroPython code directly on the WiPy.

With your serial program open (PuTTY, screen, picocom, etc) you may see a blank
screen with a flashing cursor.  Press Enter and you should be presented with a
MicroPython prompt, i.e. ``>>>``.  Let's make sure it is working with the obligatory test::

    >>> print("hello WiPy!")
    hello WiPy!

In the above, you should not type in the ``>>>`` characters.  They are there to
indicate that you should type the text after it at the prompt.  In the end, once
you have entered the text ``print("hello WiPy!")`` and pressed Enter, the output
on your screen should look like it does above.

If you already know some Python you can now try some basic commands here.

If any of this is not working you can try either a hard reset or a soft reset;
see below.

Go ahead and try typing in some other commands.  For example::

    >>> from machine import Pin
    >>> import wipy
    >>> wipy.heartbeat(False)  # disable the heartbeat
    >>> led = Pin('GP25', mode=Pin.OUT)
    >>> led(1)
    >>> led(0)
    >>> led.toggle()
    >>> 1 + 2
    3
    >>> 4 // 2
    2
    >>> 20 * 'py'
    'pypypypypypypypypypypypypypypypypypypypy'

Resetting the board
-------------------

If something goes wrong, you can reset the board in two ways. The first is to press CTRL-D
at the MicroPython prompt, which performs a soft reset.  You will see a message something like::

    >>> 
    MPY: soft reboot
    MicroPython v1.4.6-146-g1d8b5e5 on 2015-10-21; WiPy with CC3200
    Type "help()" for more information.
    >>>

If that isn't working you can perform a hard reset (turn-it-off-and-on-again) by pressing the
RST switch (the small black button next to the heartbeat LED). During telnet, this will end
your session, disconnecting whatever program that you used to connect to the WiPy.
