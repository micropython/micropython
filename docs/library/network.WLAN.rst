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
   protocol       (ESP32 Only.) WiFi Low level 802.11 protocol. See `WLAN.PROTOCOL_DEFAULT` and protocol mode constants.
   bandwidth      (ESP32 Only.) WiFi channel bandwidth. See `WLAN.BW_HT20` and `WLAN.BW_HT40`.
   promiscuous    (ESP32 Only.) Enable promiscuous mode (boolean). Captures packets from all sources, not just connected AP.
   =============  ===========

CSI Methods (ESP32 only)
------------------------

.. note::
   These methods are only available on ESP32 ports when CSI support is enabled in the firmware build.

   **Pre-configured boards**: The ``ESP32_GENERIC_C6`` and ``ESP32_GENERIC_S3`` boards with ``CSI`` variant have
   CSI support pre-enabled and optimized for CSI applications. To use CSI on other
   boards, you need to enable ``CONFIG_ESP_WIFI_CSI_ENABLED=y`` in your build
   configuration (e.g., in ``sdkconfig.base`` or a custom ``sdkconfig.board`` file).

Channel State Information (CSI) provides detailed information about the Wi-Fi channel
state by analyzing physical layer signals. CSI data contains amplitude and phase
information for each subcarrier, enabling applications such as motion detection,
indoor localization, and gesture recognition.

CSI capture requires an active Wi-Fi connection. Frames are captured from received
Wi-Fi packets and stored in a circular buffer for asynchronous reading.

.. note::
   **Recommended WiFi Configuration**: For optimal CSI capture, configure WiFi
   settings after ``wlan.active(True)`` but before ``wlan.connect()``::

      wlan.config(protocol=network.MODE_11B | network.MODE_11G | network.MODE_11N)  # WiFi 4 (802.11n)
      wlan.config(bandwidth=wlan.BW_HT20)          # HT20 for stable CSI
      wlan.config(promiscuous=False)               # CSI from connected AP only
      wlan.config(pm=wlan.PM_NONE)                 # Disable power management

   - WiFi 4 (802.11b/g/n) provides consistent CSI behavior across all ESP32 variants
   - ``BW_HT20`` (20MHz) provides more stable CSI than ``BW_HT40`` (40MHz)
   - ``promiscuous=False`` captures CSI only from connected AP; set to ``True`` to capture from all packets
   - ``PM_NONE`` disables power management to avoid packet loss

