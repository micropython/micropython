The MicroPython Interactive Interpreter Mode (aka REPL)
=======================================================

This section covers some characteristics of the MicroPython Interactive
Interpreter Mode. A commonly used term for this is REPL (read-eval-print-loop)
which will be used to refer to this interactive prompt.

Auto-indent
-----------

When typing python statements which end in a colon (for example if, for, while)
then the prompt will change to three dots (...) and the cursor will be indented
by 4 spaces. When you press return, the next line will continue at the same
level of indentation for regular statements or an additional level of indentation
where appropriate. If you press the backspace key then it will undo one
level of indentation.

If your cursor is all the way back at the beginning, pressing RETURN will then
execute the code that you've entered. The following shows what you'd see
after entering a for statement (the underscore shows where the cursor winds up):

    >>> for i in range(30):
    ...     _

If you then enter an if statement, an additional level of indentation will be
provided:

    >>> for i in range(30):
    ...     if i > 3:
    ...         _

Now enter ``break`` followed by RETURN and press BACKSPACE:

    >>> for i in range(30):
    ...     if i > 3:
    ...         break
    ...     _

Finally type ``print(i)``, press RETURN, press BACKSPACE and press RETURN again:

    >>> for i in range(30):
    ...     if i > 3:
    ...         break
    ...     print(i)
    ...
    0
    1
    2
    3
    >>>

Auto-indent won't be applied if the previous two lines were all spaces.  This
means that you can finish entering a compound statement by pressing RETURN
twice, and then a third press will finish and execute.

Auto-completion
---------------

While typing a command at the REPL, if the line typed so far corresponds to
the beginning of the name of something, then pressing TAB will show
possible things that could be entered. For example, first import the machine
module by entering ``import machine`` and pressing RETURN.
Then type ``m`` and press TAB and it should expand to ``machine``.
Enter a dot ``.`` and press TAB again. You should see something like:

    >>> machine.
    __name__        info            unique_id       reset
    bootloader      freq            rng             idle
    sleep           deepsleep       disable_irq     enable_irq
    Pin

The word will be expanded as much as possible until multiple possibilities exist.
For example, type ``machine.Pin.AF3`` and press TAB and it will expand to
``machine.Pin.AF3_TIM``. Pressing TAB a second time will show the possible
expansions:

    >>> machine.Pin.AF3_TIM
    AF3_TIM10       AF3_TIM11       AF3_TIM8        AF3_TIM9
    >>> machine.Pin.AF3_TIM

Interrupting a running program
------------------------------

You can interrupt a running program by pressing Ctrl-C. This will raise a KeyboardInterrupt
which will bring you back to the REPL, providing your program doesn't intercept the
KeyboardInterrupt exception.

For example:

    >>> for i in range(1000000):
    ...     print(i)
    ...
    0
    1
    2
    3
    ...
    6466
    6467
    6468
    Traceback (most recent call last):
      File "<stdin>", line 2, in <module>
    KeyboardInterrupt:
    >>>

Paste mode
----------

If you want to paste some code into your terminal window, the auto-indent feature
will mess things up. For example, if you had the following python code: ::

   def foo():
       print('This is a test to show paste mode')
       print('Here is a second line')
   foo()

and you try to paste this into the normal REPL, then you will see something like
this:

    >>> def foo():
    ...         print('This is a test to show paste mode')
    ...             print('Here is a second line')
    ...             foo()
    ...
    Traceback (most recent call last):
      File "<stdin>", line 3
    IndentationError: unexpected indent

If you press Ctrl-E, then you will enter paste mode, which essentially turns off
the auto-indent feature, and changes the prompt from ``>>>`` to ``===``. For example:

    >>>
    paste mode; Ctrl-C to cancel, Ctrl-D to finish
    === def foo():
    ===     print('This is a test to show paste mode')
    ===     print('Here is a second line')
    === foo()
    ===
    This is a test to show paste mode
    Here is a second line
    >>>

Paste Mode allows blank lines to be pasted. The pasted text is compiled as if
it were a file. Pressing Ctrl-D exits paste mode and initiates the compilation.

.. _repl_soft_reset:

Soft reset
----------

A :ref:`soft_reset` will reset the python interpreter, but tries not to reset the
method by which you're connected to the MicroPython board (USB-serial, or Wifi).

You can perform a soft reset from the REPL by pressing Ctrl-D, or from your python
code by executing: ::

    machine.soft_reset()

For example, if you reset your MicroPython board, and you execute a dir()
command, you'd see something like this:

    >>> dir()
    ['__name__', 'pyb']

Now create some variables and repeat the dir() command:

    >>> i = 1
    >>> j = 23
    >>> x = 'abc'
    >>> dir()
    ['j', 'x', '__name__', 'pyb', 'i']
    >>>

Now if you enter Ctrl-D, and repeat the dir() command, you'll see that your
variables no longer exist:

