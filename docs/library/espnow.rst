:mod:`espnow` --- Support for the ESP-NOW protocol
==================================================

.. module:: espnow
    :synopsis: ESP-NOW wireless protocol support

This module provides an interface to the `ESP-NOW <https://www.espressif.com/
en/products/software/esp-now/overview>`_ protocol provided by Espressif on
ESP32 and ESP8266 devices (`API docs <https://docs.espressif.com/
projects/esp-idf/en/latest/api-reference/network/esp_now.html>`_).

Table of Contents:
------------------

    - `Introduction`_
    - `Configuration`_
    - `Sending and Receiving Data`_
    - `Receiving Data by Iteration - (ESP32 Only)`_
    - `Peer Management`_
    - `Exceptions`_
    - `Constants - (ESP32 Only)`_
    - `Wifi Signal Strength (RSSI) - (ESP32 Only)`_
    - `Supporting asyncio - (ESP32 Only)`_
    - `Stream IO interface - (deprecated)`_
    - `Broadcast and Multicast`_
    - `ESPNow and Wifi Operation`_
    - `ESPNow and Sleep Modes`_
    - `Notes on using on_recv callbacks`_

Introduction
------------

ESP-NOW is a connectionless wireless communication protocol supporting:

- Direct communication between up to 20 registered peers:

  - Without the need for a wireless access point (AP),

- Encrypted and unencrypted communication (up to 6 encrypted peers),

- Message sizes up to 250 bytes,

- Can operate alongside Wifi operation (:doc:`network.WLAN<network.WLAN>`) on
  ESP32 and ESP8266 devices.

It is especially useful for small IoT networks, latency sensitive or power
sensitive applications (such as battery operated devices) and for long-range
communication between devices (hundreds of metres).

This module also support tracking the Wifi signal strength (RSSI) of peer
devices.

.. note::
  This module is still under development and its classes, functions, methods
  and constants are subject to change. This module is not yet included in the
  main branch of micropython. Until such a time that the code is accepted into
  the micropython main branch, the following resources are available:
  `Source code
  <https://github.com/glenn20/micropython/tree/espnow-g20>`_ |
  `Pre-compiled images
  <https://github.com/glenn20/micropython-espnow-images>`_ |
  `Pull request (PR#6515)
  <https://github.com/micropython/micropython/pull/6515>`_


Load this module from the :doc:`esp<esp>` module. A simple example would be:

**Sender:** ::

    import network
    from esp import espnow

    # A WLAN interface must be active to send()/recv()
    w0 = network.WLAN(network.STA_IF)  # Or network.AP_IF
    w0.active(True)
    w0.disconnect()   # For ESP8266

    e = espnow.ESPNow()
    e.init()
    peer = b'\xbb\xbb\xbb\xbb\xbb\xbb'   # MAC address of peer's wifi interface
    e.add_peer(peer)

    e.send("Starting...")       # Send to all peers
    for i in range(100):
        e.send(peer, str(i)*20, True)
        e.send(b'end')

**Receiver:** ::

    import network
    from esp import espnow

    # A WLAN interface must be active to send()/recv()
    w0 = network.WLAN(network.STA_IF)
    w0.active(True)
    w0.disconnect()   # For ESP8266

    e = espnow.ESPNow()
    e.init()
    peer = b'\xaa\xaa\xaa\xaa\xaa\xaa'   # MAC address of peer's wifi interface
    e.add_peer(peer)

    while True:
        host, msg = e.irecv()     # Available on ESP32 and ESP8266
        if msg:             # msg == None if timeout in irecv()
            print(host, msg)
            if msg == b'end':
                break

class ESPNow
------------

Constructor
-----------

.. class:: ESPNow()

    Returns the singleton ESPNow object. As this is a singleton, all calls to
    `espnow.ESPNow()` return a reference to the same object.

    .. note::
      Some methods are available only on the ESP32 due to code size
      restrictions on the ESP8266 and differences in the Espressif API.

Configuration
-------------

