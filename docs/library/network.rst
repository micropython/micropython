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

.. only:: port_pyboard

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

class WLAN
==========

.. _network.WLAN:

.. only:: port_esp8266

    This class provides a driver for WiFi network processor in the ESP8266.  Example usage::

        import network
        # setup as a station
        nic = network.WLAN()
        nic.connect('your-ssid', 'your-password')
        # now use socket as usual

    Constructors
    ------------
    .. class:: WLAN()

    Create a WLAN driver object.

    Methods
    -------

    .. method:: wlan.connect(ssid, password)

        Connect to the specified wireless network, using the specified password.

    .. method:: wlan.disconnect()

        Disconnect from the currently connected wireless network.

    .. method:: wlan.scan(cb)

        Initiate scanning for the available wireless networks.

        Scanning is only possible if the radio is in station or station+AP mode; if
        called while in AP only mode, an OSError exception will be raised.

        Once the scanning is complete, the provided callback function ``cb`` will
        be called once for each network found, and passed a tuple with information
        about that network:

            (ssid, bssid, channel, RSSI, authmode, hidden)

        There are five values for authmode:

            * 0 -- open
            * 1 -- WEP
            * 2 -- WPA-PSK
            * 3 -- WPA2-PSK
            * 4 -- WPA/WPA2-PSK

        and two for hidden:

            * 0 -- visible
            * 1 -- hidden

.. only:: port_wipy

    This class provides a driver for WiFi network processor in the WiPy.  Example usage::
    
        import network
        # setup as a station
        nic = network.WLAN(mode=WLAN.STA)
        nic.connect('your-ssid', security=WLAN.WPA2, key='your-key')
        while not nic.isconnected():
            pyb.delay(50)
        print(nic.ifconfig())

        # now use socket as usual
        ...

    Constructors
    ------------
    
    .. class:: WLAN(..)

       Create a WLAN object, and optionally configure it. See ``iwconfig`` for params of configuration.

    Methods
    -------

    .. method:: iwconfig(\*, mode, ssid, security, key, channel, antenna)
    
       Set or get the WiFi network processor configuration.
    
       Arguments are:
    
         - ``mode`` can be either ``WLAN.STA`` or ``WLAN.AP``.
         - ``ssid`` is a string with the ssid name. Only needed when mode is ``WLAN.AP``.
         - ``security`` can be ``WLAN.OPEN``, ``WLAN.WEP``, ``WLAN.WPA`` or ``WLAN.WPA2``. 
           Only needed when mode is ``WLAN.AP``.
         - ``key`` is a string with the network password. Not needed when mode is ``WLAN.STA``
           or security is ``WLAN.OPEN``. If ``security`` is ``WLAN.WEP`` the key must be a
           string representing hexadecimal values (e.g. 'ABC1DE45BF').
         - ``channel`` a number in the range 1-11. Only needed when mode is ``WLAN.AP``.
         - ``antenna`` selects between the internal and the external antenna. Can be either
           ``WLAN.INTERNAL`` or ``WLAN.EXTERNAL``.
    
       For example, you can do::

          # create and configure as an access point
          nic.iwconfig(mode=WLAN.AP, ssid='wipy-wlan', security=WLAN.WPA2, key='www.wipy.io', channel=7, antenna=WLAN.INTERNAL)

       or::

          # configure as an station
          nic.iwconfig(mode=WLAN.STA)

       With no arguments given, the current configuration is returned as a namedtuple that looks like this:
       ``(mode=2, ssid='wipy-wlan', security=2, key='www.wipy.io', channel=5, antenna=0)``

    .. method:: wlan.connect(ssid, \*, security=WLAN.OPEN, key=None, bssid=None, timeout=5000)
    
       Connect to a wifi access point using the given SSID, and other security
       parameters.
          
          - ``key`` is always a string, but if ``security`` is ``WLAN.WEP`` the key must be a string
            representing hexadecimal values (e.g. 'ABC1DE45BF').
          - ``bssid`` is the MAC address of the AP to connect to. Useful when there are several APs
            with the same ssid.
          - ``timeout`` is the maximum time in milliseconds to wait for the connection to succeed.
    
    .. method:: wlan.scan()
    
       Performs a network scan and returns a list of named tuples with (ssid, bssid, security, channel, rssi).
       Note that channel is always ``None`` since this info is not provided by the WiPy.
    
    .. method:: wlan.disconnect()
    
       Disconnect from the wifi access point.
    
    .. method:: wlan.isconnected()
    
       In case of STA mode, returns ``True`` if connected to a wifi access point and has a valid IP address.
       In AP mode returns ``True`` when a station is connected. Returns ``False`` otherwise.
    
    .. method:: wlan.ifconfig(['dhcp' or configtuple])
    
       With no parameters given eturns a 4-tuple of ``(ip, subnet mask, gateway, DNS server)``.
       
       if ``'dhcp'`` is passed as a parameter then the DHCP client is enabled and the IP params
       are negotiated with the AP.
       
       if the 4-tuple config is given then a static IP is configured. For example::
    
          nic.ifconfig(('192.168.0.4', '255.255.255.0', '192.168.0.1', '8.8.8.8'))
    
    .. method:: wlan.mac()
    
       Returns a 6-byte long bytes object with the MAC address.

    .. method:: wlan.connections()
    
       Returns a list of the devices currently connected. Each item in the list is a
       tuple of ``(ssid, mac)``.

    .. method:: wlan.callback(wake_from)

        Create a callback to be triggered when a WLAN event occurs during ``pyb.Sleep.SUSPENDED``
        mode. Events are triggered by socket activity or by WLAN connection/disconnection.

            - ``wake_from`` must be ``pyb.Sleep.SUSPENDED``.

        Returns a callback object.

    Constants
    ---------
    
    .. data:: WLAN.STA

       WiFi station mode

    .. data:: WLAN.AP

       WiFi access point mode

    .. data:: WLAN.OPEN
    .. data:: WLAN.WEP
    .. data:: WLAN.WPA
    .. data:: WLAN.WPA2

       selects the network security

    .. data:: WLAN.INTERNAL
    .. data:: WLAN.EXTERNAL

       selects the antenna type
