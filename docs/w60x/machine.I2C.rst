.. currentmodule:: machine
.. _machine.I2C:

class I2C -- a two-wire serial protocol
=======================================

I2C is a two-wire protocol for communicating between devices.  At the physical
level it consists of 2 wires: SCL and SDA, the clock and data lines respectively.

I2C objects are created attached to a specific bus.  They can be initialised
when created, or initialised later on.

Printing the I2C object gives you information about its configuration.

Example usage::

	from machine import I2C
	
	i2c = I2C(115200,2,3)
	
	i2c.read(10)       # read 10 characters, returns a bytes object
    i2c.read()         # read all available characters
    i2c.readinto(buf)  # read and store into the given buffer
    i2c.write('abc')   # write the 3 characters
	
Constructors
------------

.. class:: I2C(freq,device_addr,reg_addr)

   Construct and return a new I2C object using the following parameters:
   
	- *freq* should be an integer which sets the maximum frequency
        for SCL.
	- *device_addr* should be an integer which sets the maximum device address.
	- *reg_addr* should be an integer which sets the maximum register address.
   
General Methods
---------------

.. method:: I2C.init(freq=115200)

  Initialise the I2C bus with the given arguments:

     - *freq* is the SCL clock rate
	 
Primitive I2C operations
------------------------

The following methods implement the primitive I2C master bus operations and can
be combined to make any I2C transaction.  They are provided if you need more
control over the bus, otherwise the standard methods (see below) can be used.

These methods are available on software I2C only.

.. method:: I2C.read(len)

    Reads bytes from the bus. The number of bytes read is the *len*. 
   
.. method:: I2C.readinto(buf,[len])

    Reads bytes from the bus and stores them into *buf*.  The number of bytes
    read is the *len*. If there is no *len*, the number of bytes is the length of *buf*.

.. method:: I2C.readline(buf,[len])

    Reads a line from the bus and stores them into *buf*. The number of bytes
    read is the *len*. If there is no *len*, the number of bytes is 16

.. method:: I2C.write(buf)

    Write the bytes from *buf* to the bus.