.. method:: ESPNow.init()
            ESPNow.init([rx_buf[, timeout[, on_recv]]])    (ESP8266 only)

    Prepare the software and hardware for use of the ESPNow communication
    protocol, including:

    - initialise the ESPNow data structures,
    - allocate the recv data buffer,
    - invoke esp_now_init() and
    - register the send and recv callbacks.

    **ESP8266**: `config()` is unavailable on the ESP8266 due to code size
    restrictions, so values for the ``rxbuf``, ``timeout`` and ``on_recv``
    options may be provided as positional arguments to `init()` ( eg.
    ``init(1024, 5000, callback_fn)``). If any argument is set to ``None`` the
    option will be left at it's default value (see `config()`).

    .. data:: Returns:

        ``None``

.. method:: ESPNow.deinit()

    De-initialise the Espressif ESPNow software stack (esp_now_deinit()),
    disable callbacks and deallocate the recv data buffer.

    **Note**: `deinit()` will also deregister all peers which must be
    re-registered after `init()`.

    .. data:: Returns:

        ``None``

.. method:: ESPNow.config('param')
            ESPNow.config(param=value, ...)

    **ESP32 only:** Use `init([rx_buf],
    [timeout])<ESPNow.init()>` on the ESP8266.

    Get or set configuration values of the ESPNow interface. To get a value
    the parameter name should be quoted as a string, and just one parameter is
    queried at a time.  To set values use the keyword syntax, and one or more
    parameters can be set at a time.

    .. data:: Options:

        ``rxbuf``: *(default=516)* Get/set the size in bytes of the internal
        buffer used to store incoming ESPNow packet data. The default size is
        selected to fit two max-sized ESPNow packets (250 bytes) with
        associated mac_address (6 bytes) and a message byte count (1 byte)
        plus buffer overhead. Increase this if you expect to receive a lot of
        large packets or expect bursty incoming traffic.

        **Note:** The recv buffer is allocated by `ESPNow.init()`. Changing
        this value will have no effect until the next call of `ESPNow.init()`.

        ``timeout``: *(default=300,000)* Default read timeout (in
        milliseconds). The timeout can also be provided as arg to `recv()` and
        `irecv()`.

        ``rate``: Set the transmission speed for espnow packets. Must be set to
        a number from the allowed numeric values in `enum wifi_phy_rate_t
        <https://docs.espressif.com/projects/esp-idf/en/v4.4.1/esp32/
        api-reference/network/esp_wifi.html#_CPPv415wifi_phy_rate_t>`_.

        ``on_recv``: *(default=None)* Get/set a callback function to be called
        *as soon as possible* after a message has been received from another
        ESPNow device. The function will be called with the espnow object as an
        argument, eg::

          def recv_cb(e):
              print(e.irecv(0))
          e.config(on_recv=recv_cb)

        See `Notes on using on_recv callbacks`_ below for more information.

    .. data:: Returns:

        ``None``

    .. data:: Raises:

        - ``OSError(num, "ESP_ERR_ESPNOW_NOT_INIT")`` if not initialised.
        - ``ValueError()`` on invalid configuration options or values.

Sending and Receiving Data
--------------------------

A wifi interface (``network.STA_IF`` or ``network.AP_IF``) must be
`active()<network.WLAN.active>` before messages can be sent or received,
but it is not necessary to connect or configure the WLAN interface.
For example::

    import network

    w0 = network.WLAN(network.STA_IF)
    w0.active(True)
    w0.disconnect()    # For ESP8266

**Note:** The ESP8266 has a *feature* that causes it to automatically reconnect
to the last wifi Access Point when set `active(True)<network.WLAN.active>`
(even after reboot/reset). As noted below, this reduces the reliability of
receiving ESP-NOW messages. You can avoid this by calling
`disconnect()<network.WLAN.disconnect>` after
`active(True)<network.WLAN.active>`.

