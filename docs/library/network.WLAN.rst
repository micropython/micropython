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

.. method:: WLAN.connect(ssid=None, key=None, *, bssid=None)

    Connect to the specified wireless network, using the specified key.
    If *bssid* is given then the connection will be restricted to the
    access-point with that MAC address (the *ssid* must also be specified
    in this case).

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
   protocol       (ESP32 Only.) WiFi Low level 802.11 protocol. See `WLAN.PROTOCOL_DEFAULTS`.
   =============  ===========

CSI Methods (ESP32 only)
------------------------

.. note::
   These methods are only available on ESP32 ports (ESP32, ESP32-S2, ESP32-S3,
   ESP32-C3, ESP32-C5, ESP32-C6) when CSI support is enabled in the firmware build.

Channel State Information (CSI) provides detailed information about the Wi-Fi channel
state by analyzing physical layer signals. CSI data contains amplitude and phase
information for each subcarrier, enabling applications such as motion detection,
indoor localization, and gesture recognition.

CSI capture requires an active Wi-Fi connection. Frames are captured from received
Wi-Fi packets and stored in a circular buffer for asynchronous reading.

.. method:: WLAN.csi_enable(buffer_size=16, lltf_en=True, htltf_en=True, stbc_htltf2_en=True, ltf_merge_en=True, channel_filter_en=True, manu_scale=False, shift=0)

   Enable CSI capture with the specified configuration.

   .. note::
      Parameter support varies by device. ESP32-C6 and ESP32-C5 use the Wi-Fi 6
      CSI API which automatically configures legacy CSI capture (equivalent to
      ``acquire_csi_legacy = 1``). On these devices, only ``buffer_size`` is
      configurable via Python; the other parameters (``lltf_en``, ``htltf_en``,
      etc.) are ignored. ESP32, ESP32-S2, ESP32-S3, and ESP32-C3 use the standard
      CSI API where all parameters are configurable. See the table below for details.

   .. note::
      **Wi-Fi Traffic Requirement**: CSI data is extracted from received Wi-Fi
      packets. For CSI capture to work, there must be active Wi-Fi traffic
      directed to the ESP32 device. Without traffic, no CSI frames will be
      captured. To generate test traffic, you can use ping from another device
      on the network (e.g., ``ping -i 0.1 <esp32_ip_address>``). More frequent
      packets generate more CSI data but increase computational load on the
      device.

   **Parameters:**

   * **buffer_size** (int, optional): Number of CSI frames to store in the circular
     buffer (1-1024). Each frame uses approximately 552 bytes of RAM. Default: 16.
     Larger buffers reduce frame drops but use more memory.
     **Supported on all ESP32 devices.**

   * **lltf_en** (bool, optional): Enable Legacy Long Training Field CSI capture.
     Default: True.
     **Only supported on ESP32, ESP32-S2, ESP32-S3, ESP32-C3.**

   * **htltf_en** (bool, optional): Enable HT Long Training Field CSI capture.
     Default: True.
     **Only supported on ESP32, ESP32-S2, ESP32-S3, ESP32-C3.**

   * **stbc_htltf2_en** (bool, optional): Enable STBC HT-LTF2 CSI capture.
     Default: True.
     **Only supported on ESP32, ESP32-S2, ESP32-S3, ESP32-C3.**

   * **ltf_merge_en** (bool, optional): Merge L-LTF and HT-LTF data by averaging.
     Default: True.
     **Only supported on ESP32, ESP32-S2, ESP32-S3, ESP32-C3.**

   * **channel_filter_en** (bool, optional): Enable channel filter to smooth adjacent
     subcarriers. Default: True.
     **Only supported on ESP32, ESP32-S2, ESP32-S3, ESP32-C3.**

   * **manu_scale** (bool, optional): Use manual scaling mode. Default: False
     (automatic scaling).
     **Only supported on ESP32, ESP32-S2, ESP32-S3, ESP32-C3.**

   * **shift** (int, optional): Shift value for manual scaling (0-15). Only used when
     ``manu_scale=True``. Default: 0.
     **Only supported on ESP32, ESP32-S2, ESP32-S3, ESP32-C3.**

   **Parameter Support by Device:**

   =====================  ==========  ==========  ==========  ==========  ==========  ==========
   Parameter              ESP32       ESP32-S2    ESP32-S3    ESP32-C3    ESP32-C5    ESP32-C6
   =====================  ==========  ==========  ==========  ==========  ==========  ==========
   buffer_size            ✓           ✓           ✓           ✓           ✓           ✓
   lltf_en                ✓           ✓           ✓           ✓           —           —
   htltf_en               ✓           ✓           ✓           ✓           —           —
   stbc_htltf2_en         ✓           ✓           ✓           ✓           —           —
   ltf_merge_en           ✓           ✓           ✓           ✓           —           —
   channel_filter_en      ✓           ✓           ✓           ✓           —           —
   manu_scale             ✓           ✓           ✓           ✓           —           —
   shift                  ✓           ✓           ✓           ✓           —           —
   =====================  ==========  ==========  ==========  ==========  ==========  ==========

   **Note:** ESP32-C5 and ESP32-C6 use the Wi-Fi 6 API which automatically enables
   legacy CSI capture (via ``acquire_csi_legacy = 1``) but does not expose the
   standard API parameters. These devices support legacy, HT20, and Wi-Fi 6 CSI
   capture through their internal configuration.

   **Note:** ESP32-C5 and ESP32-C6 use the Wi-Fi 6 (802.11ax) CSI API which has
   different internal configuration options. These devices automatically configure
   CSI capture with ``acquire_csi_legacy = 1`` (for legacy 802.11a/g packets),
   ``acquire_csi_ht20 = 1`` (for HT packets), and ``acquire_csi_su = 1`` (for
   Wi-Fi 6 HE packets). The Python configuration parameters (``lltf_en``,
   ``htltf_en``, etc.) are accepted but ignored on these devices, as they use a
   different API structure internally.

   **Raises:** ``OSError`` if CSI cannot be enabled (e.g., Wi-Fi not connected).

   **Example:** ::

      import network
      wlan = network.WLAN(network.STA_IF)
      wlan.active(True)
      wlan.connect("SSID", "password")
      
      # Enable CSI with custom buffer size
      wlan.csi_enable(buffer_size=64)

