.. currentmodule:: pyb
.. _pyb.SPI:

class SPI -- a master-driven serial protocol
============================================

SPI is a serial protocol that is driven by a master.  At the physical level
there are 3 lines: SCK, MOSI, MISO.

.. only:: port_pyboard

    See usage model of I2C; SPI is very similar.  Main difference is
    parameters to init the SPI bus::

        from pyb import SPI
        spi = SPI(1, SPI.MASTER, baudrate=600000, polarity=1, phase=0, crc=0x7)

    Only required parameter is mode, SPI.MASTER or SPI.SLAVE.  Polarity can be
    0 or 1, and is the level the idle clock line sits at.  Phase can be 0 or 1
    to sample data on the first or second clock edge respectively.  Crc can be
    None for no CRC, or a polynomial specifier.

    Additional methods for SPI::

        data = spi.send_recv(b'1234')        # send 4 bytes and receive 4 bytes
        buf = bytearray(4)
        spi.send_recv(b'1234', buf)          # send 4 bytes and receive 4 into buf
        spi.send_recv(buf, buf)              # send/recv 4 bytes from/to buf

Constructors
------------

.. only:: port_pyboard

    .. class:: pyb.SPI(bus, ...)

       Construct an SPI object on the given bus.  ``bus`` can be 1 or 2, or
       'X' or 'Y'. With no additional parameters, the SPI object is created but
       not initialised (it has the settings from the last initialisation of
       the bus, if any).  If extra arguments are given, the bus is initialised.
       See ``init`` for parameters of initialisation.

       The physical pins of the SPI busses are:

         - ``SPI(1)`` is on the X position: ``(NSS, SCK, MISO, MOSI) = (X5, X6, X7, X8) = (PA4, PA5, PA6, PA7)``
         - ``SPI(2)`` is on the Y position: ``(NSS, SCK, MISO, MOSI) = (Y5, Y6, Y7, Y8) = (PB12, PB13, PB14, PB15)``

       At the moment, the NSS pin is not used by the SPI driver and is free
       for other use.

Methods
-------

.. method:: SPI.deinit()

   Turn off the SPI bus.

.. only:: port_pyboard

    .. method:: SPI.init(mode, baudrate=328125, \*, prescaler, polarity=1, phase=0, bits=8, firstbit=SPI.MSB, ti=False, crc=None)

       Initialise the SPI bus with the given parameters:

         - ``mode`` must be either ``SPI.MASTER`` or ``SPI.SLAVE``.
         - ``baudrate`` is the SCK clock rate (only sensible for a master).
         - ``prescaler`` is the prescaler to use to derive SCK from the APB bus frequency;
           use of ``prescaler`` overrides ``baudrate``.
         - ``polarity`` can be 0 or 1, and is the level the idle clock line sits at.
         - ``phase`` can be 0 or 1 to sample data on the first or second clock edge
           respectively.
         - ``bits`` can be 8 or 16, and is the number of bits in each transferred word.
         - ``firstbit`` can be ``SPI.MSB`` or ``SPI.LSB``.
         - ``crc`` can be None for no CRC, or a polynomial specifier.

       Note that the SPI clock frequency will not always be the requested baudrate.
       The hardware only supports baudrates that are the APB bus frequency
       (see :meth:`pyb.freq`) divided by a prescaler, which can be 2, 4, 8, 16, 32,
       64, 128 or 256.  SPI(1) is on AHB2, and SPI(2) is on AHB1.  For precise
       control over the SPI clock frequency, specify ``prescaler`` instead of
       ``baudrate``.

       Printing the SPI object will show you the computed baudrate and the chosen
       prescaler.

.. only:: port_pyboard

    .. method:: SPI.recv(recv, \*, timeout=5000)
    
       Receive data on the bus:

         - ``recv`` can be an integer, which is the number of bytes to receive,
           or a mutable buffer, which will be filled with received bytes.
         - ``timeout`` is the timeout in milliseconds to wait for the receive.

       Return value: if ``recv`` is an integer then a new buffer of the bytes received,
       otherwise the same buffer that was passed in to ``recv``.
    
    .. method:: SPI.send(send, \*, timeout=5000)

       Send data on the bus:

         - ``send`` is the data to send (an integer to send, or a buffer object).
         - ``timeout`` is the timeout in milliseconds to wait for the send.

       Return value: ``None``.

    .. method:: SPI.send_recv(send, recv=None, \*, timeout=5000)
    
       Send and receive data on the bus at the same time:

         - ``send`` is the data to send (an integer to send, or a buffer object).
         - ``recv`` is a mutable buffer which will be filled with received bytes.
           It can be the same as ``send``, or omitted.  If omitted, a new buffer will
           be created.
         - ``timeout`` is the timeout in milliseconds to wait for the receive.

       Return value: the buffer with the received bytes.

Constants
---------

.. only:: port_pyboard

    .. data:: SPI.MASTER
    .. data:: SPI.SLAVE
    
       for initialising the SPI bus to master or slave mode
    
    .. data:: SPI.LSB
    .. data:: SPI.MSB
    
       set the first bit to be the least or most significant bit