.. method:: ESPNow.send(mac, msg[, sync])
            ESPNow.send(msg)   (ESP32 only)

    Send the data contained in ``msg`` to the peer with given network ``mac``
    address. In the second form, ``mac=None`` and ``sync=True``. The peer must
    be registered with `ESPNow.add_peer()<ESPNow.add_peer()>` before the
    message can be sent.

    .. data:: Arguments:

      - ``mac``: byte string exactly 6 bytes long or ``None``. If ``mac`` is
        ``None`` (ESP32 only) the message will be sent to all registered peers,
        except any broadcast or multicast MAC addresses.

      - ``msg``: string or byte-string up to ``ESPNow.MAX_DATA_LEN`` (250)
        bytes long.

      - ``sync``:

        - ``True``: (default) send ``msg`` to the peer(s) and wait for a
          response (or not).

        - ``False`` send ``msg`` and return immediately. Responses from the
          peers will be discarded.

    .. data:: Returns:

      ``True`` if ``sync=False`` or if ``sync=True`` and *all* peers respond,
      else ``False``.

    .. data:: Raises:

      - ``OSError(num, "ESP_ERR_ESPNOW_NOT_INIT")`` if not initialised.
      - ``OSError(num, "ESP_ERR_ESPNOW_NOT_FOUND")`` if peer is not registered.
      - ``OSError(num, "ESP_ERR_ESPNOW_IF")`` the wifi interface is not
        `active()<network.WLAN.active>`.
      - ``OSError(num, "ESP_ERR_ESPNOW_NO_MEM")`` internal ESP-NOW buffers are
        full.
      - ``ValueError()`` on invalid values for the parameters.

    **Note**: A peer will respond with success if its wifi interface is
    `active()<network.WLAN.active>` and set to the same channel as the sender,
    regardless of whether it has initialised it's ESP-Now system or is
    actively listening for ESP-Now traffic (see the Espressif ESP-Now docs).

.. method:: ESPNow.irecv([timeout])

    Wait for an incoming message and return the MAC address of the sender and
    the message. **Note**: It is **not** necessary to register a peer (using
    `add_peer()<ESPNow.add_peer()>`) to receive a message from that peer.

    .. data:: Arguments:

        ``timeout`` optionally sets a timeout (in milliseconds) for the read.
        The default timeout (5 minutes) can be set using `ESPNow.config()` on
        the ESP32 or `ESPNow.init()` on the ESP8266. If ``timeout`` is less than
        zero, then wait forever.

    .. data:: Returns:

      - ``(None, None)`` if ``timeout`` is reached before a message is
        received, or

      - ``(mac, msg)``: a `callee-owned tuple`, where:

        - ``mac`` is the MAC address of the sending device (peer) and

        - ``msg`` is the message/data sent from the peer.

      **Note**: Memory will be allocated once for the tuple and ``msg``
      byte string on invocation of `espnow.ESPNow()<ESPNow()>` and reused for
      subsequent calls to `irecv()<ESPNow.irecv()>`. You must make a copy of
      ``msg`` if you wish to retain the value across calls to ``irecv()``.
      This greatly reduces memory fragmentation compared to using
      `recv()<ESPNow.recv()>`.

      The ``mac`` values are always references to the ``peer`` addresses in
      the **peer device table** (see `ESPNow.peers`).

    .. data:: Raises:

      - ``OSError(num, "ESP_ERR_ESPNOW_NOT_INIT")`` if not initialised.
      - ``OSError(num, "ESP_ERR_ESPNOW_IF")`` the wifi interface is not
        `active()<network.WLAN.active>`.
      - ``ValueError()`` on invalid ``timeout`` values.

.. method:: ESPNow.recv([timeout]) (ESP32 only)

    **ESP32 only:** Use `irecv()` on the esp8266.

    As for `irecv()`, except that `recv()` will return a newly allocated tuple
    of byte strings.

    .. data:: Returns:

      - ``(mac, message)``: a newly allocated tuple of `bytes` where:

        - ``mac`` is the mac address of the sending device (peer) and

        - ``msg`` is the message/data sent from the peer.

      **Note**: repeatedly calling `recv()<ESPNow.recv()>` will exercise the
      micropython memory allocation as new storage is allocated for each new
      message and tuple. Use `irecv()<ESPNow.irecv()>` for a more
      memory-efficient option.

      The ``mac`` values are always references to the ``peer`` addresses in
      the **peer device table** (see `ESPNow.peers()`).

.. method:: ESPNow.poll() (ESP32 only)

    .. data:: Returns:

       ``True`` if data is available to be read with ``recv()/irecv()``, else
       ``False``.

.. method:: ESPNow.stats() (ESP32 only)

    .. data:: Returns:

      A 5-tuple containing the number of packets sent/received/lost:

      ``(tx_pkts, tx_responses, tx_failures, rx_packets, dropped_rx_packets)``

    Incoming packets are *dropped* when the recv buffers are full. To reduce
    packet loss, increase the ``rxbuf`` config parameters and ensure you are
    calling `irecv()<ESPNow.irecv()>` as quickly as possible.

    **Note**: Dropped packets will still be acknowledged to the sender as
    received.

