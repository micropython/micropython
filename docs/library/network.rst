****************************************
:mod:`network` --- network configuration
****************************************

.. module:: network
   :synopsis: network configuration

This module provides network drivers and routing configuration.  Network
drivers for specific hardware are available within this module and are
used to configure a hardware network interface.  Configured interfaces
are then available for use via the :mod:`socket` module.

For example::

    # configure a specific network interface
    # see below for examples of specific drivers
    import network
    nic = network.Driver(...)
    print(nic.ifconfig())

    # now use socket as usual
    import socket
    addr = socket.getaddrinfo('micropython.org', 80)[0][-1]
    s = socket.socket()
    s.connect(addr)
    s.send(b'GET / HTTP/1.1\r\nHost: micropython.org\r\n\r\n')
    data = s.recv(1000)
    s.close()

class CC3K
==========

This class provides a driver for CC3000 wifi modules.  Example usage::

    import network
    nic = network.CC3K(pyb.SPI(2), pyb.Pin.board.Y5, pyb.Pin.board.Y4, pyb.Pin.board.Y3)
    nic.connect('your-ssid', 'your-password')
    while not nic.isconnected():
        pyb.delay(50)
    print(nic.ifconfig())

    # now use socket as usual
    ...

For this example to work the CC3000 module must have the following connections:

    - MOSI connected to Y8
    - MISO connected to Y7
    - CLK connected to Y6
    - CS connected to Y5
    - VBEN connected to Y4
    - IRQ connected to Y3

It is possible to use other SPI busses and other pins for CS, VBEN and IRQ.

Constructors
------------

.. class:: CC3K(spi, pin_cs, pin_en, pin_irq)

   Create a CC3K driver object, initialise the CC3000 module using the given SPI bus
   and pins, and return the CC3K object.

   Arguments are:

     - ``spi`` is an :ref:`SPI object <pyb.SPI>` which is the SPI bus that the CC3000 is
       connected to (the MOSI, MISO and CLK pins).
     - ``pin_cs`` is a :ref:`Pin object <pyb.Pin>` which is connected to the CC3000 CS pin.
     - ``pin_en`` is a :ref:`Pin object <pyb.Pin>` which is connected to the CC3000 VBEN pin.
     - ``pin_irq`` is a :ref:`Pin object <pyb.Pin>` which is connected to the CC3000 IRQ pin.

   All of these objects will be initialised by the driver, so there is no need to
   initialise them yourself.  For example, you can use::

     nic = network.CC3K(pyb.SPI(2), pyb.Pin.board.Y5, pyb.Pin.board.Y4, pyb.Pin.board.Y3)

Methods
-------

.. method:: cc3k.connect(ssid, key=None, \*, security=WPA2, bssid=None)

   Connect to a wifi access point using the given SSID, and other security
   parameters.

.. method:: cc3k.disconnect()

   Disconnect from the wifi access point.

.. method:: cc3k.isconnected()

   Returns True if connected to a wifi access point and has a valid IP address,
   False otherwise.

.. method:: cc3k.ifconfig()

   Returns a 7-tuple with (ip, subnet mask, gateway, DNS server, DHCP server,
   MAC address, SSID).

.. method:: cc3k.patch_version()

   Return the version of the patch program (firmware) on the CC3000.

.. method:: cc3k.patch_program('pgm')

   Upload the current firmware to the CC3000.  You must pass 'pgm' as the first
   argument in order for the upload to proceed.

Constants
---------

.. data:: CC3K.WEP
.. data:: CC3K.WPA
.. data:: CC3K.WPA2

   security type to use

class WIZNET5K
==============

This class allows you to control WIZnet5x00 Ethernet adaptors based on
the W5200 and W5500 chipsets (only W5200 tested).

Example usage::

    import network
    nic = network.WIZNET5K(pyb.SPI(1), pyb.Pin.board.X5, pyb.Pin.board.X4)
    print(nic.ifconfig())

    # now use socket as usual
    ...

For this example to work the WIZnet5x00 module must have the following connections:

    - MOSI connected to X8
    - MISO connected to X7
    - SCLK connected to X6
    - nSS connected to X5
    - nRESET connected to X4

It is possible to use other SPI busses and other pins for nSS and nRESET.

Constructors
------------

.. class:: WIZNET5K(spi, pin_cs, pin_rst)

   Create a WIZNET5K driver object, initialise the WIZnet5x00 module using the given
   SPI bus and pins, and return the WIZNET5K object.

   Arguments are:

     - ``spi`` is an :ref:`SPI object <pyb.SPI>` which is the SPI bus that the WIZnet5x00 is
       connected to (the MOSI, MISO and SCLK pins).
     - ``pin_cs`` is a :ref:`Pin object <pyb.Pin>` which is connected to the WIZnet5x00 nSS pin.
     - ``pin_rst`` is a :ref:`Pin object <pyb.Pin>` which is connected to the WIZnet5x00 nRESET pin.

   All of these objects will be initialised by the driver, so there is no need to
   initialise them yourself.  For example, you can use::

     nic = network.WIZNET5K(pyb.SPI(1), pyb.Pin.board.X5, pyb.Pin.board.X4)

Methods
-------

.. method:: wiznet5k.ifconfig([(ip, subnet, gateway, dns)])

   Get/set IP address, subnet mask, gateway and DNS.

   When called with no arguments, this method returns a 4-tuple with the above information.

   To set the above values, pass a 4-tuple with the required information.  For example::

    nic.ifconfig(('192.168.0.4', '255.255.255.0', '192.168.0.1', '8.8.8.8'))

.. method:: wiznet5k.regs()

   Dump the WIZnet5x00 registers.  Useful for debugging.
