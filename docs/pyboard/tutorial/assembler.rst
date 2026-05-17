.. _pyboard_tutorial_assembler:

Inline assembler
================

Here you will learn how to write inline assembler in MicroPython.

**Note**: this is an advanced tutorial, intended for those who already
know a bit about microcontrollers and assembly language.

MicroPython includes an inline assembler.  It allows you to write
assembly routines as a Python function, and you can call them as you would
a normal Python function.

Returning a value
-----------------

Inline assembler functions are denoted by a special function decorator.
Let's start with the simplest example::

    @micropython.asm_thumb
    def fun():
        movw(r0, 42)

You can enter this in a script or at the REPL.  This function takes no
arguments and returns the number 42.  ``r0`` is a register, and the value
in this register when the function returns is the value that is returned.
MicroPython always interprets the ``r0`` as an integer, and converts it to an
integer object for the caller.

If you run ``print(fun())`` you will see it print out 42.

Accessing peripherals
---------------------

For something a bit more complicated, let's turn on an LED::

    @micropython.asm_thumb
    def led_on():
        movwt(r0, stm.GPIOA)
        movw(r1, 1 << 13)
        strh(r1, [r0, stm.GPIO_BSRRL])

This code uses a few new concepts:

  - ``stm`` is a module which provides a set of constants for easy
    access to the registers of the pyboard's microcontroller.  Try
    running ``import stm`` and then ``help(stm)`` at the REPL.  It will
    give you a list of all the available constants.

  - ``stm.GPIOA`` is the address in memory of the GPIOA peripheral.
    On the pyboard, the red LED is on port A, pin PA13.

  - ``movwt`` moves a 32-bit number into a register.  It is a convenience
    function that turns into 2 thumb instructions: ``movw`` followed by ``movt``.
    The ``movt`` also shifts the immediate value right by 16 bits.

  - ``strh`` stores a half-word (16 bits).  The instruction above stores
    the lower 16-bits of ``r1`` into the memory location ``r0 + stm.GPIO_BSRRL``.
    This has the effect of setting high all those pins on port A for which
    the corresponding bit in ``r0`` is set.  In our example above, the 13th
    bit in ``r0`` is set, so PA13 is pulled high.  This turns on the red LED.

Accepting arguments
-------------------

Inline assembler functions can accept up to 4 arguments.  If they are
used, they must be named ``r0``, ``r1``, ``r2`` and ``r3`` to reflect the registers
and the calling conventions.

Here is a function that adds its arguments::

    @micropython.asm_thumb
    def asm_add(r0, r1):
        add(r0, r0, r1)

This performs the computation ``r0 = r0 + r1``.  Since the result is put
in ``r0``, that is what is returned.  Try ``asm_add(1, 2)``, it should return
3.

Loops
-----

We can assign labels with ``label(my_label)``, and branch to them using
``b(my_label)``, or a conditional branch like ``bgt(my_label)``.

The following example flashes the green LED.  It flashes it ``r0`` times. ::

    @micropython.asm_thumb
    def flash_led(r0):
        # get the GPIOA address in r1
        movwt(r1, stm.GPIOA)

        # get the bit mask for PA14 (the pin LED #2 is on)
        movw(r2, 1 << 14)

        b(loop_entry)

        label(loop1)

        # turn LED on
        strh(r2, [r1, stm.GPIO_BSRRL])

        # delay for a bit
        movwt(r4, 5599900)
        label(delay_on)
        sub(r4, r4, 1)
        cmp(r4, 0)
        bgt(delay_on)

        # turn LED off
        strh(r2, [r1, stm.GPIO_BSRRH])

        # delay for a bit
        movwt(r4, 5599900)
        label(delay_off)
        sub(r4, r4, 1)
        cmp(r4, 0)
        bgt(delay_off)

        # loop r0 times
        sub(r0, r0, 1)
        label(loop_entry)
        cmp(r0, 0)
        bgt(loop1)

Further reading
---------------

For further information about supported instructions of the inline assembler,
see the :ref:`reference documentation <asm_thumb2_index>`.
