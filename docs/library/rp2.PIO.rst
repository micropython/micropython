.. currentmodule:: rp2
.. _rp2.PIO:

class PIO -- advanced PIO usage
===============================

The :class:`PIO` class gives access to an instance of the RP2040/RP2350's PIO
(programmable I/O) interface.

The preferred way to interact with PIO is using :class:`rp2.StateMachine`, the
PIO class is for advanced use.

For assembling PIO programs, see :func:`rp2.asm_pio`.


Constructors
------------

.. class:: PIO(id)

    Gets the PIO instance numbered *id*. The RP2040 has two PIO instances
    (0 and 1); the RP2350 has three (0, 1 and 2).

    Raises a ``ValueError`` if any other argument is provided.


Methods
-------

.. method:: PIO.gpio_base([base])

    Query and optionally set the current GPIO base for this PIO instance.

    If an argument is given then it must be a pin (or integer corresponding to a pin
    number), restricted to either GPIO0 or GPIO16.  The GPIO base will then be set to
    that pin.  Setting the GPIO base must be done before any programs are added or state
    machines created.

    Returns the current GPIO base pin.

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

    Gets the state machine numbered *id*. Each PIO instance has four state
    machines, numbered 0 to 3.

    Optionally initialize it with a *program*: see `StateMachine.init`.

    >>> rp2.PIO(1).state_machine(3)
    StateMachine(7)

.. method:: PIO.irq(handler=None, trigger=IRQ_SM0|IRQ_SM1|IRQ_SM2|IRQ_SM3, hard=False)

    Returns the IRQ object for this PIO instance.

    MicroPython only uses IRQ 0 on each PIO instance. IRQ 1 is not available.

    Optionally configure it.

.. method:: PIO.version()

    Returns the hardware version of this PIO instance as an integer:
    0 for PIO v0 (RP2040), 1 for PIO v1 (RP2350).

    This can be used to detect at runtime which PIO features are available.

.. method:: PIO.sm_mask_enable(sm_mask, *, next_mask=0, prev_mask=0)

    Atomically enable state machines across PIO blocks in a single write to
    the ``CTRL`` register.

    *sm_mask* is a 4-bit mask of state machines (bits 0–3) to enable on
    this PIO instance.

    *next_mask* and *prev_mask* are 4-bit masks of state machines to enable
    simultaneously on the neighbouring (next/previous) PIO instance.
    Setting either causes the ``NEXTPREV_SM_ENABLE`` bit to be asserted in
    the same write, making the enable across all specified PIO blocks
    simultaneous.

    Only available on RP2350 (PIO v1).

    Example — start SM0 on PIO0 and SM0 on PIO1 at exactly the same time::

        p0 = rp2.PIO(0)
        p0.sm_mask_enable(0b0001, next_mask=0b0001)


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

.. data:: PIO.JOIN_RX_PUT
          PIO.JOIN_RX_GET

    RP2350 (PIO v1) FIFO join modes, used for the *fifo_join* argument to
    `asm_pio`.

    ``JOIN_RX_PUT``: the state machine writes to its RX FIFO entries with
    ``MOV rxfifo[n], ISR``, and the host reads them with
    `StateMachine.putget`.  The entries act as status registers: the state
    machine updates them and the host reads them at any time.

    ``JOIN_RX_GET``: the host writes to the RX FIFO entries with
    `StateMachine.putget`, and the state machine reads them with
    ``MOV OSR, rxfifo[n]``.  The entries act as configuration registers:
    the host updates them and the state machine reads them at any time.

    Only available on RP2350.

.. data:: PIO.IRQ_SM0
          PIO.IRQ_SM1
          PIO.IRQ_SM2
          PIO.IRQ_SM3

    These constants are used for the *trigger* argument to `PIO.irq`.