Receiving Data by Iteration - (ESP32 Only)
------------------------------------------

It is also possible to read messages by iterating over the ESPNow singleton
object. This will yield ``(mac, message)`` tuples using the alloc-free
`irecv()` method, eg::

    for mac, msg in e:
        print(f"Recv: mac={mac}, message={msg}")
        if msg == b"!halt":
            e.deinit()

The iteration will continue while ``e`` remains initialised. That is, if you
call `deinit()<ESPNow.deinit()>` (or fail to call `init()<ESPNow.init()>`
before entering the loop), a ``StopIteration`` exception will be raised and
the for loop will exit.

**Note**: Iteration will yield ``(None, None)`` if the default timeout expires
while waiting for a message.

Peer Management
---------------

The Espressif ESP-Now software requires that other devices (peers) must be
*registered* before we can `send()<ESPNow.send()>` them messages. It is
**not** necessary to *register* a peer to receive a message from that peer.

.. method:: ESPNow.set_pmk(pmk)

    Set the Primary Master Key (PMK) which is used to encrypt the Local Master
    Keys (LMK) for encrypting ESPNow data traffic. If this is not set, a
    default PMK is used by the underlying Espressif esp_now software stack.

    **Note:** messages will only be encrypted if ``lmk`` is also set in
    `ESPNow.add_peer()` (see `Security
    <https://docs.espressif.com/projects/esp-idf/en/latest/
    esp32/api-reference/network/esp_now.html#security>`_ in the Espressif API
    docs).

    .. data:: Arguments:

      ``pmk``: Must be a byte string of length `espnow.KEY_LEN` (16
      bytes).

    .. data:: Returns:

      ``None``

    .. data:: Raises:

      ``ValueError()`` on invalid ``pmk`` values.

.. method:: ESPNow.add_peer(mac, [lmk], [channel], [ifidx], [encrypt])
            ESPNow.add_peer(mac, param=value, ...)   (ESP32 only)

    Add/register the provided ``mac`` address as a peer. Additional parameters
    may also be specified as positional or keyword arguments:

    .. data:: Arguments:

        - ``mac``: The MAC address of the peer (as a 6-byte byte-string).

        - ``lmk``: The Local Master Key (LMK) key used to encrypt data
          transfers with this peer (unless the *encrypt* parameter is set to
          *False*). Must be:

          - a byte-string of length ``espnow.KEY_LEN`` (16 bytes), or

          - any non ``True`` python value (default= ``b''``), signifying an
            *empty* key which will disable encryption.

        - ``channel``: The wifi channel (2.4GHz) to communicate with this peer.
          Must be an integer from 0 to 14. If channel is set to 0 the current
          channel of the wifi device will be used. (default=0)

        - ``ifidx``: *(ESP32 only)* Index of the wifi interface which will be
          used to send data to this peer. Must be an integer set to
          ``network.STA_IF`` (=0) or ``network.AP_IF`` (=1).
          (default=0/``network.STA_IF``). See `ESPNow and Wifi Operation`_
          below for more information.

        - ``encrypt``: *(ESP32 only)* If set to ``True`` data exchanged with
          this peer will be encrypted with the PMK and LMK. (default =
          ``False``)

        **ESP8266**: Keyword args may not be used on the ESP8266.

    .. data:: Raises:

        - ``OSError(num, "ESP_ERR_ESPNOW_NOT_INIT")`` if not initialised.
        - ``OSError(num, "ESP_ERR_ESPNOW_EXIST")`` if ``mac`` is already
          registered.
        - ``OSError(num, "ESP_ERR_ESPNOW_FULL")`` if too many peers are
          already registered.
        - ``ValueError()`` on invalid keyword args or values.

.. method:: ESPNow.del_peer(mac)

    Deregister the peer associated with the provided ``mac`` address.

    .. data:: Returns:

        ``None``

    .. data:: Raises:

        - ``OSError(num, "ESP_ERR_ESPNOW_NOT_INIT")`` if not initialised.
        - ``OSError(num, "ESP_ERR_ESPNOW_NOT_FOUND")`` if ``mac`` is not
          registered.
        - ``ValueError()`` on invalid ``mac`` values.