.. method:: WLAN.csi_enable(buffer_size=16, legacy_ltf=False, ht_ltf=True, stbc=False, capture_ack=False, scale=None)

   Enable CSI capture with the specified configuration. The configuration is
   unified across all ESP32 devices, with parameters automatically mapped to the
   appropriate API for each device type.

   .. note::
      Only common parameters are exposed in the API. All other parameters use
      optimized defaults for motion detection. ESP32-C5 and ESP32-C6 use a newer
      ESP-IDF CSI API structure, while ESP32, ESP32-S2, ESP32-S3, and ESP32-C3
      use the legacy CSI API structure. The unified configuration automatically maps
      parameters to the correct API structure for each device.

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
     buffer (1-1024). Each frame uses approximately 540 bytes of RAM. Default: 16.
     Larger buffers reduce frame drops but use more memory.

   * **legacy_ltf** (bool, optional): Enable Legacy Long Training Field CSI capture.
     Default: False (HT-LTF only, optimized for motion detection).
     **Note:** On ESP32-C5/C6, legacy CSI is always enabled internally
     (``acquire_csi_legacy=1``) regardless of this setting for compatibility.

   * **ht_ltf** (bool, optional): Enable HT Long Training Field CSI capture
     (PRIMARY - best SNR). Default: True.
     **Maps to** ``acquire_csi_ht20`` on ESP32-C5/C6.

   * **stbc** (bool, optional): Enable STBC HT-LTF2 CSI capture (legacy devices) or
     HE-STBC CSI capture (ESP32-C6 only).
     Default: False (disabled for consistency).

   * **capture_ack** (bool, optional): Enable capture of 802.11 ACK frames.
     Default: False (adds noise, not useful).

   * **scale** (int or None, optional): CSI data scaling value. Default: None (automatic scaling).
     - ``None`` or ``0``: Automatic scaling (recommended)
     - ``1-15``: Manual scaling with the specified shift value
     **On ESP32/S2/S3/C3:** Maps to ``manu_scale=False`` (auto) or ``manu_scale=True`` with ``shift=scale`` (manual).
     **On ESP32-C5/C6:** Maps to ``val_scale_cfg=0`` (auto) or manual scaling via ``shift`` mapped to ``val_scale_cfg``.

   **Default Configuration (optimized for motion detection):**

   All other parameters use optimized defaults that are automatically configured
   based on the device:

   * **ESP32, ESP32-S2, ESP32-S3, ESP32-C3** (Legacy API):
     - ``ltf_merge_en=False`` (no merge, HT-LTF only)
     - ``channel_filter_en=False`` (raw subcarriers)
     - ``scale=None`` (maps to ``manu_scale=False``, auto-scaling)
     - ``shift=0`` (not used with auto-scaling)

   * **ESP32-C5, ESP32-C6** (Modern API):
     - ``acquire_csi_legacy=1`` (always enabled for compatibility)
     - ``acquire_csi_ht20=1`` (enabled via ``ht_ltf``)
     - ``acquire_csi_ht40=False`` (disabled)
     - ``acquire_csi_su=False`` (disabled)
     - ``acquire_csi_mu=False`` (disabled)
     - ``acquire_csi_dcm=False`` (disabled)
     - ``acquire_csi_beamformed=False`` (disabled)
     - ``val_scale_cfg=0`` (auto-scaling, mapped from ``scale=None``)

   **Parameter Support by Device:**

   =====================  ==========  ==========  ==========  ==========  ==========  ==========
   Parameter              ESP32       ESP32-S2    ESP32-S3    ESP32-C3    ESP32-C5    ESP32-C6
   =====================  ==========  ==========  ==========  ==========  ==========  ==========
   buffer_size            ✓           ✓           ✓           ✓           ✓           ✓
   legacy_ltf             ✓           ✓           ✓           ✓           ✓           ✓
   ht_ltf                 ✓           ✓           ✓           ✓           ✓           ✓
   stbc                   ✓           ✓           ✓           ✓           ✓           ✓
   capture_ack            ✓           ✓           ✓           ✓           ✓           ✓
   scale                  ✓           ✓           ✓           ✓           ✓           ✓
   csi_force_gain         ✗           ✗           ✓           ✓           ✓           ✓
   =====================  ==========  ==========  ==========  ==========  ==========  ==========

   **Note:** The configuration is unified across all devices. Only common parameters
   are exposed in the API. All other parameters use optimized defaults. Parameters are
   automatically mapped to the appropriate ESP-IDF CSI API structure for each device.

   **Raises:** ``OSError`` if CSI cannot be enabled (e.g., Wi-Fi not connected).

   **Example:** ::

      import network
      wlan = network.WLAN(network.WLAN.IF_STA)
      wlan.active(True)
      
      # Configure for consistent CSI across all devices (WiFi 4)
      wlan.config(protocol=network.MODE_11B | network.MODE_11G | network.MODE_11N)
      wlan.config(bandwidth=wlan.BW_HT20)
      wlan.config(pm=wlan.PM_NONE)
      
      wlan.connect("SSID", "password")
      
      # Enable CSI with default settings (auto-scaling)
      wlan.csi_enable(buffer_size=64)
      
      # Enable CSI with manual scaling (shift=4)
      wlan.csi_enable(buffer_size=64, scale=4)
      
      # Enable CSI with custom configuration
      wlan.csi_enable(buffer_size=32, ht_ltf=True, legacy_ltf=False, scale=None)

.. method:: WLAN.csi_disable()

   Disable CSI capture and clean up resources.

   **Example:** ::

      wlan.csi_disable()

.. method:: WLAN.csi_read()

   Read a CSI frame from the buffer.

   **Returns:** A tuple containing CSI frame data, or ``None`` if no frames are
   available.

   **Frame tuple fields (in order):**

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
   * **22 - agc_gain** (int): AGC (Automatic Gain Control) value, unsigned 0-255 (ESP32-S3/C3/C5/C6 only, 0 on other platforms)
   * **23 - fft_gain** (int): FFT scaling gain value, signed -128 to 127 (ESP32-S3/C3/C5/C6 only, 0 on other platforms)

   **Example:** ::

      frame = wlan.csi_read()
      if frame:
          print(f"RSSI: {frame[0]} dBm")              # rssi
          print(f"Channel: {frame[1]}")               # channel
          print(f"CSI samples: {len(frame[5])}")      # data
          print(f"MAC: {frame[2].hex()}")             # mac

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

