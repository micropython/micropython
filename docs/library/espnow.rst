:mod:`espnow` --- Support for the ESP-NOW protocol
==================================================

.. module:: espnow
   :synopsis: ESP-NOW wireless protocol support

This module provides an interface to the ESP-NOW protocol provided by Espressif
on ESP32 and ESP8266 devices.

Load this module from the :doc:`esp` module. A simple example would be:

**Sender:** ::

        import network
        from esp import espnow

        # A WLAN interface must be active to send()/recv()
        w0 = network.WLAN(network.STA_IF)  # Or network.AP_IF
        w0.active(True)

        e = espnow.ESPNow()
        e.init()
        peer = b'0\xae\xa4XC\xd0'   # MAC address of peer's wifi interface
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

        e = espnow.ESPNow()
        e.init()
        peer = b'0\xae\xa4XC\xa0'   # MAC address of peer's wifi interface
        e.add_peer(peer)

        print(e.irecv())
        for msg in e:
            print(msg)
            if (msg[1] == b'end')
                break

.. note:: This module is still under development and its classes, functions,
          methods and constants are subject to change.

class ESPNow
------------

Constructor
-----------

.. class:: ESPNow()

    Returns the singleton ESPNow object.

Configuration
-------------

.. method:: ESPNow.init()

    Prepare the software and hardware for use of the ESPNow communication
    protocol, including:

    - initialise the ESPNow data structures,
    - allocate the send and recv data buffers,
    - invoke esp_now_init() and
    - register the send and recv callbacks.

.. method:: ESPNow.deinit()

    De-initialise the Espressif ESPNow software stack (esp_now_deinit()), disable
    the interrupt callbacks and deallocate the send and recv data buffers.

    **Note**: `deinit()` will also deregister all peers which must be
    re-registered after `init()`.

.. method:: ESPNow.config()
            ESPNow.config('param')
            ESPNow.config(param=value, ...)

    Get or set configuration values of the ESPNow interface.  To get a value the
    parameter name should be quoted as a string, and just one parameter is
    queried at a time.  To set values use the keyword syntax, and one or more
    parameters can be set at a time. Invocation with no arguments will return a
    dict of all parameter names and values.

    Currently supported values are:

    - ``txbuf``: *(default=140)* Get/set the size in bytes of the internal
      buffer used to store send response data. Increasing this allows better
      handling of bursty outgoing/incoming data.

    - ``rxbuf``: *(default=514)* Get/set the size in bytes of the internal
      buffer used to store incoming ESPNow packet data. The default size is
      selected to fit two
      max-sized ESPNow packets (250 bytes) with associated mac_address (6 bytes)
      and a message byte count (1 byte) plus buffer overhead. Increase this if
      you expect to receive a lot of large packets or expect bursty incoming
      traffic.

      **Note:** The send and recv buffers are only allocated by `ESPNow.init()`.
      Changing these values will have no effect until the next call of
      `ESPNow.init()`.

    - ``timeout``: *(default=300,000)* Default read timeout (in milliseconds).
      The timeout can also be provided as arg to `recv()` and `irecv()`.

.. method:: ESPNow.clear(True)

    Clear out any data in the recv (and send-response) buffers. Use this to clean
    up after receiving a ``Buffer error`` (should not happen). All data in the
    buffers will be discarded. An arg of `True` is required to guard against
    inadvertent use.

.. method:: ESPNow.set_pmk(pmk)

    Set the Primary Master Key (PMK) which is used to encrypt the Local Master
    Keys (LMK) for encrypting ESPNow data traffic. If this is not set, a default
    PMK is used by the underlying Espressif esp_now software stack. The ``pmk``
    argument bust be a byte string of length `espnow.KEY_LEN` (16 bytes).

Sending and Receiving Data
--------------------------

A wifi interface (``network.STA_IF`` or ``network.AP_IF``) must be
`active()<network.WLAN.active>`
before messages can be sent or received, but it is not necessary to connect or
configure the WLAN interface. For example::

    import network

    w0 = network.WLAN(network.STA_IF)
    w0.active(True)

.. method:: ESPNow.send(mac, msg, [sync])

    Send the data contained in ``msg`` to the peer with given network ``mac``
    address. ``mac`` must be a byte string exactly 6 bytes long and ``msg`` must
    be a string or byte-string such that ``0<len(msg)<=espnow.MAX_DATA_LEN``
    (250) bytes. If ``mac`` is ``None`` the message will be sent to all
    registered peers as an ESP-Now broadcast.

    - If ``sync=True`` send ``msg`` to the peer and wait for a response (or not).
      Returns ``False`` if any peers fail to respond.

    - If ``sync=False`` handover ``msg`` to the esp_now software stack for
      transmission and return immediately.
      Responses from the peers will be discarded.
      Always returns ``True``.

    **Note**: A peer will respond with success if it's wifi interface is
    active(), regardless of whether it has initialised it's ESP-Now system or
    is actively listening for ESP-Now traffic (see the Espressif ESP-Now docs).

.. method:: ESPNow.recv([timeout])

    Wait for an incoming message and return a newly allocated tuple of
    bytestrings: ``(mac, message)``, where:

    - ``mac`` is the mac address of the sending device (peer) and

    - ``msg`` is the message/data sent from the peer.

    ``Timeout`` optionally sets a timeout (in milliseconds) for the read. The
    default timeout can be set in `ESPNow.config()`.

    **Note**: repeatedly calling `irecv()<ESPNow.send()>` will exercise the
    micropython garbage collection as new storage is allocated for each new
    message and tuple. Use `irecv()<ESPNow.irecv()>`
    for a more memory-efficient option.

