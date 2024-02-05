Getting a MicroPython REPL prompt
=================================

REPL stands for Read Evaluate Print Loop, and is the name given to the
interactive MicroPython prompt that you can access on the ESP8266.  Using the
REPL is by far the easiest way to test out your code and run commands.

There are two ways to access the REPL: either via a wired connection through the
UART serial port, or via WiFi.

REPL over the serial port
-------------------------

The REPL is always available on the UART0 serial peripheral, which is connected
to the pins GPIO1 for TX and GPIO3 for RX.  The baudrate of the REPL is 115200.
If your board has a USB-serial converter on it then you should be able to access
the REPL directly from your PC.  Otherwise you will need to have a way of
communicating with the UART.

To access the prompt over USB-serial you need to use a terminal emulator program.
On Windows TeraTerm is a good choice, on Mac you can use the built-in ``screen``
program, and Linux has ``picocom`` and ``minicom``.  Of course, there are many
other terminal programs that will work, so pick your favourite!

For example, on Linux you can try running::

    picocom /dev/ttyUSB0 -b115200

Once you have made the connection over the serial port you can test if it is
working by hitting enter a few times.  You should see the Python REPL prompt,
indicated by ``>>>``.

WebREPL - a prompt over WiFi
----------------------------

WebREPL allows you to use the Python prompt over WiFi, connecting through a
browser. The latest versions of Firefox and Chrome are supported.

For your convenience, WebREPL client is hosted at
`<http://micropython.org/webrepl>`__. Alternatively, you can install it
locally from the the GitHub repository
`<https://github.com/micropython/webrepl>`__.

Before connecting to WebREPL, you should set a password and enable it via
a normal serial connection. Initial versions of MicroPython for ESP8266
came with WebREPL automatically enabled on the boot and with the
ability to set a password via WiFi on the first connection, but as WebREPL
was becoming more widely known and popular, the initial setup has switched
to a wired connection for improved security::

    import webrepl_setup

Follow the on-screen instructions and prompts. To make any changes active,
you will need to reboot your device.

To use WebREPL connect your computer to the ESP8266's access point
(MicroPython-xxxxxx, see the previous section about this).  If you have
already reconfigured your ESP8266 to connect to a router then you can
skip this part.

Once you are on the same network as the ESP8266 you click the "Connect" button
(if you are connecting via a router then you may need to change the IP address,
by default the IP address is correct when connected to the ESP8266's access
point).  If the connection succeeds then you should see a password prompt.

Once you type the password configured at the setup step above, press Enter once
more and you should get a prompt looking like ``>>>``.  You can now start
typing Python commands!

Using the REPL
--------------

Once you have a prompt you can start experimenting!  Anything you type at the
prompt will be executed after you press the Enter key.  MicroPython will run
the code that you enter and print the result (if there is one).  If there is an
error with the text that you enter then an error message is printed.

Try typing the following at the prompt::

    >>> print('hello esp8266!')
    hello esp8266!

Note that you shouldn't type the ``>>>`` arrows, they are there to indicate that
you should type the text after it at the prompt.  And then the line following is
what the device should respond with.  In the end, once you have entered the text
``print("hello esp8266!")`` and pressed the Enter key, the output on your screen
should look exactly like it does above.

If you already know some python you can now try some basic commands here.   For
example::

    >>> 1 + 2
    3
    >>> 1 / 2
    0.5
    >>> 12**34
    4922235242952026704037113243122008064

If your board has an LED attached to GPIO2 (the ESP-12 modules do) then you can
turn it on and off using the following code::

    >>> import machine
    >>> pin = machine.Pin(2, machine.Pin.OUT)
    >>> pin.on()
    >>> pin.off()

Note that ``on`` method of a Pin might turn the LED off and ``off`` might
turn it on (or vice versa), depending on how the LED is wired on your board.
To resolve this, machine.Signal class is provided.

Line editing
~~~~~~~~~~~~

You can edit the current line that you are entering using the left and right
arrow keys to move the cursor, as well as the delete and backspace keys.  Also,
pressing Home or ctrl-A moves the cursor to the start of the line, and pressing
End or ctrl-E moves to the end of the line.

Input history
~~~~~~~~~~~~~

The REPL remembers a certain number of previous lines of text that you entered
(up to 8 on the ESP8266).  To recall previous lines use the up and down arrow
keys.

Tab completion
~~~~~~~~~~~~~~

Pressing the Tab key will do an auto-completion of the current word that you are
entering.  This can be very useful to find out functions and methods that a
module or object has.  Try it out by typing "ma" and then pressing Tab.  It
should complete to "machine" (assuming you imported machine in the above
example).  Then type "." and press Tab again to see a list of all the functions
that the machine module has.

Line continuation and auto-indent
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Certain things that you type will need "continuing", that is, will need more
lines of text to make a proper Python statement.  In this case the prompt will
change to ``...`` and the cursor will auto-indent the correct amount so you can
start typing the next line straight away.  Try this by defining the following
function::

    >>> def toggle(p):
    ...    p.value(not p.value())
    ...    
    ...    
    ...    
    >>>

In the above, you needed to press the Enter key three times in a row to finish
the compound statement (that's the three lines with just dots on them).  The
other way to finish a compound statement is to press backspace to get to the
start of the line, then press the Enter key.  (If you did something wrong and
want to escape the continuation mode then press ctrl-C; all lines will be
ignored.)

The function you just defined allows you to toggle a pin.  The pin object you
created earlier should still exist (recreate it if it doesn't) and you can
toggle the LED using::

    >>> toggle(pin)

Let's now toggle the LED in a loop (if you don't have an LED then you can just
print some text instead of calling toggle, to see the effect)::

    >>> import time
    >>> while True:
    ...     toggle(pin)
    ...     time.sleep_ms(500)
    ...    
    ...    
    ...    
    >>>

This will toggle the LED at 1Hz (half a second on, half a second off).  To stop
the toggling press ctrl-C, which will raise a KeyboardInterrupt exception and
break out of the loop.

The time module provides some useful functions for making delays and doing
timing.  Use tab completion to find out what they are and play around with them!

Paste mode
~~~~~~~~~~

Pressing ctrl-E will enter a special paste mode.  This allows you to copy and
paste a chunk of text into the REPL.  If you press ctrl-E you will see the
paste-mode prompt::

    paste mode; Ctrl-C to cancel, Ctrl-D to finish
    === 

You can then paste (or type) your text in.  Note that none of the special keys
or commands work in paste mode (eg Tab or backspace), they are just accepted
as-is.  Press ctrl-D to finish entering the text and execute it.

Other control commands
~~~~~~~~~~~~~~~~~~~~~~

There are four other control commands:

* Ctrl-A on a blank line will enter raw REPL mode.  This is like a permanent
  paste mode, except that characters are not echoed back.

* Ctrl-B on a blank like goes to normal REPL mode.

* Ctrl-C cancels any input, or interrupts the currently running code.

* Ctrl-D on a blank line will do a soft reset.

Note that ctrl-A and ctrl-D do not work with WebREPL.