.. method:: WLAN.csi_disable()

   Disable CSI capture and clean up resources.

   **Example:** ::

      wlan.csi_disable()

.. method:: WLAN.csi_read()

   Read a CSI frame from the buffer.

   **Returns:** A dictionary containing CSI frame data, or ``None`` if no frames are
   available.

   **Frame dictionary fields:**

   * **rssi** (int): Received signal strength in dBm
   * **channel** (int): Wi-Fi channel number
   * **mac** (bytes): Source MAC address (6 bytes)
   * **timestamp** (int): Timestamp in microseconds
   * **local_timestamp** (int): Local timestamp from Wi-Fi hardware
   * **data** (bytearray): CSI raw data (I/Q components as int8_t values)
   * **rate** (int): Data rate
   * **mcs** (int): Modulation and Coding Scheme index
   * **sig_mode** (int): Signal mode (legacy, HT, VHT)
   * **cwb** (int): Channel bandwidth
   * **noise_floor** (int): Background noise level in dBm
   * Additional metadata fields (see implementation for full list)

   **Example:** ::

      frame = wlan.csi_read()
      if frame:
          print(f"RSSI: {frame['rssi']} dBm")
          print(f"Channel: {frame['channel']}")
          print(f"CSI samples: {len(frame['data'])}")
          print(f"MAC: {frame['mac'].hex()}")

.. method:: WLAN.csi_available()

   Get the number of CSI frames available in the buffer.

   **Returns:** Integer count of available frames.

   **Example:** ::

      available = wlan.csi_available()
      print(f"{available} frames ready to read")

.. method:: WLAN.csi_dropped()

   Get the number of CSI frames dropped due to buffer overflow.

   **Returns:** Integer count of dropped frames.

   **Note:** Frames are dropped when the buffer is full and new frames arrive faster
   than they can be read. Increase ``buffer_size`` in ``csi_enable()`` to reduce drops.

   **Example:** ::

      dropped = wlan.csi_dropped()
      if dropped > 0:
          print(f"Warning: {dropped} frames dropped")

**CSI Usage Example:** ::

   import network
   import time

   wlan = network.WLAN(network.STA_IF)
   wlan.active(True)
   wlan.connect("SSID", "password")

   # Wait for connection
   timeout = 30
   while not wlan.isconnected() and timeout > 0:
       time.sleep(1)
       timeout -= 1

   if not wlan.isconnected():
       raise Exception("WiFi connection failed")

   # Enable CSI capture
   wlan.csi_enable(buffer_size=32)

   # Read frames
   while True:
       frame = wlan.csi_read()
       if frame:
           print(f"RSSI: {frame['rssi']} dBm, "
                 f"Channel: {frame['channel']}, "
                 f"CSI length: {len(frame['data'])} bytes")
       else:
           time.sleep(0.1)

   # Disable when done
   wlan.csi_disable()

