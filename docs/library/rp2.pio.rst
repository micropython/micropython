.. currentmodule:: rp2

PIO -- the RP2040's programmable I/O interface
==============================================

The :mod:`rp2` module includes functions and classes which give access to the RP2040's PIO (programmable I/O) interface.

Assembling PIO programs
-----------------------

.. function:: asm_pio(*, out_init=None, set_init=None, sideset_init=None, in_shiftdir=0, out_shiftdir=0, autopush=False, autopull=False, push_thresh=32, pull_thresh=32)

    Assemble a PIO program.

    The following parameters control the initial state of the GPIO pins, as one of `PIO.IN_LOW`, `PIO.IN_HIGH`, `PIO.OUT_LOW` or `PIO.OUT_HIGH`. If the program uses more than one pin, provide a tuple, e.g. ``out_init=(PIO.OUT_LOW, PIO.OUT_LOW)``.

    - *out_init* configures the pins used for ``out()`` instructions.
    - *set_init* configures the pins used for ``set()`` instructions. There can be at most 5.
    - *sideset_init* configures the pins used side-setting. There can be at most 5.

    The following parameters are used by default, but can be overriden in `state_machine.init()`:

    - *in_shiftdir* is the default direction the ISR will shift, either `PIO.SHIFT_LEFT` or `PIO.SHIFT_RIGHT`.
    - *out_shiftdir* is the default direction the OSR will shift, either `PIO.SHIFT_LEFT` or `PIO.SHIFT_RIGHT`.
    - *push_thresh* is the threshold in bits before auto-push or conditional re-pushing is triggered.
    - *pull_thresh* is the threshold in bits before auto-push or conditional re-pushing is triggered.

    The remaining parameters are:

    - *autopush* configures whether auto-push is enabled.
    - *autopull* configures whether auto-pull is enabled.

.. function:: asm_pio_encode(instr, sideset_count)

    Assemble a single PIO instruction. You usually want to use `asm_pio()` instead.

    >>> rp2.asm_pio_encode("set(0, 1)", 0)
    57345

.. class:: PIOASMError

    This exception is raised from `asm_pio()` or `asm_pio_encode()` if there is an error assembling a PIO program.


PIO class
---------

.. class:: PIO(id)

    Gets the PIO instance numbered *id*. The RP2040 has two PIO instances, numbered 0 and 1.

    Raises a ``ValueError`` if any other argument is provided.

.. method:: pio.add_program(program)

    Add the *program* to the instruction memory of this PIO instance.

    The amount of memory available for programs on each PIO instance is limited. If there isn't enough space left in the PIO's program memory, you'll get ``OSError`` 12 ``ENOMEM``.

.. method:: pio.remove_program([program])

    Remove *program* from the instruction memory of this PIO instance.

    If no program is provided, it removes all programs.

    It is not an error to remove a program which has already been removed.

.. method:: pio.state_machine(id, [program, ...])

    Gets the state machine numbered *id*. On the RP2040, each PIO instance has four state machines, numbered 0 to 3.

    Optionally initialize it with a *program*: see `state_machine.init`.

    >>> rp2.PIO(1).state_machine(3)
    StateMachine(7)

.. method:: pio.irq(handler=None, trigger=IRQ_SM0|IRQ_SM1|IRQ_SM2|IRQ_SM3, hard=False)

    Returns the IRQ object for this PIO instance.

    MicroPython only uses IRQ 0 on each PIO instance. IRQ 1 is not available.

    Optionally configure it.

.. data:: PIO.IN_LOW
          PIO.IN_HIGH
          PIO.OUT_LOW
          PIO.OUT_HIGH

    These constants are used for the *out_init*, *set_init*, and *sideset_init* arguments to `asm_pio`.

.. data:: PIO.SHIFT_LEFT
          PIO.SHIFT_RIGHT

    These constants are used for the *in_shiftdir* and *out_shiftdir* arguments to `asm_pio` or `state_machine.init`.

