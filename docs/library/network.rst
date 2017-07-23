****************************************
:mod:`network` --- network configuration
****************************************

.. module:: network
   :synopsis: network configuration

This module provides network drivers and routing configuration. To use this
module, a MicroPython variant/build with network capabilities must be installed.
Network drivers for specific hardware are available within this module and are
used to configure hardware network interface(s). Network services provided
by configured interfaces are then available for use via the :mod:`socket`
module.

For example::

    # connect/ show IP config a specific network interface
    # see below for examples of specific drivers
    import network
    import utime
    nic = network.Driver(...)
    if not nic.isconnected():
        nic.connect()
        print("Waiting for connection...")
        while not nic.isconnected():
            utime.sleep(1)
    print(nic.ifconfig())

    # now use usocket as usual
    import usocket as socket
    addr = socket.getaddrinfo('micropython.org', 80)[0][-1]
    s = socket.socket()
    s.connect(addr)
    s.send(b'GET / HTTP/1.1\r\nHost: micropython.org\r\n\r\n')
    data = s.recv(1000)
    s.close()

Common network adapter interface
================================

This section describes an (implied) abstract base class for all network
interface classes implemented by different ports of MicroPython for
different hardware. This means that MicroPython does not actually
provide `AbstractNIC` class, but any actual NIC class, as described
in the following sections, implements methods as described here.

.. class:: AbstractNIC(id=None, ...)

Instantiate a network interface object. Parameters are network interface
dependent. If there are more than one interface of the same type, the first
parameter should be `id`.

    .. method:: active([is_active])

        Activate ("up") or deactivate ("down") the network interface, if
        a boolean argument is passed. Otherwise, query current state if
        no argument is provided. Most other methods require an active
        interface (behavior of calling them on inactive interface is
        undefined).

    .. method:: connect([service_id, key=None, \*, ...])

       Connect the interface to a network. This method is optional, and
       available only for interfaces which are not "always connected".
       If no parameters are given, connect to the default (or the only)
       service. If a single parameter is given, it is the primary identifier
       of a service to connect to. It may be accompanied by a key
       (password) required to access said service. There can be further
       arbitrary keyword-only parameters, depending on the networking medium
       type and/or particular device. Parameters can be used to: a)
       specify alternative service identifer types; b) provide additional
       connection parameters. For various medium types, there are different
       sets of predefined/recommended parameters, among them:

       * WiFi: *bssid* keyword to connect by BSSID (MAC address) instead
         of access point name

    .. method:: disconnect()

       Disconnect from network.

    .. method:: isconnected()

       Returns ``True`` if connected to network, otherwise returns ``False``.

    .. method:: scan(\*, ...)

       Scan for the available network services/connections. Returns a
       list of tuples with discovered service parameters. For various
       network media, there are different variants of predefined/
       recommended tuple formats, among them:

       * WiFi: (ssid, bssid, channel, RSSI, authmode, hidden). There
         may be further fields, specific to a particular device.

       The function may accept additional keyword arguments to filter scan
       results (e.g. scan for a particular service, on a particular channel,
       for services of a particular set, etc.), and to affect scan
       duration and other parameters. Where possible, parameter names
       should match those in connect().

    .. method:: status()

       Return detailed status of the interface, values are dependent
       on the network medium/technology.

    .. method:: ifconfig([(ip, subnet, gateway, dns)])

       Get/set IP-level network interface parameters: IP address, subnet mask,
       gateway and DNS server. When called with no arguments, this method returns
       a 4-tuple with the above information. To set the above values, pass a
       4-tuple with the required information.  For example::

        nic.ifconfig(('192.168.0.4', '255.255.255.0', '192.168.0.1', '8.8.8.8'))

    .. method:: config('param')
                config(param=value, ...)

       Get or set general network interface parameters. These methods allow to work
       with additional parameters beyond standard IP configuration (as dealt with by
       `ifconfig()`). These include network-specific and hardware-specific
       parameters and status values. For setting parameters, the keyword argument
       syntax should be used, and multiple parameters can be set at once. For
       querying, a parameter name should be quoted as a string, and only one
       parameter can be queried at a time::

        # Set WiFi access point name (formally known as ESSID) and WiFi channel
        ap.config(essid='My AP', channel=11)
        # Query params one by one
        print(ap.config('essid'))
        print(ap.config('channel'))
        # Extended status information also available this way
        print(sta.config('rssi'))

