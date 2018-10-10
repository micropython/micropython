.. currentmodule:: pyb
.. _pyb.I2C:

class I2C -- a two-wire serial protocol
=======================================

I2C is a two-wire protocol for communicating between devices.  At the physical
level it consists of 2 wires: SCL and SDA, the clock and data lines respectively.

I2C objects are created attached to a specific bus.  They can be initialised
when created, or initialised later on.

Example::

    from pyb import I2C

    i2c = I2C(1)                         # create on bus 1
    i2c = I2C(1, I2C.MASTER)             # create and init as a master
    i2c.init(I2C.MASTER, baudrate=20000) # init as a master
    i2c.init(I2C.SLAVE, addr=0x42)       # init as a slave with given address
    i2c.deinit()                         # turn off the peripheral

Printing the i2c object gives you information about its configuration.

The basic methods are send and recv::

    i2c.send('abc')      # send 3 bytes
    i2c.send(0x42)       # send a single byte, given by the number
    data = i2c.recv(3)   # receive 3 bytes

To receive inplace, first create a bytearray::

    data = bytearray(3)  # create a buffer
    i2c.recv(data)       # receive 3 bytes, writing them into data

You can specify a timeout (in ms)::

    i2c.send(b'123', timeout=2000)   # timeout after 2 seconds

A master must specify the recipient's address::

    i2c.init(I2C.MASTER)
    i2c.send('123', 0x42)        # send 3 bytes to slave with address 0x42
    i2c.send(b'456', addr=0x42)  # keyword for address

Master also has other methods::

    i2c.is_ready(0x42)           # check if slave 0x42 is ready
    i2c.scan()                   # scan for slaves on the bus, returning
                                 #   a list of valid addresses
    i2c.mem_read(3, 0x42, 2)     # read 3 bytes from memory of slave 0x42,
                                 #   starting at address 2 in the slave
    i2c.mem_write('abc', 0x42, 2, timeout=1000) # write 'abc' (3 bytes) to memory of slave 0x42
                                                # starting at address 2 in the slave, timeout after 1 second

Constructors
------------

.. class:: pyb.I2C(bus, ...)

   Construct an I2C object on the given bus.  ``bus`` can be 1 or 2, 'X' or
   'Y'. With no additional parameters, the I2C object is created but not
   initialised (it has the settings from the last initialisation of
   the bus, if any).  If extra arguments are given, the bus is initialised.
   See ``init`` for parameters of initialisation.

   The physical pins of the I2C busses on Pyboards V1.0 and V1.1 are:

     - ``I2C(1)`` is on the X position: ``(SCL, SDA) = (X9, X10) = (PB6, PB7)``
     - ``I2C(2)`` is on the Y position: ``(SCL, SDA) = (Y9, Y10) = (PB10, PB11)``

   On the Pyboard Lite:

     - ``I2C(1)`` is on the X position: ``(SCL, SDA) = (X9, X10) = (PB6, PB7)``
     - ``I2C(3)`` is on the Y position: ``(SCL, SDA) = (Y9, Y10) = (PA8, PB8)``

   Calling the constructor with 'X' or 'Y' enables portability between Pyboard
   types.

Methods
-------

.. method:: I2C.deinit()

   Turn off the I2C bus.

.. method:: I2C.init(mode, \*, addr=0x12, baudrate=400000, gencall=False, dma=False)

  Initialise the I2C bus with the given parameters:

     - ``mode`` must be either ``I2C.MASTER`` or ``I2C.SLAVE``
     - ``addr`` is the 7-bit address (only sensible for a slave)
     - ``baudrate`` is the SCL clock rate (only sensible for a master)
     - ``gencall`` is whether to support general call mode
     - ``dma`` is whether to allow the use of DMA for the I2C transfers (note
       that DMA transfers have more precise timing but currently do not handle bus
       errors properly)

.. method:: I2C.is_ready(addr)

   Check if an I2C device responds to the given address.  Only valid when in master mode.

.. method:: I2C.mem_read(data, addr, memaddr, \*, timeout=5000, addr_size=8)

   Read from the memory of an I2C device:

     - ``data`` can be an integer (number of bytes to read) or a buffer to read into
     - ``addr`` is the I2C device address
     - ``memaddr`` is the memory location within the I2C device
     - ``timeout`` is the timeout in milliseconds to wait for the read
     - ``addr_size`` selects width of memaddr: 8 or 16 bits

   Returns the read data.
   This is only valid in master mode.

.. method:: I2C.mem_write(data, addr, memaddr, \*, timeout=5000, addr_size=8)

   Write to the memory of an I2C device:

     - ``data`` can be an integer or a buffer to write from
     - ``addr`` is the I2C device address
     - ``memaddr`` is the memory location within the I2C device
     - ``timeout`` is the timeout in milliseconds to wait for the write
     - ``addr_size`` selects width of memaddr: 8 or 16 bits

   Returns ``None``.
   This is only valid in master mode.

.. method:: I2C.recv(recv, addr=0x00, \*, timeout=5000)

   Receive data on the bus:

     - ``recv`` can be an integer, which is the number of bytes to receive,
       or a mutable buffer, which will be filled with received bytes
     - ``addr`` is the address to receive from (only required in master mode)
     - ``timeout`` is the timeout in milliseconds to wait for the receive

   Return value: if ``recv`` is an integer then a new buffer of the bytes received,
   otherwise the same buffer that was passed in to ``recv``.

.. method:: I2C.send(send, addr=0x00, \*, timeout=5000)

   Send data on the bus:

     - ``send`` is the data to send (an integer to send, or a buffer object)
     - ``addr`` is the address to send to (only required in master mode)
     - ``timeout`` is the timeout in milliseconds to wait for the send

   Return value: ``None``.

.. method:: I2C.scan()

   Scan all I2C addresses from 0x01 to 0x7f and return a list of those that respond.
   Only valid when in master mode.

Constants
---------

.. data:: I2C.MASTER

   for initialising the bus to master mode

.. data:: I2C.SLAVE

   for initialising the bus to slave mode
