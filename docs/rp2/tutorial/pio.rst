Programmable IO
===============

The RP2xxx family of microcontrollers provides hardware support for standard
communication protocols like I2C, SPI and UART. For protocols where there
is no hardware support, or where there is a requirement of custom I/O
behaviour, Programmable Input Output (PIO) comes into play. Also, some
MicroPython applications make use of a technique called bit banging in which
pins are rapidly turned on and off to transmit data. This can make the entire
process slow as the processor concentrates on bit banging rather than executing
other logic. However, PIO allows bit banging to happen in the background while
the CPU is executing the main work.

PIO State Machines
------------------

State machines transfer data to/from other entities using First-In-First-Out (FIFO)
buffers, which allow the state machine and main processor to work independently
yet also synchronise their data. The FIFOs can be linked to the DMA to transfer
larger amounts of data without extensive CPU usage.


**RP2040**
^^^^^^^^^^

The RP2040 has two PIO blocks, each with four independent state machines (total of 8).

Each FIFO has four words (each of 32 bits) per state machine (TX and RX).

**RP2350**
^^^^^^^^^^

The RP2350 has three PIO blocks, each with four independent state machines (total of 12).

Similar to the RP2040, each state machine has a four-word FIFO per state machine (TX and RX),
but they have some additional functions

For each state machine:

- TX or RX FIFOs can be combined into a single 8-word FIFO for higher bandwidth.
- The RX FIFO supports random read/write access to its registers.

**Compatibility**
^^^^^^^^^^^^^^^^^

RP2350 PIO is backward-compatible with RP2040, with some additional enhancements.

At the moment, native support for RP2350 PIO enhancements is very limited,
but work for some features are already in progress

Instructions
------------

Each PIO block has 32 instructions of memory shared among its four state machines.

All PIO instructions follow a common pattern::

    <instruction> .side(<side_set_value>) [<delay_value>]

The side-set ``.side(...)`` and delay ``[...]`` parts are both optional, and if
specified allow the instruction to perform more than one operation.  This keeps
PIO programs small and efficient.

There are nine instructions which perform the following tasks:

- ``jmp()`` transfers control to a different part of the code
- ``wait()`` pauses until a particular action happens
- ``in_()`` shifts the bits from a source (scratch register or set of pins) to the
  input shift register
- ``out()`` shifts the bits from the output shift register to a destination
- ``push()`` sends data to the RX FIFO
- ``pull()`` receives data from the TX FIFO
- ``mov()`` moves data from a source to a destination
- ``irq()`` sets or clears an IRQ flag
- ``set()`` writes a literal value to a destination

The instruction modifiers are:

- ``.side()`` sets the side-set pins at the start of the instruction
- ``[]`` delays for a certain number of cycles after execution of the instruction

There are also directives:

- ``wrap_target()`` specifies where the program execution will get continued from
- ``wrap()`` specifies the instruction where the control flow of the program will
  get wrapped from
- ``label()`` sets a label for use with ``jmp()`` instructions
- ``word()`` emits a raw 16-bit value which acts as an instruction in the program

An example
----------

Take the ``pio_1hz.py`` example for a simple understanding of how to use the PIO
and state machines. Below is the code for reference.

.. code-block:: python3

    # Example using PIO to blink an LED and raise an IRQ at 1Hz.

    import time
    from machine import Pin
    import rp2


    @rp2.asm_pio(set_init=rp2.PIO.OUT_LOW)
    def blink_1hz():
        # Cycles: 1 + 1 + 6 + 32 * (30 + 1) = 1000
        irq(rel(0))
        set(pins, 1)
        set(x, 31)                  [5]
        label("delay_high")
        nop()                       [29]
        jmp(x_dec, "delay_high")

        # Cycles: 1 + 1 + 6 + 32 * (30 + 1) = 1000
        nop()
        set(pins, 0)
        set(x, 31)                  [5]
        label("delay_low")
        nop()                       [29]
        jmp(x_dec, "delay_low")


    # Create the StateMachine with the blink_1hz program, outputting on Pin(25).
    sm = rp2.StateMachine(0, blink_1hz, freq=2000, set_base=Pin(25))

    # Set the IRQ handler to print the millisecond timestamp.
    sm.irq(lambda p: print(time.ticks_ms()))

    # Start the StateMachine.
    sm.active(1)

This creates an instance of class :class:`rp2.StateMachine` which runs the
``blink_1hz`` program at 2000Hz, and connects to pin 25.  The ``blink_1hz``
program uses the PIO to blink an LED connected to this pin at 1Hz, and also
raises an IRQ as the LED turns on.  This IRQ then calls the ``lambda`` function
which prints out a millisecond timestamp.

The ``blink_1hz`` program is a PIO assembler routine.  It connects to a single
pin which is configured as an output and starts out low.  The instructions do
the following:

- ``irq(rel(0))`` raises the IRQ associated with the state machine.
- The LED is turned on via the ``set(pins, 1)`` instruction.
- The value 31 is put into register X, and then there is a delay for 5 more
  cycles, specified by the ``[5]``.
- The ``nop() [29]`` instruction waits for 30 cycles.
- The ``jmp(x_dec, "delay_high")`` will keep looping to the ``delay_high`` label
  as long as the register X is non-zero, and will also post-decrement X.  Since
  X starts with the value 31 this jump will happen 31 times, so the ``nop() [29]``
  runs 32 times in total (note there is also one instruction cycle taken by the
  ``jmp`` for each of these 32 loops).
- The single ``nop()`` correlates with the cycle used for IRQ raise, and ensures
  the same number of cycles are used for LED on and LED off.
- ``set(pins, 0)`` will turn the LED off by setting pin 25 low.
- Another 32 loops of ``nop() [29]`` and ``jmp(...)`` will execute.
- Because ``wrap_target()`` and ``wrap()`` are not specified, their default will
  be used and execution of the program will wrap around from the bottom to the
  top.  This wrapping does not cost any execution cycles.

The entire routine takes exactly 2000 cycles of the state machine.  Setting the
frequency of the state machine to 2000Hz makes the LED blink at 1Hz.

This example works identically on both RP2040 and RP2350 (using StateMachine ID 0). On
RP2350, higher ``freq`` values are possible due to the faster system clock.

Compatibility Notes
-------------------

PIO programs using the basic instructions shown here are fully compatible between RP2040
and RP2350. The MicroPython ``rp2`` module API (including ``@rp2.asm_pio``,
``StateMachine``, etc.) is the same, though RP2350 supports StateMachine IDs 0–11.

RP2350 offers additional advanced features (new instructions, FIFO modes, security, GPIO
relocation) not covered in this introduction; see the RP2350 datasheet for details. Some
programs may require minor adjustments due to differences in default GPIO reset states
between the two chips.