**Understanding CSI Data:**

CSI data is provided as I/Q (In-phase/Quadrature) components in the ``data`` field.
Each pair of values represents one subcarrier: ``[I0, Q0, I1, Q1, I2, Q2, ...]``.

**CSI Data Length:**

The ``data`` field is a bytearray that references an internal buffer of up to
512 bytes, but the actual length varies based on Wi-Fi mode and configuration.
The bytearray length (returned by ``len(frame['data'])``) reflects only the
valid data bytes, not the full buffer size. Always use ``len(frame['data'])``
to determine how many bytes are actually present in each frame.

Typical CSI data lengths:

* **HT20 (20 MHz bandwidth)**: 64 subcarriers = 128 bytes (most common)
  - Each subcarrier uses 2 bytes (I and Q components)
  - This is the default configuration and covers most use cases

* **HT40 (40 MHz bandwidth)**: 128 subcarriers = 256 bytes
  - Requires router/AP support for 40 MHz channels
  - Provides higher resolution but uses more bandwidth

* **Legacy 802.11a/g**: 64 subcarriers = 128 bytes
  - L-LTF (Legacy Long Training Field) data

* **Multi-antenna/MIMO**: May use additional bytes
  - Multiple spatial streams require more data
  - STBC (Space-Time Block Code) can increase data size

The 512-byte buffer accommodates all these cases, including future support for
HT40 and advanced MIMO configurations. For most applications, only the first
128 bytes (64 subcarriers) are needed.

To calculate amplitude for analysis::

   import math

   def csi_amplitude(csi_data):
       amplitudes = []
       # Process I/Q pairs (each subcarrier has I and Q components)
       for i in range(0, len(csi_data) - 1, 2):
           i_val = csi_data[i]      # In-phase component
           q_val = csi_data[i + 1] if i + 1 < len(csi_data) else 0  # Quadrature component
           # Mathematical formula: amplitude = √(I² + Q²)
           # This calculates the magnitude of the complex number (I, Q)
           # representing the channel response for this subcarrier
           amp = math.sqrt(i_val * i_val + q_val * q_val)
           amplitudes.append(amp)
       return amplitudes

   # Example: Get number of subcarriers
   frame = wlan.csi_read()
   if frame:
       csi_data = frame['data']
       # Mathematical formula:
       # Each subcarrier requires 2 bytes: one for I (In-phase) and one for Q (Quadrature)
       # Therefore: num_subcarriers = total_bytes ÷ 2
       # Example: 128 bytes ÷ 2 = 64 subcarriers (HT20)
       #          256 bytes ÷ 2 = 128 subcarriers (HT40)
       num_subcarriers = len(csi_data) // 2
       print(f"CSI data: {len(csi_data)} bytes, {num_subcarriers} subcarriers")

**Memory Usage:**

The circular buffer uses approximately ``buffer_size × 552 bytes`` of RAM.
For example, a buffer size of 64 frames uses about 35 KB of RAM.

**Platform Notes:**

* **ESP32-C6 / ESP32-C5**: Support Wi-Fi 6 (802.11ax) CSI capture.

* **ESP32 / ESP32-S2 / ESP32-S3 / ESP32-C3**: Support 802.11b/g/n CSI capture.

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

.. data:: WLAN.PROTOCOL_DEFAULTS

      A bitmap representing all of the default 802.11 Wi-Fi modes supported by
      the chip. Consult `ESP-IDF Wi-Fi Protocols`_ documentation for details.

.. data:: WLAN.PROTOCOL_LR

      This value corresponds to the `Espressif proprietary "long-range" mode`_,
      which is not compatible with standard Wi-Fi devices. By setting this
      protocol it's possible for an ESP32 STA in long-range mode to connect to
      an ESP32 AP in long-range mode, or to use `ESP-NOW long range modes
      <espnow-long-range>`.

      This mode can be bitwise ORed with some standard 802.11 protocol bits
      (including `WLAN.PROTOCOL_DEFAULTS`) in order to support a mix of standard
      Wi-Fi modes as well as LR mode, consult the `Espressif long-range
      documentation`_ for more details.

      Long range mode is not supported on ESP32-C2.

.. _ESP-IDF Wi-Fi Protocols: https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/wifi.html#wi-fi-protocol-mode
.. _Espressif proprietary "long-range" mode:
.. _Espressif long-range documentation: https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/wifi.html#long-range-lr
