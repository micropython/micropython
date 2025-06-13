.. currentmodule:: network
.. _network.WLAN:

class WLAN -- control built-in WiFi interfaces
==============================================

This class provides a driver for WiFi network processors.  Example usage::

    import network
    # enable station interface and connect to WiFi access point
    nic = network.WLAN(network.WLAN.IF_STA)
    nic.active(True)
    nic.connect('your-ssid', 'your-key')
    # now use sockets as usual

Constructors
------------
.. class:: WLAN(interface_id)

Create a WLAN network interface object. Supported interfaces are
``network.WLAN.IF_STA`` (station aka client, connects to upstream WiFi access
points) and ``network.WLAN.IF_AP`` (access point, allows other WiFi clients to
connect). Availability of the methods below depends on interface type.
For example, only STA interface may `WLAN.connect()` to an access point.

Methods
-------

.. method:: WLAN.active([is_active])

    Activate ("up") or deactivate ("down") network interface, if boolean
    argument is passed. Otherwise, query current state if no argument is
    provided. Most other methods require active interface.

.. method:: WLAN.connect(ssid=None, key=None, *, bssid=None, wpa3=False, param=value, ...)

    Connect to the specified wireless network, using the specified key.
    If *bssid* is given then the connection will be restricted to the
    access-point with that MAC address (the *ssid* must also be specified
    in this case). *wpa3* enforces WPA3 authentication and will reject the
    network if WPA3 is not supported.

    WPA Enterprise (ESP32 port only)

        * eap_method -- EAP method to use (string)

    Connect to the specified wireless network, using WPA-Enterprise authentication and
    the specified parameters. The EAP methods provided are EAP-PWD, EAP-PEAP,
    EAP-TTLS, and EAP-TLS. EAP-TLS is UNTESTED and thus EXPERIMENTAL.

    Common parameters:

        * ssid -- WiFi access point name, (string, e.g. "eduroam")

    EAP-PWD parameters

        * username -- your network username (string)
        * password -- your network password (string)

    EAP-PEAP parameters:

        * username -- your network username (string)
        * password -- your network password (string)
        * identity -- anonymous identity (string)
        * ca_cert -- the CA certificate (filename, string)

    EAP-TTLS parameters:

        * username -- your network username (string)
        * password -- your network password (string)
        * identity -- anonymous identity (string)
        * ca_cert -- the CA certificate (filename, string)
        * ttls_phase2_method -- TTLS Phase 2 method (integer)

    EAP-TTLS supports the following TTLS Phase 2 methods: 

        * 0 -- PWD
        * 1 -- MSCHAPv2 (default)
        * 2 -- MSCHAP
        * 3 -- PAP
        * 4 -- CHAP

    Please note that MSCHAPv2 and CHAP have known security issues and should be avoided.

    EAP-TLS parameters:

        * client_cert -- client certificate filename (string)
        * private_key -- private key filename (string)
        * private_key_password -- private key password (string, optional)
        * disable_time_check -- suppress the validity check for the local client certificate when using EAP-TLS (boolean, default False)

    disable_time_check is only included for the sake of completeness. In practice,
    you want to renew the client certificate before expiry.

    Certificate files need to be uploaded first, e.g.::

     mpremote cp <file> :

    EAP-PWD should be used whenever possible. It connects swiftly and uses the least resources.
    When using one of the other methods, make sure the system time is correct to prevent
    certificate validation errors. Best practice is to use a battery buffered RTC and to set
    the system time using NTP regularly. A temporary workaround if no battery buffered RTC is
    available is to set the system time to the image build time, like:

     import sys
     import machine
     (year, month, day) = sys.version.split(" on ")[1].split("-")
     rtc = machine.RTC()
     date_time = (int(year), int(month), int(day), 0, 0, 0, 0, 0)
     rtc.init(date_time)

.. method:: WLAN.disconnect()

    Disconnect from the currently connected wireless network.

.. method:: WLAN.scan()

    Scan for the available wireless networks.
    Hidden networks -- where the SSID is not broadcast -- will also be scanned
    if the WLAN interface allows it.

    Scanning is only possible on STA interface. Returns list of tuples with
    the information about WiFi access points:

        (ssid, bssid, channel, RSSI, security, hidden)

    *bssid* is hardware address of an access point, in binary form, returned as
    bytes object. You can use `binascii.hexlify()` to convert it to ASCII form.

    There are five values for security:

        * 0 -- open
        * 1 -- WEP
        * 2 -- WPA-PSK
        * 3 -- WPA2-PSK
        * 4 -- WPA/WPA2-PSK

    and two for hidden:

        * 0 -- visible
        * 1 -- hidden