.. method:: WLAN.csi_force_gain(agc, fft)

   Force AGC (Automatic Gain Control) and FFT gain to fixed values for stable CSI
   measurements. This eliminates amplitude variations caused by automatic gain control,
   which is essential for consistent motion detection.

   .. note::
      This method is only supported on ESP32-S3, ESP32-C3, ESP32-C5, and ESP32-C6.
      On ESP32 and ESP32-S2, it raises ``NotImplementedError``.

   **Parameters:**

   * **agc** (int or None): AGC gain value, unsigned 0-255, or ``None`` to re-enable automatic control
   * **fft** (int or None): FFT gain value, signed -128 to 127, or ``None`` to re-enable automatic control

   **Note:** Both parameters must be set together. Either provide both as integers,
   or both as ``None`` to re-enable automatic gain control.

   **Usage:**

   The recommended approach is to collect gain samples from the first ~300 packets
   after Wi-Fi connection, calculate the **median** (more robust than mean against outliers),
   and then lock the gain to those values::

      # Check if gain lock is supported
      if wlan.csi_gain_lock_supported():
          # Collect gain samples from first 300 packets
          import time
          agc_samples = []
          fft_samples = []
          while len(agc_samples) < 300:
              frame = wlan.csi_read()
              if frame:
                  agc_samples.append(frame[22])  # agc_gain (unsigned)
                  fft_samples.append(frame[23])  # fft_gain (signed)
              else:
                  time.sleep_ms(10)  # Wait for more frames

          # Calculate median (more robust than mean)
          agc_samples.sort()
          fft_samples.sort()
          median_agc = agc_samples[len(agc_samples) // 2]
          median_fft = fft_samples[len(fft_samples) // 2]

          wlan.csi_force_gain(median_agc, median_fft)
          print(f"Gain locked: AGC={median_agc}, FFT={median_fft}")

          # Re-enable automatic gain control (if needed)
          # wlan.csi_force_gain(None, None)

   **Raises:** ``NotImplementedError`` on ESP32 and ESP32-S2.

.. method:: WLAN.csi_gain_lock_supported()

   Check if gain lock is supported on the current platform.

   **Returns:** ``True`` if ``csi_force_gain()`` is supported, ``False`` otherwise.

   **Platform support:**

   * ESP32-S3, ESP32-C3, ESP32-C5, ESP32-C6: Supported
   * ESP32, ESP32-S2: Not supported (returns ``False``)

   **Example:** ::

      if wlan.csi_gain_lock_supported():
          print("Gain lock is supported on this device")
      else:
          print("Gain lock not available (ESP32 or ESP32-S2)")

**CSI Usage Example:** ::

   import network
   import time

   wlan = network.WLAN(network.WLAN.IF_STA)
   wlan.active(True)

   # Configure WiFi for optimal CSI capture (WiFi 4 for consistency)
   wlan.config(protocol=network.MODE_11B | network.MODE_11G | network.MODE_11N)
   wlan.config(bandwidth=wlan.BW_HT20)          # HT20 for stable CSI
   wlan.config(promiscuous=False)               # CSI from connected AP only
   wlan.config(pm=wlan.PM_NONE)                 # Disable power management

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
           print(f"RSSI: {frame[0]} dBm, "
                 f"Channel: {frame[1]}, "
                 f"CSI length: {len(frame[5])} bytes")
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
       csi_data = frame[5]  # data field is at index 5
       # Mathematical formula:
       # Each subcarrier requires 2 bytes: one for I (In-phase) and one for Q (Quadrature)
       # Therefore: num_subcarriers = total_bytes ÷ 2
       # Example: 128 bytes ÷ 2 = 64 subcarriers (HT20)
       #          256 bytes ÷ 2 = 128 subcarriers (HT40)
       num_subcarriers = len(csi_data) // 2
       print(f"CSI data: {len(csi_data)} bytes, {num_subcarriers} subcarriers")

**Memory Usage:**

The circular buffer uses approximately ``buffer_size × 540 bytes`` of RAM.
For example, a buffer size of 64 frames uses about 34 KB of RAM.

**Platform Notes:**

* **ESP32-C5, ESP32-C6**: Use newer ESP-IDF CSI API structure. Legacy CSI is always
  enabled internally for compatibility. Default configuration: legacy + HT20, auto-scaling,
  optimized for motion detection.

* **ESP32, ESP32-S2, ESP32-S3, ESP32-C3**: Use legacy ESP-IDF CSI API structure.
  Default configuration: HT-LTF only, auto-scaling, optimized for motion detection.

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

The following constants can be used to build a custom protocol bitmap
(available via ``network.MODE_*`` for backwards compatibility):

.. data:: network.MODE_11B

      IEEE 802.11b protocol bit.

.. data:: network.MODE_11G

      IEEE 802.11g protocol bit.

.. data:: network.MODE_11N

      IEEE 802.11n (WiFi 4) protocol bit.

.. data:: network.MODE_11AX

      IEEE 802.11ax (WiFi 6) protocol bit. Only available on ESP32-C5 and ESP32-C6.

Example of building a custom protocol bitmap::

   wlan.config(protocol=network.MODE_11B | network.MODE_11G | network.MODE_11N)

.. _ESP-IDF Wi-Fi Protocols: https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/wifi.html#wi-fi-protocol-mode
.. _Espressif proprietary "long-range" mode:
.. _Espressif long-range documentation: https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/wifi.html#long-range-lr


ESP32 Bandwidth Constants
-------------------------

The following ESP32-only constants relate to the ``WLAN.config(bandwidth=...)``
network interface parameter:

.. data:: WLAN.BW_HT20

      20MHz channel bandwidth (HT20). This is the recommended setting for CSI
      capture as it provides more stable measurements than HT40.

.. data:: WLAN.BW_HT40

      40MHz channel bandwidth (HT40). Provides higher throughput but may be
      less stable for CSI applications.