.. code-block:: python

    MPY: sync filesystems
    MPY: soft reboot
    MicroPython v1.5-51-g6f70283-dirty on 2015-10-30; PYBv1.0 with STM32F405RG
    Type "help()" for more information.
    >>> dir()
    ['__name__', 'pyb']
    >>>

For more information about reset types and the startup process, see
:doc:`/reference/reset_boot`.

The special variable _ (underscore)
-----------------------------------

When you use the REPL, you may perform computations and see the results.
MicroPython stores the results of the previous statement in the variable _ (underscore).
So you can use the underscore to save the result in a variable. For example:

    >>> 1 + 2 + 3 + 4 + 5
    15
    >>> x = _
    >>> x
    15
    >>>

.. _raw_repl:

Raw mode and raw-paste mode
---------------------------

Raw mode (also called raw REPL) is not something that a person would normally use.
It is intended for programmatic use and essentially behaves like paste mode with
echo turned off, and with optional flow control.

Raw mode is entered using Ctrl-A. You then send your python code, followed by
a Ctrl-D. The Ctrl-D will be acknowledged by 'OK' and then the python code will
be compiled and executed. Any output (or errors) will be sent back. Entering
Ctrl-B will leave raw mode and return the regular (aka friendly) REPL.

Raw-paste mode is an additional mode within the raw REPL that includes flow control,
and which compiles code as it receives it. This makes it more robust for high-speed
transfer of code into the device, and it also uses less RAM when receiving because
it does not need to store a verbatim copy of the code before compiling (unlike
standard raw mode).

Raw-paste mode uses the following protocol:

#. Enter raw REPL as usual via ctrl-A.

#. Write 3 bytes: ``b"\x05A\x01"`` (ie ctrl-E then "A" then ctrl-A).

#. Read 2 bytes to determine if the device entered raw-paste mode:

   * If the result is ``b"R\x00"`` then the device understands the command but
     doesn't support raw paste.

   * If the result is ``b"R\x01"`` then the device does support raw paste and
     has entered this mode.

   * Otherwise the result should be ``b"ra"`` and the device doesn't support raw
     paste and the string ``b"w REPL; CTRL-B to exit\r\n>"`` should be read and
     discarded.

#. If the device is in raw-paste mode then continue, otherwise fallback to
   standard raw mode.

#. Read 2 bytes, this is the flow control window-size-increment (in bytes)
   stored as a 16-bit unsigned little endian integer.  The initial value for the
   remaining-window-size variable should be set to this number.

#. Write out the code to the device:

   * While there are bytes to send, write up to the remaining-window-size worth
     of bytes, and decrease the remaining-window-size by the number of bytes
     written.

   * If the remaining-window-size is 0, or there is a byte waiting to read, read
     1 byte.  If this byte is ``b"\x01"`` then increase the remaining-window-size
     by the window-size-increment from step 5.  If this byte is ``b"\x04"`` then
     the device wants to end the data reception, and ``b"\x04"`` should be
     written to the device and no more code sent after that.  (Note: if there is
     a byte waiting to be read from the device then it does not need to be read
     and acted upon immediately, the device will continue to consume incoming
     bytes as long as reamining-window-size is greater than 0.)

#. When all code has been written to the device, write ``b"\x04"`` to indicate
   end-of-data.

#. Read from the device until ``b"\x04"`` is received.  At this point the device
   has received and compiled all of the code that was sent and is executing it.

#. The device outputs any characters produced by the executing code.  When (if)
   the code finishes ``b"\x04"`` will be output, followed by any exception that
   was uncaught, followed again by ``b"\x04"``.  It then goes back to the
   standard raw REPL and outputs ``b">"``.

For example, starting at a new line at the normal (friendly) REPL, if you write::

    b"\x01\x05A\x01print(123)\x04"

Then the device will respond with something like::

    b"\r\nraw REPL; CTRL-B to exit\r\n>R\x01\x80\x00\x01\x04123\r\n\x04\x04>"

Broken down over time this looks like::

    # Step 1: enter raw REPL
    write: b"\x01"
    read: b"\r\nraw REPL; CTRL-B to exit\r\n>"

    # Step 2-5: enter raw-paste mode
    write: b"\x05A\x01"
    read: b"R\x01\x80\x00\x01"

    # Step 6-8: write out code
    write: b"print(123)\x04"
    read: b"\x04"

    # Step 9: code executes and result is read
    read: b"123\r\n\x04\x04>"

In this case the flow control window-size-increment is 128 and there are two
windows worth of data immediately available at the start, one from the initial
window-size-increment value and one from the explicit ``b"\x01"`` value that
is sent.  So this means up to 256 bytes can be written to begin with before
waiting or checking for more incoming flow-control characters.

The ``tools/pyboard.py`` program uses the raw REPL, including raw-paste mode, to
execute Python code on a MicroPython-enabled board.