.. only:: port_pyboard

    class CC3K
    ==========
    
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
    
    .. method:: cc3k.connect(ssid, key=None, \*, security=WPA2, bssid=None)
    
       Connect to a WiFi access point using the given SSID, and other security
       parameters.
    
    .. method:: cc3k.disconnect()
    
       Disconnect from the WiFi access point.
    
    .. method:: cc3k.isconnected()
    
       Returns True if connected to a WiFi access point and has a valid IP address,
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
    
         - *spi* is an :ref:`SPI object <pyb.SPI>` which is the SPI bus that the WIZnet5x00 is
           connected to (the MOSI, MISO and SCLK pins).
         - *pin_cs* is a :ref:`Pin object <pyb.Pin>` which is connected to the WIZnet5x00 nSS pin.
         - *pin_rst* is a :ref:`Pin object <pyb.Pin>` which is connected to the WIZnet5x00 nRESET pin.
    
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

.. _network.WLAN:

.. only:: port_esp8266

    Functions
    =========

    .. function:: phy_mode([mode])

        Get or set the PHY mode.

        If the *mode* parameter is provided, sets the mode to its value. If
        the function is called without parameters, returns the current mode.

        The possible modes are defined as constants:
            * ``MODE_11B`` -- IEEE 802.11b,
            * ``MODE_11G`` -- IEEE 802.11g,
            * ``MODE_11N`` -- IEEE 802.11n.

    class WLAN
    ==========

    This class provides a driver for WiFi network processor in the ESP8266.  Example usage::

        import network
        # enable station interface and connect to WiFi access point
        nic = network.WLAN(network.STA_IF)
        nic.active(True)
        nic.connect('your-ssid', 'your-password')
        # now use sockets as usual

    Constructors
    ------------
    .. class:: WLAN(interface_id)

    Create a WLAN network interface object. Supported interfaces are
    ``network.STA_IF`` (station aka client, connects to upstream WiFi access
    points) and ``network.AP_IF`` (access point, allows other WiFi clients to
    connect). Availability of the methods below depends on interface type.
    For example, only STA interface may `connect()` to an access point.

    Methods
    -------

    .. method:: wlan.active([is_active])

        Activate ("up") or deactivate ("down") network interface, if boolean
        argument is passed. Otherwise, query current state if no argument is
        provided. Most other methods require active interface.

    .. method:: wlan.connect(ssid, password)

        Connect to the specified wireless network, using the specified password.

    .. method:: wlan.disconnect()

        Disconnect from the currently connected wireless network.

    .. method:: wlan.scan()

        Scan for the available wireless networks.

        Scanning is only possible on STA interface. Returns list of tuples with
        the information about WiFi access points:

            (ssid, bssid, channel, RSSI, authmode, hidden)

        *bssid* is hardware address of an access point, in binary form, returned as
        bytes object. You can use `ubinascii.hexlify()` to convert it to ASCII form.

        There are five values for authmode:

            * 0 -- open
            * 1 -- WEP
            * 2 -- WPA-PSK
            * 3 -- WPA2-PSK
            * 4 -- WPA/WPA2-PSK

        and two for hidden:

            * 0 -- visible
            * 1 -- hidden

    .. method:: wlan.status()

        Return the current status of the wireless connection.

        The possible statuses are defined as constants:

            * ``STAT_IDLE`` -- no connection and no activity,
            * ``STAT_CONNECTING`` -- connecting in progress,
            * ``STAT_WRONG_PASSWORD`` -- failed due to incorrect password,
            * ``STAT_NO_AP_FOUND`` -- failed because no access point replied,
            * ``STAT_CONNECT_FAIL`` -- failed due to other problems,
            * ``STAT_GOT_IP`` -- connection successful.

    .. method:: wlan.isconnected()

        In case of STA mode, returns ``True`` if connected to a WiFi access
        point and has a valid IP address.  In AP mode returns ``True`` when a
        station is connected. Returns ``False`` otherwise.

    .. method:: wlan.ifconfig([(ip, subnet, gateway, dns)])

       Get/set IP-level network interface parameters: IP address, subnet mask,
       gateway and DNS server. When called with no arguments, this method returns
       a 4-tuple with the above information. To set the above values, pass a
       4-tuple with the required information.  For example::

        nic.ifconfig(('192.168.0.4', '255.255.255.0', '192.168.0.1', '8.8.8.8'))

    .. method:: wlan.config('param')
    .. method:: wlan.config(param=value, ...)

       Get or set general network interface parameters. These methods allow to work
       with additional parameters beyond standard IP configuration (as dealt with by
       `wlan.ifconfig()`). These include network-specific and hardware-specific
       parameters. For setting parameters, keyword argument syntax should be used,
       multiple parameters can be set at once. For querying, parameters name should
       be quoted as a string, and only one parameter can be queries at time::

        # Set WiFi access point name (formally known as ESSID) and WiFi channel
        ap.config(essid='My AP', channel=11)
        # Query params one by one
        print(ap.config('essid'))
        print(ap.config('channel'))

       Following are commonly supported parameters (availability of a specific parameter
       depends on network technology type, driver, and MicroPython port).

       =========  ===========
       Parameter  Description
       =========  ===========
       mac        MAC address (bytes)
       essid      WiFi access point name (string)
       channel    WiFi channel (integer)
       hidden     Whether ESSID is hidden (boolean)
       authmode   Authentication mode supported (enumeration, see module constants)
       password   Access password (string)
       =========  ===========