.. method:: ESPNow.get_peer(mac) (ESP32 only)

    Return information on a registered peer.

    .. data:: Returns:

        ``(mac, lmk, channel, ifidx, encrypt)``: a tuple of the "peer
        info" associated with the ``mac`` address.

    .. data:: Raises:

        - ``OSError(num, "ESP_ERR_ESPNOW_NOT_INIT")`` if not initialised.
        - ``OSError(num, "ESP_ERR_ESPNOW_NOT_FOUND")`` if ``mac`` is not
          registered.
        - ``ValueError()`` on invalid ``mac`` values.

.. method:: ESPNow.peer_count() (ESP32 only)

    Return the number of registered peers:

    - ``(peer_num, encrypt_num)``: where

      - ``peer_num`` is the number of peers which are registered, and
      - ``encrypt_num`` is the number of encrypted peers.

.. method:: ESPNow.get_peers() (ESP32 only)

    Return the "peer info" parameters for all the registered peers (as a tuple
    of tuples).

.. method:: ESPNow.mod_peer(mac, lmk, [channel], [ifidx], [encrypt]) (ESP32 only)
            ESPNow.mod_peer(mac, 'param'=value, ...) (ESP32 only)

    Modify the parameters of the peer associated with the provided ``mac``
    address. Parameters may be provided as positional or keyword arguments
    (see `ESPNow.add_peer()`).

Constants - (ESP32 Only)
------------------------

.. data:: espnow.MAX_DATA_LEN(=250)
          espnow.KEY_LEN(=16)
          espnow.MAX_TOTAL_PEER_NUM(=20)
          espnow.MAX_ENCRYPT_PEER_NUM(=6)

Exceptions
----------

If the underlying Espressif ESPNow software stack returns an error code,
the micropython ESPNow module will raise an ``OSError(errnum, errstring)``
exception where ``errstring`` is set to the name of one of the error codes
identified in the
`Espressif ESP-Now docs
<https://docs.espressif.com/projects/esp-idf/en/latest/
api-reference/network/esp_now.html#api-reference>`_. For example::

    try:
        e.send(peer, 'Hello')
    except OSError as err:
        if len(err.args) < 2:
            raise err
        if err.args[1] == 'ESP_ERR_ESPNOW_NOT_INIT':
            e.init()
        elif err.args[1] == 'ESP_ERR_ESPNOW_NOT_FOUND':
            e.add_peer(peer)
        elif err.args[1] == 'ESP_ERR_ESPNOW_IF':
            network.WLAN(network.STA_IF).active(True)
        else:
            raise err

Wifi Signal Strength (RSSI) - (ESP32 only)
------------------------------------------

The ESPnow module maintains a **peer device table** which contains the signal
strength of the last received message for all known peers. The **peer device
table** can be accessed using `ESPNow.peers` and can be used to track device
proximity and identify *nearest neighbours* in a network of peer devices. This
feature is **not** available on ESP8266 devices.

.. data:: ESPNow.peers

    A reference to the **peer device table**: a dict of known peer devices
    and rssi values::

        {peer: [rssi, time_ms], ...}

    where:

    - ``peer`` is the peer MAC address (as `bytes`);
    - ``rssi`` is the wifi signal strength in dBm (-127 to 0) of the last
      message received from the peer; and
    - ``time_ms`` is the time the message was received (in milliseconds since
      system boot - wraps every 12 days).

    Example::

      >>> e.peers
      {b'\xaa\xaa\xaa\xaa\xaa\xaa': [-31, 18372],
       b'\xbb\xbb\xbb\xbb\xbb\xbb': [-43, 12541]}

    **Note**: the ``mac`` addresses returned by `recv()` and `irecv()` are
    references to the ``peer`` key values in the **peer device table**.

    **Note**: RSSI and timestamp values in the device table are updated only
    when `ESPNow.irecv()` or `ESPNow.recv()` are called to read out the
    incoming message.

Supporting asyncio - (ESP32 Only)
---------------------------------

A supplementary module (`aioespnow`) is available to provide
:doc:`uasyncio<uasyncio>` support.