.. data:: PIO.IRQ_SM0
          PIO.IRQ_SM1
          PIO.IRQ_SM2
          PIO.IRQ_SM3

    These constants are used for the *trigger* argument to `pio.irq`.

StateMachine class
------------------

.. class:: StateMachine(id, [program, ...])

    Get the state machine numbered *id*. The RP2040 has two identical PIO instances, each with 4 state machines: so there are 8 state machines in total, numbered 0 to 7.

    Optionally initialize it with the given program *program*: see `state_machine.init`.

.. method:: state_machine.init(program, freq=-1, *, in_base=None, out_base=None, set_base=None, jmp_pin=None, sideset_base=None, in_shiftdir=None, out_shiftdir=None, push_thresh=None, pull_thresh=None)

    Configure the state machine instance to run the given *program*.

    The program is added to the instruction memory of this PIO instance. If the instruction memory already contains this program, then its offset is re-used so as to save on instruction memory.

    - *freq* is the frequency in Hz to run the state machine at. Defaults to the system clock frequency. 

      The clock divider is computed as ``system clock frequency / freq``, so there can be slight rounding errors.
    - *in_base* is the first pin to use for ``in()`` instructions.
    - *out_base* is the first pin to use for ``out()`` instructions.
    - *set_base* is the first pin to use for ``set()`` instructions.
    - *jmp_pin* is the first pin to use for ``jmp(pin, ...)`` instructions.
    - *sideset_base* is the first pin to use for side-setting.
    - *in_shiftdir* is the direction the ISR will shift, either `PIO.SHIFT_LEFT` or `PIO.SHIFT_RIGHT`.
    - *out_shiftdir* is the direction the OSR will shift, either `PIO.SHIFT_LEFT` or `PIO.SHIFT_RIGHT`.
    - *push_thresh* is the threshold in bits before auto-push or conditional re-pushing is triggered.
    - *pull_thresh* is the threshold in bits before auto-push or conditional re-pushing is triggered.

.. method:: state_machine.active([value])

    Gets or sets whether the state machine is currently running.

    >>> sm.active()
    True
    >>> sm.active(0)
    False

.. method:: state_machine.restart()

    Restarts the state machine and jumps to the beginning of the program.

    This method clears the state machine's internal state using the RP2040's ``SM_RESTART`` register. This includes:

     - input and output shift counters
     - the contents of the input shift register
     - the delay counter
     - the waiting-on-IRQ state
     - a stalled instruction run using `state_machine.exec()`

.. method:: state_machine.exec(instr)

    Execute a single PIO instruction. Uses `asm_pio_encode` to encode the instruction from the given string *instr*.

    >>> sm.exec("set(0, 1)")

.. method:: state_machine.get(buf=None, shift=0)

    Pull a word from the state machine's RX FIFO.

    If the FIFO is empty, it blocks until data arrives (i.e. the state machine pushes a word).

    The value is shifted right by *shift* bits before returning, i.e. the return value is ``word >> shift``.

.. method:: state_machine.put(value, shift=0)

    Push a word onto the state machine's TX FIFO.

    If the FIFO is full, it blocks until there is space (i.e. the state machine pulls a word).

    The value is first shifted left by *shift* bits, i.e. the state machine receives ``value << shift``.

.. method:: state_machine.rx_fifo()

    Returns the number of words in the state machine's RX FIFO. A value of 0 indicates the FIFO is empty.

    Useful for checking if data is waiting to be read, before calling `state_machine.get()`.

.. method:: state_machine.tx_fifo()

    Returns the number of words in the state machine's TX FIFO. A value of 0 indicates the FIFO is empty.

    Useful for checking if there is space to push another word using `state_machine.put()`.

.. method:: state_machine.irq(handler=None, trigger=0|1, hard=False)

     Returns the IRQ object for the given StateMachine.

     Optionally configure it.

