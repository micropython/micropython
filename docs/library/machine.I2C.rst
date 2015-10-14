.. _machine.I2C:

class I2C -- a two-wire serial protocol
=======================================

I2C is a two-wire protocol for communicating between devices.  At the physical
level it consists of 2 wires: SCL and SDA, the clock and data lines respectively.

I2C objects are created attached to a specific bus.  They can be initialised
when created, or initialised later on.

.. only:: port_wipy

    Example::

        from machine import I2C

        i2c = I2C(0)                         # create on bus 0
        i2c = I2C(0, I2C.MASTER)             # create and init as a master
        i2c.init(I2C.MASTER, baudrate=20000) # init as a master
        i2c.deinit()                         # turn off the peripheral

Printing the i2c object gives you information about its configuration.

.. only:: port_wipy

    A master must specify the recipient's address::

        i2c.init(I2C.MASTER)
        i2c.writeto(0x42, '123')        # send 3 bytes to slave with address 0x42
        i2c.writeto(addr=0x42, b'456')  # keyword for address

    Master also has other methods::

        i2c.scan()                          # scan for slaves on the bus, returning
                                            #   a list of valid addresses
        i2c.readfrom_mem(0x42, 2, 3)        # read 3 bytes from memory of slave 0x42,
                                            #   starting at address 2 in the slave
        i2c.writeto_mem(0x42, 2, 'abc')     # write 'abc' (3 bytes) to memory of slave 0x42
                                            # starting at address 2 in the slave, timeout after 1 second

Constructors
------------

.. only:: port_wipy

    .. class:: machine.I2C(bus, ...)

       Construct an I2C object on the given bus.  `bus` can only be 0.
       If the bus is not given, the default one will be selected (0).

Methods
-------

.. method:: i2c.deinit()

   Turn off the I2C bus.

.. only:: port_wipy

    .. method:: i2c.init(mode, \*, baudrate=100000, pins=(SDA, SCL))

      Initialise the I2C bus with the given parameters:

         - ``mode`` must be ``I2C.MASTER``
         - ``baudrate`` is the SCL clock rate
         - ``pins`` is an optional tuple with the pins to assign to the I2C bus.

    .. method:: i2c.readfrom(addr, nbytes)

        Read ``nbytes`` from the slave specified by ``addr``.
        Returns a ``bytes`` object with the data read.

    .. method:: i2c.readfrom_into(addr, buf)

        Read into ``buf`` from the slave specified by ``addr``.
        Returns the number of bytes read.

    .. method:: i2c.writeto(addr, buf, \*, stop=True)

        Write ``buf`` to the slave specified by ``addr``. Set ``stop`` to ``False``
        if the transfer should be continued.
        Returns the number of bytes written.

    .. method:: i2c.readfrom_mem(addr, memaddr, nbytes, \*, addrsize=8)

        Read ``nbytes`` from the slave specified by ``addr`` starting from the memory
        address specified by ``memaddr``.
        Param ``addrsize`` specifies the address size in bits.
        Returns a ``bytes`` object with the data read.

    .. method:: i2c.readfrom_mem_into(addr, memaddr, buf, \*, addrsize=8)

        Read into ``buf`` from the slave specified by ``addr`` starting from the memory
        address specified by ``memaddr``.
        Param ``addrsize`` specifies the address size in bits.
        Returns the number of bytes read.

    .. method:: i2c.writeto_mem(addr, memaddr, buf, \*, addrsize=8)

        Write ``buf`` to the slave specified by ``addr`` starting from the
        memory address specified by ``memaddr``. Param ``addrsize`` specifies the 
        address size in bits.
        Set ``stop`` to ``False`` if the transfer should be continued.
        Returns the number of bytes written.

.. method:: i2c.scan()

   Scan all I2C addresses from 0x01 to 0x7f and return a list of those that respond.
   Only valid when in master mode.

Constants
---------

.. data:: I2C.MASTER

   for initialising the bus to master mode
