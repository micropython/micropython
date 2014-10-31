****************************************
:mod:`network` --- network configuration
****************************************

.. module:: network
   :synopsis: network configuration

This module provides network drivers and routing configuration.


class CC3k
==========

Constructors
------------

.. class:: CC3k(spi, pin_cs, pin_en, pin_irq)

   Initialise the CC3000 using the given SPI bus and pins and return a CC3k object.


Methods
-------

.. method:: cc3k.connect(ssid, key=None, \*, security=WPA2, bssid=None)


class WIZnet5k
==============

This class allows you to control WIZnet5x00 Ethernet adaptors based on
the W5200 and W5500 chipsets (only W5200 tested).

Example usage::

    import wiznet5k
    w = wiznet5k.WIZnet5k()
    print(w.ipaddr())
    w.gethostbyname('micropython.org')
    s = w.socket()
    s.connect(('192.168.0.2', 8080))
    s.send('hello')
    print(s.recv(10))


Constructors
------------

.. class:: WIZnet5k(spi, pin_cs, pin_rst)

   Create and return a WIZnet5k object.


Methods
-------

.. method:: wiznet5k.ipaddr([(ip, subnet, gateway, dns)])

   Get/set IP address, subnet mask, gateway and DNS.

.. method:: wiznet5k.regs()

   Dump WIZnet5k registers.
