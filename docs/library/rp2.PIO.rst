.. currentmodule:: rp2
.. _rp2.PIO:

class PIO -- advanced PIO usage
===============================

The :class:`PIO` class gives access to an instance of the RP2040's PIO
(programmable I/O) interface.

The preferred way to interact with PIO is using :class:`rp2.StateMachine`, the
PIO class is for advanced use.

For assembling PIO programs, see :func:`rp2.asm_pio`.


Constructors
------------

.. class:: PIO(id)

    Gets the PIO instance numbered *id*. The RP2040 has two PIO instances,
    numbered 0 and 1.

    Raises a ``ValueError`` if any other argument is provided.


Methods
-------

.. method:: PIO.add_program(program)

    Add the *program* to the instruction memory of this PIO instance.

    The amount of memory available for programs on each PIO instance is
    limited. If there isn't enough space left in the PIO's program memory
    this method will raise ``OSError(ENOMEM)``.

.. method:: PIO.remove_program([program])

    Remove *program* from the instruction memory of this PIO instance.

    If no program is provided, it removes all programs.

    It is not an error to remove a program which has already been removed.

.. method:: PIO.state_machine(id, [program, ...])

    Gets the state machine numbered *id*. On the RP2040, each PIO instance has
    four state machines, numbered 0 to 3.

    Optionally initialize it with a *program*: see `StateMachine.init`.

    >>> rp2.PIO(1).state_machine(3)
    StateMachine(7)

.. method:: PIO.irq(handler=None, trigger=IRQ_SM0|IRQ_SM1|IRQ_SM2|IRQ_SM3, hard=False)

    Returns the IRQ object for this PIO instance.

    MicroPython only uses IRQ 0 on each PIO instance. IRQ 1 is not available.

    Optionally configure it.


Constants
---------

.. data:: PIO.IN_LOW
          PIO.IN_HIGH
          PIO.OUT_LOW
          PIO.OUT_HIGH

    These constants are used for the *out_init*, *set_init*, and *sideset_init*
    arguments to `asm_pio`.

.. data:: PIO.SHIFT_LEFT
          PIO.SHIFT_RIGHT

    These constants are used for the *in_shiftdir* and *out_shiftdir* arguments
    to `asm_pio` or `StateMachine.init`.

.. data:: PIO.JOIN_NONE
          PIO.JOIN_TX
          PIO.JOIN_RX

    These constants are used for the *fifo_join* argument to `asm_pio`.

.. data:: PIO.IRQ_SM0
          PIO.IRQ_SM1
          PIO.IRQ_SM2
          PIO.IRQ_SM3

    These constants are used for the *trigger* argument to `PIO.irq`.