A small async server example::

    import network
    import aioespnow as espnow
    import uasyncio as asyncio

    # A WLAN interface must be active to send()/recv()
    network.WLAN(network.STA_IF).active(True)

    e = espnow.ESPNow()  # Returns AIOESPNow enhanced with async support
    e.init()
    peer = b'\xbb\xbb\xbb\xbb\xbb\xbb'
    e.add_peer(peer)

    # Send a periodic ping to a peer
    async def heartbeat(e, peer, period=30):
        while True:
            if not await e.asend(peer, b'ping'):
                print("Heartbeat: peer not responding:", peer)
            else:
                print("Heartbeat: ping", peer)
            await asyncio.sleep(period)

    # Echo any received messages back to the sender
    async def echo_server(e):
        async for mac, msg in e:
            print("Echo:", msg)
            try:
                await e.asend(mac, msg)
            except OSError as err:
                if len(err.args) > 1 and err.args[1] == 'ESP_ERR_ESPNOW_NOT_FOUND':
                    e.add_peer(mac)
                    await e.asend(mac, msg)

    async def main(e, peer, timeout, period):
        asyncio.create_task(heartbeat(e, peer, period))
        asyncio.create_task(echo_server(e))
        await asyncio.sleep(timeout)

    asyncio.run(main(e, peer, 120, 10))

.. module:: aioespnow
    :synopsis: ESP-NOW :doc:`uasyncio` support

.. class:: AIOESPNow(e)

    Returns the singleton `AIOESPNow` object. The `AIOESPNow` class inherits
    all the methods of `ESPNow<espnow.ESPNow>` and extends the interface with the
    following async methods. The constructor takes an optional argument which
    should be an existing `ESPNow<espnow.ESPNow>` instance.

.. method:: async AIOESPNow.arecv()
            async AIOESPNow.airecv()

    Asyncio support for `ESPNow.recv()` and `ESPNow.irecv()`. Note that
    these methods do not take a timeout value as argument.

.. method:: async AIOESPNow.asend(mac, msg, sync=True)
            async AIOESPNow.asend(msg)

    Asyncio support for `ESPNow.send()`.

**Snippet:** Extend an already initialised `ESPNow<espnow.ESPNow>` with
async support::

    ...

    from aioespnow import AIOESPNow
    import uasyncio as asyncio

    a = AIOESPNow(e)    # Return an asyncio enhanced ESPNow object

    asyncio.run(a.airecv())

**Snippet:** Use `AIOESPNow` as stand-in for `ESPNow<espnow.ESPNow>`::

    from aioespnow import AIOESPNow
    import uasyncio as asyncio

    e = AIOESPNow()    # An ESPNow object extended with async support

    e.init()
    peer = b'\xbb\xbb\xbb\xbb\xbb\xbb'
    e.add_peer(peer)

    asyncio.run(e.asend(peer, b'ping'))

.. function:: ESPNow()

    Return the `AIOESPNow` singleton object. This is a convenience function
    for adding async support to existing non-async code.

**Snippet:** Transition from existing non-async code::

    import network
    # from esp import espnow
    import aioespnow as espnow

    e = espnow.ESPNow()
    e.init()
    ...

`AIOESPNow` also supports reading incoming messages by asynchronous
iteration using ``async for``, eg::

    e = AIOESPNow()
    e.init()

    async def recv_till_halt(e):
        async for mac, msg in e:
            print(mac, msg)
            if msg == b'halt':
              break

    asyncio.run(recv_till_halt(e))

Stream IO interface - (deprecated)
----------------------------------

**The Stream IO interface support for reading and writing data is
deprecated!!**

Broadcast and Multicast
-----------------------

All active ESP-Now clients will receive messages sent to their MAC address and
all devices (**except ESP8266 devices**) will also receive messages sent to the
``broadcast`` MAC address (``b'\xff\xff\xff\xff\xff\xff'``) or any multicast
MAC address.

All ESP-Now devices (including ESP8266 devices) can also send messages to the
``broadcast`` MAC address or any multicast MAC address.

To `send()<ESPNow.send()>` a broadcast message, the ``broadcast`` (or
multicast) MAC address must first be registered using
`add_peer()<ESPNow.add_peer()>`. `send()<ESPNow.send()>` will always return
``True`` for broadcasts, regardless of whether any devices receive the
message. It is not permitted to encrypt messages sent to the ``broadcast``
address or any multicast address.

