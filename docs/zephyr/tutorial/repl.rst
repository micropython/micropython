Getting a MicroPython REPL prompt
=================================

REPL stands for Read Evaluate Print Loop, and is the name given to the
interactive MicroPython prompt that you can access on your board through
Zephyr. It is recommended to use REPL to test out your code and run commands.

REPL over the serial port
-------------------------

The REPL is available on a UART serial peripheral specified for the board by
the ``zephyr,console`` devicetree node. The baudrate of the REPL is 115200.
If your board has a USB-serial converter on it then you should be able to access
the REPL directly from your PC.

To access the prompt over USB-serial you will need to use a terminal emulator
program. For a Linux or Mac machine, open a terminal and run::

        screen /dev/ttyACM0 115200

You can also try ``picocom`` or ``minicom`` instead of screen. You may have to use
``/dev/ttyACM1`` or a higher number for ``ttyACM``. Additional permissions
may be necessary to access this device (eg group ``uucp`` or ``dialout``, or use sudo).
For Windows, get a terminal software, such as puTTY and connect via a serial session
using the proper COM port.

Using the REPL
--------------

With your serial program open (PuTTY, screen, picocom, etc) you may see a
blank screen with a flashing cursor. Press Enter (or reset the board) and
you should be presented with the following text::

        *** Booting Zephyr OS build v4.0.0 ***
        MicroPython v1.24.0-preview.179.g5b85b24bd on 2024-08-05; zephyr-frdm_k64f with mk64f12
        Type "help()" for more information.
        >>>

Now you can try running MicroPython code directly on your board.

Anything you type at the prompt, indicated by ``>>>``, will be executed after you press
the Enter key. If there is an error with the text that you enter then an error
message is printed.

Start by typing the following at the prompt to make sure it is working::

        >>> print("hello world!")
        hello world!

If you already know some python you can now try some basic commands here. For
example::

        >>> 1 + 2
        3
        >>> 1 / 2
        0.5
        >>> 3 * 'Zephyr'
        ZephyrZephyrZephyr

If your board has an LED, you can blink it using the following code::

        >>>import time
        >>>from machine import Pin

        >>>LED = Pin(("GPIO_1", 21), Pin.OUT)
        >>>while True:
        ...    LED.value(1)
        ...    time.sleep(0.5)
        ...    LED.value(0)
        ...    time.sleep(0.5)

The above code uses an LED location for a FRDM-K64F board (port B, pin 21;
following Zephyr conventions ports are identified by "GPIO_x", where *x*
starts from 0). You will need to adjust it for another board using the board's
reference materials.
