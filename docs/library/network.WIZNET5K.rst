.. currentmodule:: network
.. _network.WIZNET5K:

class WIZNET5K -- control WIZnet5x00 Ethernet modules
=====================================================

This class allows you to control WIZnet5x00 Ethernet adaptors based on
the W5200 and W5500 chipsets.  The particular chipset that is supported
by the firmware is selected at compile-time via the MICROPY_PY_NETWORK_WIZNET5K
option.

Example usage::

    import network
    nic = network.WIZNET5K(pyb.SPI(1), pyb.Pin.board.X5, pyb.Pin.board.X4)
    print(nic.ipconfig("addr4"))

    # now use socket as usual
    ...

For this example to work the WIZnet5x00 module must have the following connections:

    - MOSI connected to X8
    - MISO connected to X7
    - SCLK connected to X6
    - nSS connected to X5
    - nRESET connected to X4

It is possible to use other SPI buses and other pins for nSS and nRESET.

Constructors
------------

.. class:: WIZNET5K(spi, pin_cs, pin_rst)

   Create a WIZNET5K driver object, initialise the WIZnet5x00 module using the given
   SPI bus and pins, and return the WIZNET5K object.

   Arguments are:

     - *spi* is an :ref:`SPI object <pyb.SPI>` which is the SPI bus that the WIZnet5x00 is
       connected to (the MOSI, MISO and SCLK pins).
     - *pin_cs* is a :ref:`Pin object <pyb.Pin>` which is connected to the WIZnet5x00 nSS pin.
     - *pin_rst* is a :ref:`Pin object <pyb.Pin>` which is connected to the WIZnet5x00 nRESET pin.

   All of these objects will be initialised by the driver, so there is no need to
   initialise them yourself.  For example, you can use::

     nic = network.WIZNET5K(pyb.SPI(1), pyb.Pin.board.X5, pyb.Pin.board.X4)

Methods
-------

This class implements most methods from `AbstractNIC <AbstractNIC>`, which are documented there. Additional methods are:

.. method:: WIZNET5K.regs()

   Dump the WIZnet5x00 registers.  Useful for debugging.