**Note**: `ESPNow.send(None, msg)<ESPNow.send()>` will send to all registered
peers *except* the broadcast address. To send a broadcast or multicast
message, you must specify the ``broadcast`` (or multicast) MAC address as the
peer. For example::

    bcast = b'\xff' * 6
    e.add_peer(bcast)
    e.send(bcast, "Hello World!")

ESPNow and Wifi Operation
-------------------------

The ESP32/8266 devices have two **apparently** independent wifi interfaces
(``STA_IF`` and ``AP_IF``) and each has their own MAC address. ESPNow messages
may be sent and received on any `active()<network.WLAN.active>`
`WLAN<network.WLAN()>` interface (``network.STA_IF`` or ``network.AP_IF``),
even if that interface is also connected to a wifi network or configured as an
access point.

Managing peers can become complex if you are using more than just the STA_IF
interface. You must:

- choose the correct MAC address of the remote peer (STA_IF or AP_IF) to
  register with `add_peer()`,
- register it with the correct local interface (``ifidx`` = STA_IF or AP_IF),
  and
- ensure the correct interfaces are ``active(True)`` on the local and remote
  peer.

`ESPNow.send()<ESPNow.send()>` will raise an
``OSError('ESP_ERR_ESPNOW_IF')``
exception when trying to send a message to a peer which is registered to a
local interface which is not ``active(True)``. Note also that both
interfaces may be active simultaneously, leading to a lot of flexibility
in configuring ESPNow and Wifi networks.

Sending ESPNow packets to a STA_IF interface which is also connected to a wifi
access point (AP) can be unreliable due to the default power saving mode
(WIFI_PS_MIN_MODEM) of the ESP32 when connected to an external Access Point.

There are several options to improve reliability of receiving ESPNow packets
when also connected to a wifi network:

1. Disable the power-saving mode on the STA_IF interface:

   - Use ``WLAN(STA_IF).config(ps_mode=WIFI_PS_NONE)``
   - This requires the ESPNow patches on ESP32 (not supported in micropython
     as of v1.17).

2. Use the AP_IF interface to send/receive ESPNow traffic:

   - Register all peers with ``e.add_peer(peer, lmk, channel, network.AP_IF)``
   - Configure peers to send messages to the ``AP_IF`` mac address
   - This will also activate the ESP32 as an access point!

3. Configure ESPNow clients to retry sending messages.

**Example 1:** Disable power saving mode on STA_IF::

  import network
  from esp import espnow

  peer = b'0\xaa\xaa\xaa\xaa\xaa'        # MAC address of peer
  e = espnow.ESPNow()
  e.init()

  w0 = network.WLAN(network.STA_IF)
  w0.active(True)
  w0.connect('myssid', 'myppassword')
  while not w0.isconnected():            # Wait until connected...
      time.sleep(0.1)
  w0.config(ps_mode=network.WIFI_PS_NONE)  # ..then disable power saving

  e.add_peer(peer)                       # Register peer on STA_IF
  if not e.send(peer, b'ping'):          # Message will be from STA_IF mac address
    print('Ping failed!')

  print('Send me messages at:', w0.config('mac'))

**Example 2:** Send and receive ESPNow traffic on AP_IF interface::

  import network
  from esp import espnow

  peer = b'feedee'                       # MAC address of peer
  e = espnow.ESPNow()
  e.init()

  w0 = network.WLAN(network.STA_IF)
  w0.active(True)                        # Set channel will fail unless Active
  w0.config(channel=6)
  w0.connect('myssid', 'myppassword')

  w1 = network.WLAN(network.AP_IF)
  w1.config(hidden=True)                 # AP_IF operates on same channel as STA_IF
  w1.active(True)

  e.add_peer(peer, None, None, network.AP_IF)  # Register peer on AP_IF
  e.send(peer, b'ping')                  # Message will be from AP_IF mac address

  print('Send me messages at:', w1.config('mac'))

Other issues to take care with when using ESPNow with wifi are:

