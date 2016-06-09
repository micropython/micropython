.. currentmodule:: machine

class SPI -- a master-driven serial protocol
============================================

SPI is a serial protocol that is driven by a master.  At the physical level
there are 3 lines: SCK, MOSI, MISO.

.. only:: port_wipy

    See usage model of I2C; SPI is very similar.  Main difference is
    parameters to init the SPI bus::

        from machine import SPI
        spi = SPI(0, mode=SPI.MASTER, baudrate=1000000, polarity=0, phase=0, firstbit=SPI.MSB)

    Only required parameter is mode, must be SPI.MASTER.  Polarity can be 0 or 
    1, and is the level the idle clock line sits at.  Phase can be 0 or 1 to 
    sample data on the first or second clock edge respectively.

Constructors
------------

.. only:: port_wipy

    .. class:: SPI(id, ...)

       Construct an SPI object on the given bus.  ``id`` can be only 0.
       With no additional parameters, the SPI object is created but not
       initialised (it has the settings from the last initialisation of
       the bus, if any).  If extra arguments are given, the bus is initialised.
       See ``init`` for parameters of initialisation.

Methods
-------

.. method:: SPI.init(mode, baudrate=1000000, \*, polarity=0, phase=0, bits=8, firstbit=SPI.MSB, pins=(CLK, MOSI, MISO))

   Initialise the SPI bus with the given parameters:

     - ``mode`` must be ``SPI.MASTER``.
     - ``baudrate`` is the SCK clock rate.
     - ``polarity`` can be 0 or 1, and is the level the idle clock line sits at.
     - ``phase`` can be 0 or 1 to sample data on the first or second clock edge
       respectively.
     - ``bits`` is the width of each transfer, accepted values are 8, 16 and 32.
     - ``firstbit`` can be ``SPI.MSB`` only.
     - ``pins`` is an optional tupple with the pins to assign to the SPI bus.

.. method:: SPI.deinit()

   Turn off the SPI bus.

.. method:: SPI.write(buf)

    Write the data contained in ``buf``. 
    Returns the number of bytes written.

.. method:: SPI.read(nbytes, *, write=0x00)

    Read the ``nbytes`` while writing the data specified by ``write``.
    Return the number of bytes read.

.. method:: SPI.readinto(buf, *, write=0x00)

    Read into the buffer specified by ``buf`` while writing the data specified by
    ``write``.
    Return the number of bytes read.

.. method:: SPI.write_readinto(write_buf, read_buf)

    Write from ``write_buf`` and read into ``read_buf``. Both buffers must have the
    same length.
    Returns the number of bytes written

Constants
---------

.. data:: SPI.MASTER

   for initialising the SPI bus to master

.. data:: SPI.MSB

   set the first bit to be the most significant bit
