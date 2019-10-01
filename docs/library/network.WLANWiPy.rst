.. currentmodule:: network
.. _network.WLANWiPy:

class WLANWiPy -- WiPy specific WiFi control
============================================

.. note::

    This class is a non-standard WLAN implementation for the WiPy.
    It is available simply as ``network.WLAN`` on the WiPy but is named in the
    documentation below as ``network.WLANWiPy`` to distinguish it from the
    more general :ref:`network.WLAN <network.WLAN>` class.

This class provides a driver for the WiFi network processor in the WiPy. Example usage::

    import network
    import time
    # setup as a station
    wlan = network.WLAN(mode=WLAN.STA)
    wlan.connect('your-ssid', auth=(WLAN.WPA2, 'your-key'))
    while not wlan.isconnected():
        time.sleep_ms(50)
    print(wlan.ifconfig())

    # now use socket as usual
    ...

Constructors
------------

.. class:: WLANWiPy(id=0, ...)

   Create a WLAN object, and optionally configure it. See `init()` for params of configuration.

.. note::

   The ``WLAN`` constructor is special in the sense that if no arguments besides the id are given,
   it will return the already existing ``WLAN`` instance without re-configuring it. This is
   because ``WLAN`` is a system feature of the WiPy. If the already existing instance is not
   initialized it will do the same as the other constructors an will initialize it with default
   values.

Methods
-------

.. method:: WLANWiPy.init(mode, \*, ssid, auth, channel, antenna)

   Set or get the WiFi network processor configuration.

   Arguments are:

     - *mode* can be either ``WLAN.STA`` or ``WLAN.AP``.
     - *ssid* is a string with the ssid name. Only needed when mode is ``WLAN.AP``.
     - *auth* is a tuple with (sec, key). Security can be ``None``, ``WLAN.WEP``,
       ``WLAN.WPA`` or ``WLAN.WPA2``. The key is a string with the network password.
       If ``sec`` is ``WLAN.WEP`` the key must be a string representing hexadecimal
       values (e.g. 'ABC1DE45BF'). Only needed when mode is ``WLAN.AP``.
     - *channel* a number in the range 1-11. Only needed when mode is ``WLAN.AP``.
     - *antenna* selects between the internal and the external antenna. Can be either
       ``WLAN.INT_ANT`` or ``WLAN.EXT_ANT``.

   For example, you can do::

      # create and configure as an access point
      wlan.init(mode=WLAN.AP, ssid='wipy-wlan', auth=(WLAN.WPA2,'www.wipy.io'), channel=7, antenna=WLAN.INT_ANT)

   or::

      # configure as an station
      wlan.init(mode=WLAN.STA)

.. method:: WLANWiPy.connect(ssid, \*, auth=None, bssid=None, timeout=None)

   Connect to a WiFi access point using the given SSID, and other security
   parameters.

      - *auth* is a tuple with (sec, key). Security can be ``None``, ``WLAN.WEP``,
        ``WLAN.WPA`` or ``WLAN.WPA2``. The key is a string with the network password.
        If ``sec`` is ``WLAN.WEP`` the key must be a string representing hexadecimal
        values (e.g. 'ABC1DE45BF').
      - *bssid* is the MAC address of the AP to connect to. Useful when there are several
        APs with the same ssid.
      - *timeout* is the maximum time in milliseconds to wait for the connection to succeed.

.. method:: WLANWiPy.scan()

   Performs a network scan and returns a list of named tuples with (ssid, bssid, sec, channel, rssi).
   Note that channel is always ``None`` since this info is not provided by the WiPy.

.. method:: WLANWiPy.disconnect()

   Disconnect from the WiFi access point.

.. method:: WLANWiPy.isconnected()

   In case of STA mode, returns ``True`` if connected to a WiFi access point and has a valid IP address.
   In AP mode returns ``True`` when a station is connected, ``False`` otherwise.

.. method:: WLANWiPy.ifconfig(if_id=0, config=['dhcp' or configtuple])

   With no parameters given returns a 4-tuple of *(ip, subnet_mask, gateway, DNS_server)*.

   if ``'dhcp'`` is passed as a parameter then the DHCP client is enabled and the IP params
   are negotiated with the AP.

   If the 4-tuple config is given then a static IP is configured. For instance::

      wlan.ifconfig(config=('192.168.0.4', '255.255.255.0', '192.168.0.1', '8.8.8.8'))

.. method:: WLANWiPy.mode([mode])

   Get or set the WLAN mode.

.. method:: WLANWiPy.ssid([ssid])

   Get or set the SSID when in AP mode.

.. method:: WLANWiPy.auth([auth])

   Get or set the authentication type when in AP mode.

.. method:: WLANWiPy.channel([channel])

   Get or set the channel (only applicable in AP mode).

.. method:: WLANWiPy.antenna([antenna])

   Get or set the antenna type (external or internal).

.. method:: WLANWiPy.mac([mac_addr])

   Get or set a 6-byte long bytes object with the MAC address.

.. method:: WLANWiPy.irq(\*, handler, wake)

    Create a callback to be triggered when a WLAN event occurs during ``machine.SLEEP``
    mode. Events are triggered by socket activity or by WLAN connection/disconnection.

        - *handler* is the function that gets called when the IRQ is triggered.
        - *wake* must be ``machine.SLEEP``.

    Returns an IRQ object.

Constants
---------

.. data:: WLANWiPy.STA
.. data:: WLANWiPy.AP

   selects the WLAN mode

.. data:: WLANWiPy.WEP
.. data:: WLANWiPy.WPA
.. data:: WLANWiPy.WPA2

   selects the network security

.. data:: WLANWiPy.INT_ANT
.. data:: WLANWiPy.EXT_ANT

   selects the antenna type