.. method:: WLAN.status([param])

    Return the current status of the wireless connection.

    When called with no argument the return value describes the network link status.
    The possible statuses are defined as constants in the :mod:`network` module:

        * ``STAT_IDLE`` -- no connection and no activity,
        * ``STAT_CONNECTING`` -- connecting in progress,
        * ``STAT_WRONG_PASSWORD`` -- failed due to incorrect password,
        * ``STAT_NO_AP_FOUND`` -- failed because no access point replied,
        * ``STAT_CONNECT_FAIL`` -- failed due to other problems,
        * ``STAT_GOT_IP`` -- connection successful.

    When called with one argument *param* should be a string naming the status
    parameter to retrieve, and different parameters are supported depending on the
    mode the WiFi is in.

    In STA mode, passing ``'rssi'`` returns a signal strength indicator value, whose
    format varies depending on the port (this is available on all ports that support
    WiFi network interfaces, except for CC3200).

    In AP mode, passing ``'stations'`` returns a list of connected WiFi stations
    (this is available on all ports that support WiFi network interfaces, except for
    CC3200).  The format of the station information entries varies across ports,
    providing either the raw BSSID of the connected station, the IP address of the
    connected station, or both.

.. method:: WLAN.isconnected()

    In case of STA mode, returns ``True`` if connected to a WiFi access
    point and has a valid IP address.  In AP mode returns ``True`` when a
    station is connected. Returns ``False`` otherwise.

.. method:: WLAN.ifconfig([(ip, subnet, gateway, dns)])

   Get/set IP-level network interface parameters: IP address, subnet mask,
   gateway and DNS server. When called with no arguments, this method returns
   a 4-tuple with the above information. To set the above values, pass a
   4-tuple with the required information.  For example::

    nic.ifconfig(('192.168.0.4', '255.255.255.0', '192.168.0.1', '8.8.8.8'))

.. method:: WLAN.config('param')
            WLAN.config(param=value, ...)

   Get or set general network interface parameters. These methods allow to work
   with additional parameters beyond standard IP configuration (as dealt with by
   `AbstractNIC.ipconfig()`). These include network-specific and hardware-specific
   parameters. For setting parameters, keyword argument syntax should be used,
   multiple parameters can be set at once. For querying, parameters name should
   be quoted as a string, and only one parameter can be queries at time::

    # Set WiFi access point name (formally known as SSID) and WiFi channel
    ap.config(ssid='My AP', channel=11)
    # Query params one by one
    print(ap.config('ssid'))
    print(ap.config('channel'))

   Following are commonly supported parameters (availability of a specific parameter
   depends on network technology type, driver, and :term:`MicroPython port`).

   =============  ===========
   Parameter      Description
   =============  ===========
   mac            MAC address (bytes)
   ssid           WiFi access point name (string)
   channel        WiFi channel (integer). Depending on the port this may only be supported on the AP interface.
   hidden         Whether SSID is hidden (boolean)
   security       Security protocol supported (enumeration, see module constants)
   key            Access key (string)
   hostname       The hostname that will be sent to DHCP (STA interfaces) and mDNS (if supported, both STA and AP). (Deprecated, use :func:`network.hostname` instead)
   reconnects     Number of reconnect attempts to make (integer, 0=none, -1=unlimited)
   txpower        Maximum transmit power in dBm (integer or float)
   pm             WiFi Power Management setting (see below for allowed values)
   protocol       (ESP32 Only.) WiFi Low level 802.11 protocol. See `WLAN.PROTOCOL_DEFAULT`.
   bandwidth      (ESP32 Only.) WiFi channel bandwidth. See `WLAN.BANDWIDTH_20` and others.
   =============  ===========

<<<<<<< HEAD
CSI Methods (ESP32 only)
------------------------

.. note::
   These methods are only available on ESP32 builds with CSI support enabled.
   The standard generic ESP32, ESP32-C3, ESP32-C5, ESP32-C6, and ESP32-S3
   board definitions enable this in their default configuration. Other builds
   need ``CONFIG_ESP_WIFI_CSI_ENABLED=y`` in the ESP-IDF configuration.

   Channel State Information (CSI) provides per-packet physical layer channel data
   derived from received Wi-Fi frames. CSI capture requires an active Wi-Fi
   connection and incoming traffic to the device. Without traffic, no CSI frames
   will be captured.

   Other Espressif CSI options are hard-coded to defaults intended for connected
   station capture.

.. method:: WLAN.csi_enable(buffer_size=16)

   Enable CSI capture and allocate a circular buffer for received frames.

   The optional ``buffer_size`` argument sets the number of frames stored before
   new incoming frames are dropped. Larger values reduce drops at the cost of RAM. The
   exact maximum depends on the build, but it is limited by the underlying
   ringbuffer implementation to roughly 100 frames.

   Raises ``OSError`` if CSI cannot be enabled, for example if Wi-Fi is not
   active or the ESP-IDF rejects the configuration.

   Example::

      import network
      import time

      wlan = network.WLAN(network.WLAN.IF_STA)
      wlan.active(True)
      wlan.config(protocol=network.MODE_11B | network.MODE_11G | network.MODE_11N)
      wlan.config(pm=wlan.PM_NONE)
      wlan.connect("SSID", "password")

      while not wlan.isconnected():
          time.sleep_ms(100)

      wlan.csi_enable(buffer_size=32)

.. method:: WLAN.csi_disable()

   Disable CSI capture and clean up resources.

