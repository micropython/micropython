.. currentmodule:: network
.. _network.USBD_NCM:

class USBD_NCM -- USB NCM network interface
==========================================

This class provides a network interface over USB using the NCM (Network
Control Model) protocol.  The host computer sees this device as a USB
Ethernet adapter and assigns it an IP address via DHCP (served by the
MicroPython device).

.. note:: ``network.USBD_NCM`` requires a port with TinyUSB and NCM support
          enabled (``MICROPY_PY_NETWORK_NCM``).  On stm32 this is
          automatically enabled when LWIP networking is enabled.  The USB
          NCM class must also be active - either compiled as the default or
          enabled in ``boot.py`` via
          ``machine.USBDevice.BUILTIN_NCM``.

Example usage::

    import network

    nic = network.USBD_NCM()
    nic.active(True)
    # wait for USB host to configure the NCM interface
    while not nic.isconnected():
        pass

    print(nic.ipconfig("addr4"))

Constructors
------------

.. class:: USBD_NCM()

   Create and return a USBD_NCM object.  This initialises the NCM network
   interface if it has not already been initialised.  Only one instance
   exists (singleton).

Methods
-------

.. method:: USBD_NCM.active([is_active])

   Activate or deactivate the network interface.  Without argument returns
   current state as a bool.

.. method:: USBD_NCM.isconnected()

   Returns ``True`` if the USB host has configured the NCM interface,
   ``False`` otherwise.

   When USB is disconnected, this returns ``False`` and network traffic
   stops. The interface remains registered with lwIP and can resume when
   the host reconnects and re-enumerates the device.

.. method:: USBD_NCM.status()

   Returns the link status as an integer: ``1`` if the interface is up,
   ``0`` otherwise.

.. method:: USBD_NCM.ipconfig('param')
            USBD_NCM.ipconfig(param=value, ...)

   See `AbstractNIC.ipconfig`.

.. method:: USBD_NCM.ifconfig([(ip, subnet, gateway, dns)])

   See `AbstractNIC.ifconfig`.

Notes
-----

**Link-local IP address:** The device IP (169.254.x.1) is derived
deterministically from the device MAC address. RFC 3927 ARP probe/announce
(conflict detection) is not implemented, so if two devices happen to derive
the same address on the same network segment, the conflict will go undetected.

**MAC address uniqueness:** The device and host-side MAC addresses are derived
from the value returned by ``mp_hal_get_mac()``. If two boards have the same
hardware MAC (e.g. the port does not use a hardware UID), they will present
the same network addresses and cause ARP conflicts.