.. only:: port_wipy

    class WLAN
    ==========

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
    
    .. class:: WLAN(id=0, ...)

       Create a WLAN object, and optionally configure it. See `init()` for params of configuration.

    .. note::

       The ``WLAN`` constructor is special in the sense that if no arguments besides the id are given,
       it will return the already existing ``WLAN`` instance without re-configuring it. This is
       because ``WLAN`` is a system feature of the WiPy. If the already existing instance is not
       initialized it will do the same as the other constructors an will initialize it with default
       values.

    Methods
    -------

    .. method:: wlan.init(mode, \*, ssid, auth, channel, antenna)
    
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

    .. method:: wlan.connect(ssid, \*, auth=None, bssid=None, timeout=None)

       Connect to a WiFi access point using the given SSID, and other security
       parameters.

          - *auth* is a tuple with (sec, key). Security can be ``None``, ``WLAN.WEP``,
            ``WLAN.WPA`` or ``WLAN.WPA2``. The key is a string with the network password.
            If ``sec`` is ``WLAN.WEP`` the key must be a string representing hexadecimal
            values (e.g. 'ABC1DE45BF').
          - *bssid* is the MAC address of the AP to connect to. Useful when there are several
            APs with the same ssid.
          - *timeout* is the maximum time in milliseconds to wait for the connection to succeed.

    .. method:: wlan.scan()

       Performs a network scan and returns a list of named tuples with (ssid, bssid, sec, channel, rssi).
       Note that channel is always ``None`` since this info is not provided by the WiPy.

    .. method:: wlan.disconnect()

       Disconnect from the WiFi access point.

    .. method:: wlan.isconnected()

       In case of STA mode, returns ``True`` if connected to a WiFi access point and has a valid IP address.
       In AP mode returns ``True`` when a station is connected, ``False`` otherwise.

    .. method:: wlan.ifconfig(if_id=0, config=['dhcp' or configtuple])

       With no parameters given returns a 4-tuple of *(ip, subnet_mask, gateway, DNS_server)*.

       if ``'dhcp'`` is passed as a parameter then the DHCP client is enabled and the IP params
       are negotiated with the AP.

       If the 4-tuple config is given then a static IP is configured. For instance::

          wlan.ifconfig(config=('192.168.0.4', '255.255.255.0', '192.168.0.1', '8.8.8.8'))

    .. method:: wlan.mode([mode])

       Get or set the WLAN mode.

    .. method:: wlan.ssid([ssid])

       Get or set the SSID when in AP mode.

    .. method:: wlan.auth([auth])

       Get or set the authentication type when in AP mode.

    .. method:: wlan.channel([channel])

       Get or set the channel (only applicable in AP mode).

    .. method:: wlan.antenna([antenna])

       Get or set the antenna type (external or internal).

    .. method:: wlan.mac([mac_addr])

       Get or set a 6-byte long bytes object with the MAC address.

    .. method:: wlan.irq(\*, handler, wake)

        Create a callback to be triggered when a WLAN event occurs during ``machine.SLEEP``
        mode. Events are triggered by socket activity or by WLAN connection/disconnection.

            - *handler* is the function that gets called when the IRQ is triggered.
            - *wake* must be ``machine.SLEEP``.

        Returns an IRQ object.

    Constants
    ---------

    .. data:: WLAN.STA
    .. data:: WLAN.AP

       selects the WLAN mode

    .. data:: WLAN.WEP
    .. data:: WLAN.WPA
    .. data:: WLAN.WPA2

       selects the network security

    .. data:: WLAN.INT_ANT
    .. data:: WLAN.EXT_ANT

       selects the antenna type
