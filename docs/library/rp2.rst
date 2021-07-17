.. currentmodule:: rp2

:mod:`rp2` --- functionality specific to the RP2040
===================================================

.. module:: rp2
    :synopsis: functionality specific to the RP2

The ``rp2`` module contains functions and classes specific to the RP2040, as
used in the Raspberry Pi Pico.

See the `RP2040 Python datasheet
<https://datasheets.raspberrypi.org/pico/raspberry-pi-pico-python-sdk.pdf>`_
for more information, and `pico-micropython-examples
<https://github.com/raspberrypi/pico-micropython-examples/tree/master/pio>`_
for example code.


PIO related functions
---------------------

The ``rp2`` module includes functions for assembling PIO programs.

For running PIO programs, see :class:`rp2.StateMachine`.

.. function:: asm_pio(*, out_init=None, set_init=None, sideset_init=None, in_shiftdir=0, out_shiftdir=0, autopush=False, autopull=False, push_thresh=32, pull_thresh=32, fifo_join=PIO.JOIN_NONE)

    Assemble a PIO program.

    The following parameters control the initial state of the GPIO pins, as one
    of `PIO.IN_LOW`, `PIO.IN_HIGH`, `PIO.OUT_LOW` or `PIO.OUT_HIGH`. If the
    program uses more than one pin, provide a tuple, e.g.
    ``out_init=(PIO.OUT_LOW, PIO.OUT_LOW)``.

    - *out_init* configures the pins used for ``out()`` instructions.
    - *set_init* configures the pins used for ``set()`` instructions. There can
      be at most 5.
    - *sideset_init* configures the pins used side-setting. There can be at
      most 5.

    The following parameters are used by default, but can be overridden in
    `StateMachine.init()`:

    - *in_shiftdir* is the default direction the ISR will shift, either
      `PIO.SHIFT_LEFT` or `PIO.SHIFT_RIGHT`.
    - *out_shiftdir* is the default direction the OSR will shift, either
      `PIO.SHIFT_LEFT` or `PIO.SHIFT_RIGHT`.
    - *push_thresh* is the threshold in bits before auto-push or conditional
      re-pushing is triggered.
    - *pull_thresh* is the threshold in bits before auto-push or conditional
      re-pushing is triggered.

    The remaining parameters are:

    - *autopush* configures whether auto-push is enabled.
    - *autopull* configures whether auto-pull is enabled.
    - *fifo_join* configures whether the 4-word TX and RX FIFOs should be
      combined into a single 8-word FIFO for one direction only. The options
      are `PIO.JOIN_NONE`, `PIO.JOIN_RX` and `PIO.JOIN_TX`.

.. function:: asm_pio_encode(instr, sideset_count)

    Assemble a single PIO instruction. You usually want to use `asm_pio()`
    instead.

    >>> rp2.asm_pio_encode("set(0, 1)", 0)
    57345

.. class:: PIOASMError

    This exception is raised from `asm_pio()` or `asm_pio_encode()` if there is
    an error assembling a PIO program.

PIO Assembly Language instructions
-----------------------------------

Each state machine has a set of instructions- which allow for a wide range of
behaviourial or location changes to the data. They are written in assembly
language which is later used in a MicroPython program to perform specific
tasks. These instructions are -

jmp(cond, lebel=None)
    condition : 0-7
    <delay_value> : 0-31
    target : instruction offset of label within the program
    see sec 3.4.2 of RPi docs for details

wait(polarity, src, index)
     polarity: 0-1
     src: gpio, pin, irq
     index: 0-31
     wait for high/low on pin 0-31 or irq 0-31
     see sec 3.4.3 of RPi docs for details

in(src, data)
    src : pin, X, Y, ISR, OSR(all 0-31)
    data : bit count(0-31)
    see sec 3.4.4 of RPi docs for details

out(dist,data)
    dist : pin, X, Y, PINDIRS, AC, ISR, OSR(all 0-31)
    data : bit count(0-31)
    see sec 3.4.5 of RPi docs for details

push(value = 0, value = 2)
    block = 0 : no block
    block = 1 : block
    see sec 3.4.6 of RPi docs for details

pull(value = 0, value = 2)
    pull(if empty) : 1 (default = 0)
    block : 1 (default = 1)  
    no-block = 0
    see sec 3.4.7 of RPi docs for details

mov(dist, src)
    dist : pin, X, Y, ISR, OSR, PC (all 0-31)
    src : pin, X, Y, ISR, OSR (all 0-31)
    see sec 3.4.8 of RPi docs for details

set(dest, data)
    dest : X, Y, PINS, PINDIRS
    data : value (0-31)
    see sec 3.4.10 of RPi docs for details

irq(mod, index = None)
    These are the following forms of this instruction(index is an integer value 
    from 0-7)
        * irq(index)
    * irq(rel(index))
    * irq(block, index)
    * irq(block, rel(index))
    * irq(clear, rel(index))
    * irq(block | clear, index)
    * irq(block | clear, rel(index))
    
    These can be summarised as two forms:
    * form 1: irq(index)
    * form 2: irq(mode,index)
      where
      * index can be an integer or rel(integer)
      * mode can be: block or clear or block | clear
      
    Allowed IRQ numbers are 0-7 (0-3 are visible from to the processor, 4-7 are 
    internal to the state machines).          
   see sec 3.4.9 of RPi docs for details




Classes
-------

.. toctree::
    :maxdepth: 1

    rp2.Flash.rst
    rp2.PIO.rst
    rp2.StateMachine.rst
