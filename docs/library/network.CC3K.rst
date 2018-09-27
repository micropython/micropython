.. currentmodule:: network
.. _network.CC3K:

class CC3K -- control CC3000 WiFi modules
=========================================

This class provides a driver for CC3000 WiFi modules.  Example usage::

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

     - *spi* is an :ref:`SPI object <pyb.SPI>` which is the SPI bus that the CC3000 is
       connected to (the MOSI, MISO and CLK pins).
     - *pin_cs* is a :ref:`Pin object <pyb.Pin>` which is connected to the CC3000 CS pin.
     - *pin_en* is a :ref:`Pin object <pyb.Pin>` which is connected to the CC3000 VBEN pin.
     - *pin_irq* is a :ref:`Pin object <pyb.Pin>` which is connected to the CC3000 IRQ pin.

   All of these objects will be initialised by the driver, so there is no need to
   initialise them yourself.  For example, you can use::

     nic = network.CC3K(pyb.SPI(2), pyb.Pin.board.Y5, pyb.Pin.board.Y4, pyb.Pin.board.Y3)

Methods
-------

.. method:: CC3K.connect(ssid, key=None, \*, security=WPA2, bssid=None)

   Connect to a WiFi access point using the given SSID, and other security
   parameters.

.. method:: CC3K.disconnect()

   Disconnect from the WiFi access point.

.. method:: CC3K.isconnected()

   Returns True if connected to a WiFi access point and has a valid IP address,
   False otherwise.

.. method:: CC3K.ifconfig()

   Returns a 7-tuple with (ip, subnet mask, gateway, DNS server, DHCP server,
   MAC address, SSID).

.. method:: CC3K.patch_version()

   Return the version of the patch program (firmware) on the CC3000.

.. method:: CC3K.patch_program('pgm')

   Upload the current firmware to the CC3000.  You must pass 'pgm' as the first
   argument in order for the upload to proceed.

Constants
---------

.. data:: CC3K.WEP
.. data:: CC3K.WPA
.. data:: CC3K.WPA2

   security type to use
