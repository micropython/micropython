Programmable IO
===============

The RP2040 has hardware support for standard communication protocols like I2C,
SPI and UART. For protocols where there is no hardware support, or where there
is a requirement of custom I/O behaviour, Programmable Input Output (PIO) comes
into play.  Also, some MicroPython applications make use of a technique called
bit banging in which pins are rapidly turned on and off to transmit data.  This
can make the entire process slow as the processor concentrates on bit banging
rather than executing other logic.  However, PIO allows bit banging to happen
in the background while the CPU is executing the main work.

Along with the two central Cortex-M0+ processing cores, the RP2040 has two PIO
blocks each of which has four independent state machines.  These state machines
can transfer data to/from other entities using First-In-First-Out (FIFO) buffers,
which allow the state machine and main processor to work independently yet also
synchronise their data.  Each FIFO has four words (each of 32 bits) which can be
linked to the DMA to transfer larger amounts of data.

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

Variables inside PIO
----------

Due to the process of converting pythonic PioASM-Code into HexCode, 
it is not possible to access to global references inside a pio-function. 
However, keyword arguments can be used to manipulate the asm-program.

This mechanic allows to write a pio program "template" 
and calculate/determine the necessary values for the PIO-variables at runtime.

The following example demonstrates how to calculate the delay for a given frequency
and how to use the calculated values in the PIO program.

.. code-block:: python3

    def calc_delay(freq):
        """Calculate the pio-variables for a given frequency"""
        half_period = int(1000/freq) # 2000Hz PIO frequency, devided in high and low phase
        error = round(1000/half_period - freq, 3) # calculate the integer cast error
        loop_count = (half_period // 31) # 31 cycles for the delay loop
        set_delay = half_period - loop_count * 31 - 2 # 2 cycles for the set and irq instructions
        loop_count -= 1  # compensate for the first loop
        assert 31 >= loop_count >= 0, "Frequency out of range"
        assert 31 >= set_delay >= 0, "Frequency out of range"
        return loop_count, set_delay, error

    
    def blink_template(loop_count, set_delay):
        # Cycles: 1 + 1 + 1 + set_delay + (loop_count+1) * (30 + 1)
        irq(rel(0))
        set(pins, 1)
        set(x, loop_count)          [set_delay]
        label("delay_high")
        nop()                       [29]
        jmp(x_dec, "delay_high")

        # Cycles: 1 + 1 + 1 + set_delay + (loop_count+1) * (30 + 1)
        nop()
        set(pins, 0)
        set(x, loop_count)          [set_delay]
        label("delay_low")
        nop()                       [29]
        jmp(x_dec, "delay_low")

    
    # Example using PIO to blink an LED and raise an IRQ at dynamic frequency.
    freq = 1
    loop_count, set_delay, error = calc_delay(freq)
    
    # call the asm-decorator with the calculated values
    blink_freq = rp2.asm_pio(set_init=rp2.PIO.OUT_LOW)(blink_template, loop_count, set_delay)

    # Create the StateMachine with the blink program, outputting on Pin(25).
    sm = rp2.StateMachine(0, pio_code_blink, freq=2000, set_base=Pin(25))

    # Set the IRQ handler to print the millisecond timestamp.
    sm.irq(lambda p: print(time.ticks_ms()))

    # Start the StateMachine.
    sm.active(1)