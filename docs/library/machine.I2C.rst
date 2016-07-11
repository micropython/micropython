.. currentmodule:: machine

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

    .. class:: I2C(bus, ...)

       Construct an I2C object on the given bus.  `bus` can only be 0.
       If the bus is not given, the default one will be selected (0).

.. only:: port_esp8266

    .. class:: I2C(scl, sda, \*, freq=400000)

       Construct and return a new I2C object.
       See the init method below for a description of the arguments.

General Methods
---------------

.. only:: port_wipy

    .. method:: I2C.init(mode, \*, baudrate=100000, pins=(SDA, SCL))

      Initialise the I2C bus with the given parameters:

         - ``mode`` must be ``I2C.MASTER``
         - ``baudrate`` is the SCL clock rate
         - ``pins`` is an optional tuple with the pins to assign to the I2C bus.

.. only:: port_esp8266

    .. method:: I2C.init(scl, sda, \*, freq=400000)

      Initialise the I2C bus with the given arguments:

         - `scl` is a pin object for the SCL line
         - `sda` is a pin object for the SDA line
         - `freq` is the SCL clock rate

.. method:: I2C.deinit()

   Turn off the I2C bus.

   Availability: WiPy.

.. method:: I2C.scan()

   Scan all I2C addresses between 0x08 and 0x77 inclusive and return a list of
   those that respond.  A device responds if it pulls the SDA line low after
   its address (including a read bit) is sent on the bus.

   Note: on WiPy the I2C object must be in master mode for this method to be valid.

Primitive I2C operations
------------------------

The following methods implement the primitive I2C master bus operations and can
be combined to make any I2C transaction.  They are provided if you need more
control over the bus, otherwise the standard methods (see below) can be used.

.. method:: I2C.start()

   Send a start bit on the bus (SDA transitions to low while SCL is high).

   Availability: ESP8266.

.. method:: I2C.stop()

   Send a stop bit on the bus (SDA transitions to high while SCL is high).

   Availability: ESP8266.

.. method:: I2C.readinto(buf)

   Reads bytes from the bus and stores them into `buf`.  The number of bytes
   read is the length of `buf`.  An ACK will be sent on the bus after
   receiving all but the last byte, and a NACK will be sent following the last
   byte.

   Availability: ESP8266.

.. method:: I2C.write(buf)

   Write all the bytes from `buf` to the bus.  Checks that an ACK is received
   after each byte and raises an OSError if not.

   Availability: ESP8266.

Standard bus operations
-----------------------

The following methods implement the standard I2C master read and write
operations that target a given slave device.

.. method:: I2C.readfrom(addr, nbytes)

   Read `nbytes` from the slave specified by `addr`.
   Returns a `bytes` object with the data read.

.. method:: I2C.readfrom_into(addr, buf)

   Read into `buf` from the slave specified by `addr`.
   The number of bytes read will be the length of `buf`.

   On WiPy the return value is the number of bytes read.  Otherwise the
   return value is `None`.

.. method:: I2C.writeto(addr, buf, \*, stop=True)

   Write the bytes from `buf` to the slave specified by `addr`.

   The `stop` argument (only available on WiPy) tells if a stop bit should be
   sent at the end of the transfer.  If `False` the transfer should be
   continued later on.

   On WiPy the return value is the number of bytes written.  Otherwise the
   return value is `None`.

Memory operations
-----------------

Some I2C devices act as a memory device (or set of registers) that can be read
from and written to.  In this case there are two addresses associated with an
I2C transaction: the slave address and the memory address.  The following
methods are convenience functions to communicate with such devices.

.. method:: I2C.readfrom_mem(addr, memaddr, nbytes, \*, addrsize=8)

   Read `nbytes` from the slave specified by `addr` starting from the memory
   address specified by `memaddr`.
   The argument `addrsize` specifies the address size in bits (on ESP8266
   this argument is not recognised and the address size is always 8 bits).
   Returns a `bytes` object with the data read.

.. method:: I2C.readfrom_mem_into(addr, memaddr, buf, \*, addrsize=8)

   Read into `buf` from the slave specified by `addr` starting from the
   memory address specified by `memaddr`.  The number of bytes read is the
   length of `buf`.
   The argument `addrsize` specifies the address size in bits (on ESP8266
   this argument is not recognised and the address size is always 8 bits).

   On WiPy the return value is the number of bytes read.  Otherwise the
   return value is `None`.

.. method:: I2C.writeto_mem(addr, memaddr, buf, \*, addrsize=8)

   Write `buf` to the slave specified by `addr` starting from the
   memory address specified by `memaddr`.
   The argument `addrsize` specifies the address size in bits (on ESP8266
   this argument is not recognised and the address size is always 8 bits).

   On WiPy the return value is the number of bytes written.  Otherwise the
   return value is `None`.

Constants
---------

.. data:: I2C.MASTER

   for initialising the bus to master mode

   Availability: WiPy.