- If using the ESP32 Access Point (AP_IF) while also connected to another
  Access Point (on STA_IF), the AP_IF will always operate on the same channel
  as the STA_IF regardless of the channel you set for the AP_IF
  (see
  `Attention Note 3
  <https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_wifi.html#_CPPv419esp_wifi_set_config16wifi_interface_tP13wifi_config_t>`_
  ).

- Some versions of the ESP IDF only permit sending ESPNow packets from the
  STA_IF interface to peers which have been registered on the same wifi
  channel as the STA_IF::

    ESPNOW: Peer channel is not equal to the home channel, send fail!

- Some versions of the ESP IDF don't permit setting the channel of the STA_IF
  at all, other than by connecting to an Access Point (This seems to be fixed
  in IDF 4+). Micropython versions without the ESPNow patches also provide no
  support for setting the channel of the STA_IF.

ESPNow and Sleep Modes
----------------------

The `machine.lightsleep([time_ms])<machine.lightsleep>` and
`machine.deepsleep([time_ms])<machine.deepsleep>` functions can be used to put
the ESP32 and periperals (including the WiFi and Bluetooth radios) to sleep.
This is useful in many applications to conserve battery power. However,
applications must disable the WLAN peripheral (using
`active(False)<network.WLAN.active>`) before entering light or deep sleep (see
`Sleep Modes <https://docs.espressif.com/
projects/esp-idf/en/latest/esp32/api-reference/system/sleep_modes.html>`_).
Otherwise the WiFi radio may not be initialised properly after wake from
sleep. If the ``STA_IF`` and ``AP_IF`` interfaces have both been set
`active(True)<network.WLAN.active()>` then both interfaces should be set
`active(False)<network.WLAN.active()>` before entering any sleep mode.

**Example:** deep sleep::

  import network
  import machine
  from esp import espnow

  peer = b'0\xaa\xaa\xaa\xaa\xaa'        # MAC address of peer
  e = espnow.ESPNow()
  e.init()

  w0 = network.WLAN(network.STA_IF)
  w0.active(True)
  e.add_peer(peer)                       # Register peer on STA_IF

  print('Sending ping...')
  if not e.send(peer, b'ping'):
    print('Ping failed!')

  e.deinit()
  w0.active(False)                       # Disable the wifi before sleep

  print('Going to sleep...')
  machine.deepsleep(10000)               # Sleep for 10 seconds then reboot

**Example:** light sleep::

  import network
  import machine
  from esp import espnow

  peer = b'0\xaa\xaa\xaa\xaa\xaa'        # MAC address of peer
  e = espnow.ESPNow()
  e.init()

  w0 = network.WLAN(network.STA_IF)
  w0.active(True)                        # Set channel will fail unless Active
  w0.config(channel=6)
  e.add_peer(peer)                       # Register peer on STA_IF

  while True:
    print('Sending ping...')
    if not e.send(peer, b'ping'):
      print('Ping failed!')

    w0.active(False)                     # Disable the wifi before sleep

    print('Going to sleep...')
    machine.lightsleep(10000)            # Sleep for 10 seconds

    w0.active(True)
    w0.config(channel=6)                 # Wifi loses config after lightsleep()

Notes on using on_recv callbacks
--------------------------------

The `ESPNow.config(on_recv=callback)<ESPNow.config()>` callback method is a
convenient and responsive way of processing incoming espnow messages,
especially if the data rate is moderate and the device is *not too busy* but
there are some caveats:

- The scheduler stack *can* easily overflow and callbacks will be missed if
  packets are arriving at a sufficient rate. It is a good idea to readout all
  available packets in the callback in case a prior callback has been missed,
  eg::

    def recv_cb2(e):
        while e.poll():
            print(e.irecv(0))
    e.config(on_recv=recv_cb2)

- Message callbacks may still be missed if the scheduler stack is
  overflowed by other micropython components (eg, bluetooth,
  machine.Pin.irq(), machine.timer, i2s, ...). Generally, this method may be
  less reliable for dealing with intense bursts of messages, or high
  throughput or on a device which is very busy dealing with other hardware
  operations.

- Take care if reading out messages with `irecv()` in the normal micropython
  control flow *and* in ``on_recv`` callbacks, as the memory returned by
  `irecv()` is shared.

- For more information on *scheduled* function callbacks see:
  `micropython.schedule()<micropython.schedule>`.
