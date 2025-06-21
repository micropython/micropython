.. currentmodule:: network
.. _network.USB_NET:

class USB_NET -- USB NCM network interface
==========================================

This class provides a network interface over USB using the NCM (Network
Control Model) protocol.  The host computer sees this device as a USB
Ethernet adapter and assigns it an IP address via DHCP (served by the
MicroPython device).

.. note:: ``network.USB_NET`` requires a port with TinyUSB and NCM support
          enabled (``MICROPY_HW_NETWORK_USBNET``).  On stm32 this is
          automatically enabled when LWIP networking is enabled.  The USB
          NCM class must also be active - either compiled as the default or
          enabled in ``boot.py`` via
          ``machine.USBDevice.BUILTIN_FLAG_NCM``.

Example usage::

    import network

    nic = network.USB_NET()
    nic.active(True)
    # wait for USB host to configure the NCM interface
    while not nic.isconnected():
        pass

    print(nic.ipconfig("addr4"))

Constructors
------------

.. class:: USB_NET()

   Create and return a USB_NET object.  This initialises the NCM network
   interface if it has not already been initialised.  Only one instance
   exists (singleton).

Methods
-------

.. method:: USB_NET.active([is_active])

   Activate or deactivate the network interface.  Without argument returns
   current state as a bool.

.. method:: USB_NET.isconnected()

   Returns ``True`` if the USB host has configured the NCM interface,
   ``False`` otherwise.

.. method:: USB_NET.status()

   Returns the link status as an integer: ``1`` if the interface is up,
   ``0`` otherwise.

.. method:: USB_NET.ipconfig('param')
            USB_NET.ipconfig(param=value, ...)

   See `AbstractNIC.ipconfig`.

.. method:: USB_NET.ifconfig([(ip, subnet, gateway, dns)])

   See `AbstractNIC.ifconfig`.
