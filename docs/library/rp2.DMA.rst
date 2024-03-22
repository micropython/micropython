.. currentmodule:: rp2
.. _rp2.DMA:

class DMA -- access to the RP2040's DMA controller
==================================================

The :class:`DMA` class offers access to the RP2040's Direct Memory Access (DMA)
controller, providing the ability move data between memory blocks and/or IO registers. The DMA
controller has its own, separate read and write bus master connections onto the bus fabric and
each DMA channel can independently read data from one address and write it back to another
address, optionally incrementing one or both pointers, allowing it to perform transfers on behalf
of the processor while the processor carries out other tasks or enters a low power state. The
RP2040's DMA controller has 12 independent DMA channels that can run concurrently. For full
details of the RP2040's DMA system see section 2.5 of the `RP2040 Datasheet
<https://datasheets.raspberrypi.org/rp2040/rp2040-datasheet.pdf>`_.

Examples
--------

The simplest use of the DMA controller is to move data from one block of memory to another.
This can be accomplished with the following code::

    a = bytearray(32*1024)
    b = bytearray(32*1024)
    d = rp2.DMA()
    c = d.pack_ctrl()  # Just use the default control value.
    # The count is in 'transfers', which defaults to four-byte words, so divide length by 4
    d.config(read=a, write=b, count=len(a)//4, ctrl=c, trigger=True)
    # Wait for completion
    while d.active():
        pass

Note that while this example sits in an idle loop while it waits for the transfer to complete,
the program could just as well do some useful work in this time instead.

Another, perhaps more common use of the DMA controller is to transfer between memory and an IO
peripheral. In this situation the address of the IO register does not change for each transfer but
the memory address needs to be incremented. It is also necessary to control the pace of the
transfer so as to not write data before it can be accepted by a peripheral or read it before the
data is ready, and this can be controlled with the ``treq_sel`` field of the DMA channel's control
register. The various fields of the control register for each DMA channel can be packed
using the :meth:`DMA.pack_ctrl()` method and unpacked using the :meth:`DMA.unpack_ctrl()`
static method. Code to transfer data from a byte array to the TX FIFO of a PIO state machine,
one byte at a time, looks like this::

    # pio_num is index of the PIO block being used, sm_num is the state machine in that block.
    # my_state_machine is an rp2.PIO() instance.
    DATA_REQUEST_INDEX = (pio_num << 3) + sm_num

    src_data = bytearray(1024)
    d = rp2.DMA()

    # Transfer bytes, rather than words, don't increment the write address and pace the transfer.
    c = d.pack_ctrl(size=0, inc_write=False, treq_sel=DATA_REQUEST_INDEX)

    d.config(
        read=src_data,
        write=my_state_machine,
        count=len(src_data),
        ctrl=c,
        trigger=True
    )

Note that in this example the value given for the write address is just the PIO state machine to
which we are sending the data. This works because PIO state machines present the buffer protocol,
allowing direct access to their data FIFO registers.

Constructor
-----------

.. class:: DMA()

    Claim one of the DMA controller channels for exclusive use.

Methods
-------

.. method:: DMA.config(read=None, write=None, count=None, ctrl=None, trigger=False)

    Configure the DMA registers for the channel and optionally start the transfer.
    Parameters are:

    - *read*: The address from which the DMA controller will start reading data or
      an object that will provide data to be read. It can be an integer or any
      object that supports the buffer protocol.
    - *write*: The address to which the DMA controller will start writing or an
      object into which data will be written. It can be an integer or any object
      that supports the buffer protocol.
    - *count*: The number of bus transfers that will execute before this channel
      stops. Note that this is the number of transfers, not the number of bytes.
      If the transfers are 2 or 4 bytes wide then the total amount of data moved
      (and thus the size of required buffer) needs to be multiplied accordingly.
    - *ctrl*: The value for the DMA control register. This is an integer value
      that is typically packed using the :meth:`DMA.pack_ctrl()`.
    - *trigger*: Optionally commence the transfer immediately.

.. method:: DMA.irq(handler=None, hard=False)

    Returns the IRQ object for this DMA channel and optionally configures it.

.. method:: DMA.close()

    Release the claim on the underlying DMA channel and free the interrupt
    handler. The :class:`DMA` object can not be used after this operation.

.. method:: DMA.pack_ctrl(default=None, **kwargs)

    Pack the values provided in the keyword arguments into the named fields of a new control
    register value. Any field that is not provided will be set to a default value. The
    default will either be taken from the provided ``default`` value, or if that is not
    given, a default suitable for the current channel; setting this to the current value
    of the `DMA.ctrl` attribute provides an easy way to override a subset of the fields.

    The keys for the keyword arguments can be any key returned by the :meth:`DMA.unpack_ctrl()`
    method. The writable values are:

    - *enable*: ``bool`` Set to enable the channel (default: ``True``).

    - *high_pri*: ``bool`` Make this channel's bus traffic high priority (default: ``False``).

    - *size*: ``int`` Transfer size: 0=byte, 1=half word, 2=word (default: 2).

    - *inc_read*: ``bool`` Increment the read address after each transfer (default: ``True``).

    - *inc_write*: ``bool`` Increment the write address after each transfer (default: ``True``).

    - *ring_size*: ``int`` If non-zero, only the bottom ``ring_size`` bits of one
      address register will change when an address is incremented, causing the
      address to wrap at the next ``1 << ring_size`` byte boundary. Which
      address is wrapped is controlled by the ``ring_sel`` flag. A zero value
      disables address wrapping.

    - *ring_sel*: ``bool`` Set to ``False`` to have the ``ring_size`` apply to the read address
      or ``True`` to apply to the write address.

    - *chain_to*: ``int`` The channel number for a channel to trigger after this transfer
      completes. Setting this value to this DMA object's own channel number
      disables chaining (this is the default).

    - *treq_sel*: ``int`` Select a Transfer Request signal. See section 2.5.3 in the RP2040
      datasheet for details.

    - *irq_quiet*: ``bool`` Do not generate interrupt at the end of each transfer. Interrupts
      will instead be generated when a zero value is written to the trigger
      register, which will halt a sequence of chained transfers (default:
      ``True``).

    - *bswap*: ``bool`` If set to true, bytes in words or half-words will be reversed before
      writing (default: ``True``).

    - *sniff_en*: ``bool`` Set to ``True`` to allow data to be accessed by the chips sniff
      hardware (default: ``False``).

    - *write_err*: ``bool`` Setting this to ``True`` will clear a previously reported write
      error.

    - *read_err*: ``bool`` Setting this to ``True`` will clear a previously reported read
      error.

    See the description of the ``CH0_CTRL_TRIG`` register in section 2.5.7 of the RP2040
    datasheet for details of all of these fields.

.. method:: DMA.unpack_ctrl(value)

    Unpack a value for a DMA channel control register into a dictionary with key/value pairs
    for each of the fields in the control register.  *value* is the ``ctrl`` register value
    to unpack.

    This method will return values for all the keys that can be passed to ``DMA.pack_ctrl``.
    In addition, it will also return the read-only flags in the control register: ``busy``,
    which goes high when a transfer starts and low when it ends, and ``ahb_err``, which is
    the logical OR of the ``read_err`` and ``write_err`` flags. These values will be ignored
    when packing, so that the dictionary created by unpacking a control register can be used
    directly as the keyword arguments for packing.

.. method:: DMA.active([value])

    Gets or sets whether the DMA channel is currently running.

    >>> sm.active()
    0
    >>> sm.active(1)
    >>> while sm.active():
    ...     pass

Attributes
----------

.. attribute:: DMA.read

    This attribute reflects the address from which the next bus transfer
    will read. It may be written with either an integer or an object
    that supports the buffer protocol and doing so has immediate effect.

.. attribute:: DMA.write

    This attribute reflects the address to which the next bus transfer
    will write.  It may be written with either an integer or an object
    that supports the buffer protocol and doing so has immediate effect.

.. attribute:: DMA.count

    Reading this attribute will return the number of remaining bus
    transfers in the *current* transfer sequence. Writing this attribute
    sets the total number of transfers to be the *next* transfer sequence.

.. attribute:: DMA.ctrl

    This attribute reflects DMA channel control register. It is typically written
    with an integer packed using the :meth:`DMA.pack_ctrl()` method. The returned
    register value can be unpacked using the :meth:`DMA.unpack_ctrl()` method.

.. attribute:: DMA.channel

    The channel number of the DMA channel. This can be passed in the ``chain_to``
    argument of `DMA.pack_ctrl()` on another channel to allow DMA chaining.

.. attribute:: DMA.registers

    This attribute is an array-like object that allows direct access to
    the DMA channel's registers. The index is by word, rather than by byte,
    so the register indices are the register address offsets divided by 4.
    See the RP2040 data sheet for register details.

Chaining and trigger register access
------------------------------------

The DMA controller in the RP2040 offers a couple advanced features to allow one DMA channel
to initiate a transfer on another channel. One is the use of the ``chain_to`` value in the
control register and the other is writing to one of the DMA channel's registers that has a
trigger effect. When coupled with the ability to have one DMA channel write directly to the
`DMA.registers` of another channel, this allows for complex transactions to be performed
without any CPU intervention.

Below is an example of using both chaining and register
triggering to implement gathering of multiple blocks of data into a single destination. Full
details of these features can be found in section 2.5 of the RP2040 data sheet and the code
below is a Pythonic version of the example in sub-section 2.5.6.2.

.. code-block:: python

    from rp2 import DMA
    from uctypes import addressof
    from array import array

    def gather_strings(string_list, buf):
        # We use two DMA channels. The first sends lengths and source addresses from the gather
        # list to the registers of the second. The second copies the data itself.
        gather_dma = DMA()
        buffer_dma = DMA()

        # Pack up length/address pairs to be sent to the registers.
        gather_list = array("I")

        for s in string_list:
            gather_list.append(len(s))
            gather_list.append(addressof(s))

        gather_list.append(0)
        gather_list.append(0)

        # When writing to the registers of the second DMA channel, we need to wrap the
        # write address on an 8-byte (1<<3 bytes) boundary. We write to the ``TRANS_COUNT``
        # and ``READ_ADD_TRIG`` registers in the last register alias (registers 14 and 15).
        gather_ctrl = gather_dma.pack_ctrl(ring_size=3, ring_sel=True)
        gather_dma.config(
            read=gather_list, write=buffer_dma.registers[14:16],
            count=2, ctrl=gather_ctrl
        )

        # When copying the data, the transfer size is single bytes, and when completed we need
        # to chain back to the start another gather DMA transaction.
        buffer_ctrl = buffer_dma.pack_ctrl(size=0, chain_to=gather_dma.channel)
        # The read and count values will be set by the other DMA channel.
        buffer_dma.config(write=buf, ctrl=buffer_ctrl)

        # Set the transfer in motion.
        gather_dma.active(1)

        # Wait until all the register values have been sent
        end_address = addressof(gather_list) + 4 * len(gather_list)
        while gather_dma.read != end_address:
            pass

    input = ["This is ", "a ", "test", " of the scatter", " gather", " process"]
    output = bytearray(64)

    print(output)
    gather_strings(input, output)
    print(output)

This example idles while waiting for the transfer to complete; alternatively it could
set an interrupt handler and return immediately.