.. method:: WLAN.csi_read([result])

   Read a CSI frame from the buffer.

   **Returns:** A list containing CSI frame data, or ``None`` if no frames are
   available.

   If the optional ``result`` argument is provided, it must be a previous list
   returned by `WLAN.csi_read()`. The list will be updated in place and
   returned again. This reduces heap churn in busy read loops by reusing the
   existing list object and, when the captured frame fits, the existing CSI
   data ``bytearray``.

   **Frame list fields (in order):**

   * **0 - rssi** (int): Received signal strength in dBm
   * **1 - channel** (int): Wi-Fi channel number
   * **2 - mac** (bytes): Source MAC address (6 bytes)
   * **3 - timestamp** (int): Timestamp in microseconds
   * **4 - local_timestamp** (int): Local timestamp from Wi-Fi hardware
   * **5 - data** (bytearray): CSI raw data (I/Q components as int8_t values)
   * **6 - rate** (int): Data rate
   * **7 - sig_mode** (int): Signal mode (legacy, HT, VHT)
   * **8 - mcs** (int): Modulation and Coding Scheme index
   * **9 - cwb** (int): Channel bandwidth
   * **10 - smoothing** (int): Smoothing applied
   * **11 - not_sounding** (int): Not sounding frame
   * **12 - aggregation** (int): Aggregation
   * **13 - stbc** (int): STBC
   * **14 - fec_coding** (int): FEC coding
   * **15 - sgi** (int): Short GI
   * **16 - noise_floor** (int): Background noise level in dBm
   * **17 - ampdu_cnt** (int): AMPDU count
   * **18 - secondary_channel** (int): Secondary channel
   * **19 - ant** (int): Antenna
   * **20 - sig_len** (int): Signal length
   * **21 - rx_state** (int): RX state

   Some metadata fields may be ``0`` on targets where ESP-IDF does not provide
   the corresponding value in the public CSI receive structure.

.. method:: WLAN.csi_available()

   Get the number of CSI frames available in the buffer.

.. method:: WLAN.csi_dropped()

   Get the number of CSI frames dropped due to buffer overflow.
   Frames are dropped when the buffer is full and new frames arrive faster than
   they can be read. Increase ``buffer_size`` in ``csi_enable()`` to reduce
   drops.
=======
>>>>>>> bb242a6d2 (esp32/network-wlan: Added WPA-Enterprise.)


Constants
---------

.. data:: WLAN.PM_PERFORMANCE
        WLAN.PM_POWERSAVE
        WLAN.PM_NONE

    Allowed values for the ``WLAN.config(pm=...)`` network interface parameter:

        * ``PM_PERFORMANCE``: enable WiFi power management to balance power
          savings and WiFi performance
        * ``PM_POWERSAVE``: enable WiFi power management with additional power
          savings and reduced WiFi performance
        * ``PM_NONE``: disable wifi power management


ESP32 Protocol Constants
------------------------

The following ESP32-only constants relate to the ``WLAN.config(protocol=...)``
network interface parameter:

.. data:: WLAN.PROTOCOL_DEFAULT

      A bitmap representing all of the default 802.11 Wi-Fi modes supported by
      the chip. Consult `ESP-IDF Wi-Fi Protocols`_ documentation for details.

.. data:: WLAN.PROTOCOL_LR

      This value corresponds to the `Espressif proprietary "long-range" mode`_,
      which is not compatible with standard Wi-Fi devices. By setting this
      protocol it's possible for an ESP32 STA in long-range mode to connect to
      an ESP32 AP in long-range mode, or to use `ESP-NOW long range modes
      <espnow-long-range>`.

      This mode can be bitwise ORed with some standard 802.11 protocol bits
      (including `WLAN.PROTOCOL_DEFAULT`) in order to support a mix of standard
      Wi-Fi modes as well as LR mode, consult the `Espressif long-range
      documentation`_ for more details.

      Long range mode is not supported on ESP32-C2.

.. data:: WLAN.BANDWIDTH_20
        WLAN.BANDWIDTH_40
        WLAN.BANDWIDTH_80
        WLAN.BANDWIDTH_160
        WLAN.BANDWIDTH_80_80

      Allowed values for the ``WLAN.config(bandwidth=...)`` network interface parameter:

      * ``BANDWIDTH_20``: specifies a 20MHz wide WiFi channel when in STA and AP mode
      * ``BANDWIDTH_40``: specifies a 40MHz wide WiFi channel when in STA and AP mode
      * ``BANDWIDTH_80``: specifies a 80MHz wide WiFi channel when in AP mode, may not
        be available on all ESP32 models
      * ``BANDWIDTH_160``: specifies a 160MHz wide WiFi channel when in AP mode, may not
        be available on all ESP32 models
      * ``BANDWIDTH_80_80``: specifies a multi-antenna 80MHz + 80MHz wide WiFi channel
        setup when in AP mode, may not be available on all ESP32 models.

      When in STA mode, bandwidth can only be changed when the adapter is not connected to a
      network.  In AP mode it can be changed at any time.

.. _ESP-IDF Wi-Fi Protocols: https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/wifi.html#wi-fi-protocol-mode
.. _Espressif proprietary "long-range" mode:
.. _Espressif long-range documentation: https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/wifi.html#long-range-lr
