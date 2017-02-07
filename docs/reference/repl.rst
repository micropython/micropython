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

    >>> for i in range(3):
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
possible things that could be entered. For example type ``m`` and press TAB
and it should expand to ``machine``. Enter a dot ``.`` and press TAB again. You
should see something like:

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

Paste Mode
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

Soft Reset
----------

A soft reset will reset the python interpreter, but tries not to reset the
method by which you're connected to the MicroPython board (USB-serial, or Wifi).

You can perform a soft reset from the REPL by pressing Ctrl-D, or from your python
code by executing: ::

    raise SystemExit

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

    PYB: sync filesystems
    PYB: soft reboot
    MicroPython v1.5-51-g6f70283-dirty on 2015-10-30; PYBv1.0 with STM32F405RG
    Type "help()" for more information.
    >>> dir()
    ['__name__', 'pyb']
    >>>

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

Raw Mode
--------

Raw mode is not something that a person would normally use. It is intended for
programmatic use. It essentially behaves like paste mode with echo turned off.

Raw mode is entered using Ctrl-A. You then send your python code, followed by
a Ctrl-D. The Ctrl-D will be acknowledged by 'OK' and then the python code will
be compiled and executed. Any output (or errors) will be sent back. Entering
Ctrl-B will leave raw mode and return the the regular (aka friendly) REPL.

The ``tools/pyboard.py`` program uses the raw REPL to execute python files on the
MicroPython board.

