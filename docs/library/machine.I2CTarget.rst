.. currentmodule:: machine
.. _machine.I2CTarget:

class I2CTarget -- an I2C target device
=======================================

An I2C target is a device which connects to an I2C bus and is controlled by an
I2C controller.  I2C targets can take many forms.  The :class:`machine.I2CTarget`
class implements an I2C target that can be configured as a memory/register device,
or as an arbitrary I2C device by using callbacks (if supported by the port).

Example usage for the case of a memory device::

    from machine import I2CTarget

    # Create the backing memory for the I2C target.
    mem = bytearray(8)

    # Create an I2C target.  Depending on the port, extra parameters
    # may be required to select the peripheral and/or pins to use.
    i2c = I2CTarget(addr=67, mem=mem)

    # At this point an I2C controller can read and write `mem`.
    ...

    # Deinitialise the I2C target.
    i2c.deinit()

Note that some ports require an ``id``, and maybe ``scl`` and ``sda`` pins, to be
passed to the `I2CTarget` constructor, to select the hardware I2C instance and
pins that it connects to.

When configured as a memory device, it's also possible to register to receive events.
For example to be notified when the memory is read/written::

    from machine import I2CTarget

    # Define an IRQ handler, for I2C events.
    def irq_handler(i2c_target):
        flags = i2c_target.irq().flags()
        if flags & I2CTarget.IRQ_END_READ:
            print("controller read target at addr", i2c_target.memaddr)
        if flags & I2CTarget.IRQ_END_WRITE:
            print("controller wrote target at addr", i2c_target.memaddr)

    # Create the I2C target and register to receive default events.
    mem = bytearray(8)
    i2c = I2CTarget(addr=67, mem=mem)
    i2c.irq(irq_handler)

More complicated I2C devices can be implemented using the full set of events.  For
example, to see the raw events as they are triggered::

    from machine import I2CTarget

    # Define an IRQ handler that prints the event id and responds to reads/writes.
    def irq_handler(i2c_target, buf=bytearray(1)):
        flags = i2c_target.irq().flags()
        print(flags)
        if flags & I2CTarget.IRQ_READ_REQ:
            i2c_target.write(buf)
        if flags & I2CTarget.IRQ_WRITE_REQ:
            i2c_target.readinto(buf)

    # Create the I2C target and register to receive all events.
    i2c = I2CTarget(addr=67)
    all_triggers = (
        I2CTarget.IRQ_ADDR_MATCH_READ
        | I2CTarget.IRQ_ADDR_MATCH_WRITE
        | I2CTarget.IRQ_READ_REQ
        | I2CTarget.IRQ_WRITE_REQ
        | I2CTarget.IRQ_END_READ
        | I2CTarget.IRQ_END_WRITE
    )
    i2c.irq(irq_handler, trigger=all_triggers, hard=True)

Constructors
------------

.. class:: I2CTarget(id, addr, *, addrsize=7, mem=None, mem_addrsize=8, scl=None, sda=None)

   Construct and return a new I2CTarget object using the following parameters:

      - *id* identifies a particular I2C peripheral.  Allowed values depend on the
        particular port/board.  Some ports have a default in which case this parameter
        can be omitted.
      - *addr* is the I2C address of the target.
      - *addrsize* is the number of bits in the I2C target address.  Valid values
        are 7 and 10.
      - *mem* is an object with the buffer protocol that is writable.  If not
        specified then there is no backing memory and data must be read/written
        using the :meth:`I2CTarget.readinto` and :meth:`I2CTarget.write` methods.
      - *mem_addrsize* is the number of bits in the memory address.  Valid values
        are 0, 8, 16, 24 and 32.
      - *scl* is a pin object specifying the pin to use for SCL.
      - *sda* is a pin object specifying the pin to use for SDA.

   Note that some ports/boards will have default values of *scl* and *sda*
   that can be changed in this constructor.  Others will have fixed values
   of *scl* and *sda* that cannot be changed.

General Methods
---------------

.. method:: I2CTarget.deinit()

   Deinitialise the I2C target.  After this method is called the hardware will no
   longer respond to requests on the I2C bus, and no other methods can be called.

.. method:: I2CTarget.readinto(buf)

   Read into the given buffer any pending bytes written by the I2C controller.
   Returns the number of bytes read.

.. method:: I2CTarget.write(buf)

   Write out the bytes from the given buffer, to be passed to the I2C controller
   after it sends a read request.  Returns the number of bytes written.  Most ports
   only accept one byte at a time to this method.

.. method:: I2CTarget.irq(handler=None, trigger=IRQ_END_READ|IRQ_END_WRITE, hard=False)

   Configure an IRQ *handler* to be called when an event occurs.  The possible events are
   given by the following constants, which can be or'd together and passed to the *trigger*
   argument:

      - ``IRQ_ADDR_MATCH_READ`` indicates that the target was addressed by a
        controller for a read transaction.
      - ``IRQ_ADDR_MATCH_READ`` indicates that the target was addressed by a
        controller for a write transaction.
      - ``IRQ_READ_REQ`` indicates that the controller is requesting data, and this
        request must be satisfied by calling `I2CTarget.write` with the data to be
        passed back to the controller.
      - ``IRQ_WRITE_REQ`` indicates that the controller has written data, and the
        data must be read by calling `I2CTarget.readinto`.
      - ``IRQ_END_READ`` indicates that the controller has finished a read transaction.
      - ``IRQ_END_WRITE`` indicates that the controller has finished a write transaction.

   Not all triggers are available on all ports.  If a port has the constant then that
   event is available.

   Note the following restrictions:

      - ``IRQ_ADDR_MATCH_READ``, ``IRQ_ADDR_MATCH_READ``, ``IRQ_READ_REQ`` and
        ``IRQ_WRITE_REQ`` must be handled by a hard IRQ callback (with the *hard* argument
        set to ``True``).  This is because these events have very strict timing requirements
        and must usually be satisfied synchronously with the hardware event.

      - ``IRQ_END_READ`` and ``IRQ_END_WRITE`` may be handled by either a soft or hard
        IRQ callback (although note that all events must be registered with the same handler,
        so if any events need a hard callback then all events must be hard).

      - If a memory buffer has been supplied in the constructor then ``IRQ_END_WRITE``
        is not emitted for the transaction that writes the memory address.  This is to
        allow ``IRQ_END_READ`` and ``IRQ_END_WRITE`` to function correctly as soft IRQ
        callbacks, where the IRQ handler may be called quite some time after the actual
        hardware event.

.. attribute:: I2CTarget.memaddr

   The integer value of the most recent memory address that was selected by the I2C
   controller (only valid if ``mem`` was specified in the constructor).

Constants
---------

.. data:: I2CTarget.IRQ_ADDR_MATCH_READ
          I2CTarget.IRQ_ADDR_MATCH_WRITE
          I2CTarget.IRQ_READ_REQ
          I2CTarget.IRQ_WRITE_REQ
          I2CTarget.IRQ_END_READ
          I2CTarget.IRQ_END_WRITE

    IRQ trigger sources.