.. method:: ESPNow.irecv([timeout])

    As for `recv()<ESPNow.recv()>` except that ``irecv()`` will return a
    "callee-owned" tuple.
    That is, memory will be allocated for the tuple and byte strings on the
    first call and re-used for subsequent calls. You must make copies if you
    wish to keep the values across calls to ``irecv()``.

.. method:: ESPNow.stats()

    Return a 4-tuple containing the number of packets sent/received/lost:
    ``(sent_packets, send_responses, recv_packets, lost_rx_packets)``. Packets
    are *lost* when the recv buffers are full. To reduce packet loss,
    increase the ``txbuf`` and ``rxbuf`` config parameters and ensure you are
    in a tight loop calling `irecv()<ESPNow.irecv()>` as quickly as possible.

Iteration over ESPNow
---------------------

It is also possible to read messages by iterating over the ESPNow singleton
object. This will yield ``(mac, message)`` tuples using the alloc-free
`irecv()` method, eg::

        for msg in e:
            print(f"Recv: mac={msg[0]}, message={msg[1]}")

**Note**: Iteration will yield ``None`` if the default timeout expires waiting
for a message.

Peer Management
---------------

The ESP-Now protocol requires that other devices (peers) must be *registered*
before we can `send()<ESPNow.send()>` them messages.

.. method:: ESPNow.add_peer(mac, [lmk], [channel], [ifidx], [encrypt])
            ESPNow.add_peer(mac, 'param'=value, ...)

    Add/register the provided ``mac`` address (a 6-byte byte-string) as a peer
    under the ESPNow protocol. The following "peer info" parameters may also be
    specified as positional or keyword arguments:

    - ``lmk``: The Local Master Key (LMK) key used to encrypt data transfers
      with this peer (if the *encrypt* parameter is set to *True*). Must be:

      - a byte-string of length ``<= espnow.KEY_LEN`` (16 bytes) (the key will be
        padded to the right with zeroes), or
      - any non-`True` python value (default= ``b''``), signifying an *empty* key
        which will disable encryption.

    - ``channel``: The wifi channel (2.4GHz) to communicate with this peer. Must
      be an integer from 0 to 14. If channel is set to 0 the current channel
      of the wifi device will be used. (default=0)

    - ``ifidx``: Index of the wifi interface which will be used to send data to
      this peer. Must be an integer set to ``network.STA_IF`` (=0) or
      ``network.AP_IF`` (=1). (default=0/``network.STA_IF``).

    - ``encrypt``: If set to ``True`` data exchanged with this peer will be
      encrypted with the PMK and LMK. (default=``False``)

.. method:: ESPNow.get_peer(mac)

    Return a 5-tuple of the "peer info" associated with the ``mac`` address::

        (mac, lmk, channel, ifidx, encrypt)

.. method:: ESPNow.peer_count()

    Return the number of peers which have been registered.

.. method:: ESPNow.get_peers()

    Return the "peer info" parameters for all the registered peers (as a tuple
    of tuples).

.. method:: ESPNow.mod_peer(mac, lmk, [channel], [ifidx], [encrypt])
            ESPNow.mod_peer(mac, 'param'=value, ...)

    Modify the parameters of the peer associated with the provided ``mac``
    address. Parameters may be provided as positional or keyword arguments.

.. method:: ESPNow.del_peer(mac)

    Deregister the peer associated with the provided ``mac`` address.

Stream IO interface
-------------------

**Note**: The ESPNow buffer packet format is not yet fully documented. It
will be supported by a python support module for reading and sending ESPNow
message packets through the ``stream`` interface.

EspNow also supports the micropython ``stream`` io interface. This is intended
to help support high throughput low-copy transfers and also to support
``uasyncio`` through the StreamReader interface. ESPNow includes
support for the following python
`stream interface <https://docs.python.org/3/library/io.html>`_ methods:

.. method:: ESPNow.read([size=-1])

    Return up to ``size`` bytes read from the espnow recv buffers as a byte
    string. Is nonblocking and returns None if no data available. The returned
    data is a stream of ESPNow buffer packet data.

.. method:: ESPNow.read1([size=-1])

    As for `read()` but will return after at most one packet is read.

.. method:: ESPNow.readinto(b)

    Read bytes into a pre-allocated, writable bytes-like object (eg. bytearray)
    and return the number of bytes read. Is nonblocking and returns None if no
    data available.

.. method:: ESPNow.readinto1(b)

    As for `readinto()` but will return after at most one packet is read.

.. method:: ESPNow.write(b)

    Write the given bytes-like object to the ESPNow interface. ``b`` must
    contain a sequence of ESPNow buffer packet data.

`ESPNow` also supports the ``poll.poll`` and ``poll.ipoll`` calls, so users
may wait on received events.

Supporting ``uasyncio``
-----------------------

`ESPNow` uses the ``stream`` io interface to support the micropython
``uasyncio`` module for asynchronous IO. A ``StreamReader`` class may be
constructed from an ESPNow object and used to support async IO. Eg::

        s = StreamReader(e)

        async def areadespnow(s):
            while e.send(b'ping'):
                msg = await(s.read1())
                if msg[8:] != b'pong'
                    break

Constants
---------

.. data:: espnow.MAX_DATA_LEN         (=250)
          espnow.KEY_LEN              (=16)
          espnow.MAX_TOTAL_PEER_NUM   (=20)
          espnow.MAX_ENCRYPT_PEER_NUM (=6)
