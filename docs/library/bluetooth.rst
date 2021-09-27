:mod:`bluetooth` --- low-level Bluetooth
========================================

.. module:: bluetooth
   :synopsis: Low-level Bluetooth radio functionality

This module provides an interface to a Bluetooth controller on a board.
Currently this supports Bluetooth Low Energy (BLE) in Central, Peripheral,
Broadcaster, and Observer roles, as well as GATT Server and Client and L2CAP
connection-oriented-channels. A device may operate in multiple roles
concurrently. Pairing (and bonding) is supported on some ports.

This API is intended to match the low-level Bluetooth protocol and provide
building-blocks for higher-level abstractions such as specific device types.

.. note:: This module is still under development and its classes, functions,
          methods and constants are subject to change.

class BLE
---------

Constructor
-----------

.. class:: BLE()

    Returns the singleton BLE object.

Configuration
-------------

.. method:: BLE.active([active], /)

    Optionally changes the active state of the BLE radio, and returns the
    current state.

    The radio must be made active before using any other methods on this class.

.. method:: BLE.config('param', /)
            BLE.config(*, param=value, ...)

    Get or set configuration values of the BLE interface.  To get a value the
    parameter name should be quoted as a string, and just one parameter is
    queried at a time.  To set values use the keyword syntax, and one ore more
    parameter can be set at a time.

    Currently supported values are:

    - ``'mac'``: The current address in use, depending on the current address mode.
      This returns a tuple of ``(addr_type, addr)``.

      See :meth:`gatts_write <BLE.gatts_write>` for details about address type.

      This may only be queried while the interface is currently active.

    - ``'addr_mode'``: Sets the address mode. Values can be:

        * 0x00 - PUBLIC - Use the controller's public address.
        * 0x01 - RANDOM - Use a generated static address.
        * 0x02 - RPA - Use resolvable private addresses.
        * 0x03 - NRPA - Use non-resolvable private addresses.

      By default the interface mode will use a PUBLIC address if available, otherwise
      it will use a RANDOM address.

    - ``'gap_name'``: Get/set the GAP device name used by service 0x1800,
      characteristic 0x2a00.  This can be set at any time and changed multiple
      times.

    - ``'rxbuf'``: Get/set the size in bytes of the internal buffer used to store
      incoming events.  This buffer is global to the entire BLE driver and so
      handles incoming data for all events, including all characteristics.
      Increasing this allows better handling of bursty incoming data (for
      example scan results) and the ability to receive larger characteristic values.

    - ``'mtu'``: Get/set the MTU that will be used during a ATT MTU exchange. The
      resulting MTU will be the minimum of this and the remote device's MTU.
      ATT MTU exchange will not happen automatically (unless the remote device initiates
      it), and must be manually initiated with
      :meth:`gattc_exchange_mtu<BLE.gattc_exchange_mtu>`.
      Use the ``_IRQ_MTU_EXCHANGED`` event to discover the MTU for a given connection.

    - ``'bond'``: Sets whether bonding will be enabled during pairing. When
      enabled, pairing requests will set the "bond" flag and the keys will be stored
      by both devices.

    - ``'mitm'``: Sets whether MITM-protection is required for pairing.

    - ``'io'``: Sets the I/O capabilities of this device.

      Available options are::

        _IO_CAPABILITY_DISPLAY_ONLY = const(0)
        _IO_CAPABILITY_DISPLAY_YESNO = const(1)
        _IO_CAPABILITY_KEYBOARD_ONLY = const(2)
        _IO_CAPABILITY_NO_INPUT_OUTPUT = const(3)
        _IO_CAPABILITY_KEYBOARD_DISPLAY = const(4)

    - ``'le_secure'``: Sets whether "LE Secure" pairing is required. Default is
      false (i.e. allow "Legacy Pairing").

Event Handling
--------------

.. method:: BLE.irq(handler, /)

    Registers a callback for events from the BLE stack. The *handler* takes two
    arguments, ``event`` (which will be one of the codes below) and ``data``
    (which is an event-specific tuple of values).

    **Note:** As an optimisation to prevent unnecessary allocations, the ``addr``,
    ``adv_data``, ``char_data``, ``notify_data``, and ``uuid`` entries in the
    tuples are read-only memoryview instances pointing to :mod:`bluetooth`'s internal
    ringbuffer, and are only valid during the invocation of the IRQ handler
    function.  If your program needs to save one of these values to access after
    the IRQ handler has returned (e.g. by saving it in a class instance or global
    variable), then it needs to take a copy of the data, either by using ``bytes()``
    or ``bluetooth.UUID()``, like this::

        connected_addr = bytes(addr)  # equivalently: adv_data, char_data, or notify_data
        matched_uuid = bluetooth.UUID(uuid)

    For example, the IRQ handler for a scan result might inspect the ``adv_data``
    to decide if it's the correct device, and only then copy the address data to be
    used elsewhere in the program.  And to print data from within the IRQ handler,
    ``print(bytes(addr))`` will be needed.

    An event handler showing all possible events::

        def bt_irq(event, data):
            if event == _IRQ_CENTRAL_CONNECT:
                # A central has connected to this peripheral.
                conn_handle, addr_type, addr = data
            elif event == _IRQ_CENTRAL_DISCONNECT:
                # A central has disconnected from this peripheral.
                conn_handle, addr_type, addr = data
            elif event == _IRQ_GATTS_WRITE:
                # A client has written to this characteristic or descriptor.
                conn_handle, attr_handle = data
            elif event == _IRQ_GATTS_READ_REQUEST:
                # A client has issued a read. Note: this is only supported on STM32.
                # Return a non-zero integer to deny the read (see below), or zero (or None)
                # to accept the read.
                conn_handle, attr_handle = data
            elif event == _IRQ_SCAN_RESULT:
                # A single scan result.
                addr_type, addr, adv_type, rssi, adv_data = data
            elif event == _IRQ_SCAN_DONE:
                # Scan duration finished or manually stopped.
                pass
            elif event == _IRQ_PERIPHERAL_CONNECT:
                # A successful gap_connect().
                conn_handle, addr_type, addr = data
            elif event == _IRQ_PERIPHERAL_DISCONNECT:
                # Connected peripheral has disconnected.
                conn_handle, addr_type, addr = data
            elif event == _IRQ_GATTC_SERVICE_RESULT:
                # Called for each service found by gattc_discover_services().
                conn_handle, start_handle, end_handle, uuid = data
            elif event == _IRQ_GATTC_SERVICE_DONE:
                # Called once service discovery is complete.
                # Note: Status will be zero on success, implementation-specific value otherwise.
                conn_handle, status = data
            elif event == _IRQ_GATTC_CHARACTERISTIC_RESULT:
                # Called for each characteristic found by gattc_discover_services().
                conn_handle, def_handle, value_handle, properties, uuid = data
            elif event == _IRQ_GATTC_CHARACTERISTIC_DONE:
                # Called once service discovery is complete.
                # Note: Status will be zero on success, implementation-specific value otherwise.
                conn_handle, status = data
            elif event == _IRQ_GATTC_DESCRIPTOR_RESULT:
                # Called for each descriptor found by gattc_discover_descriptors().
                conn_handle, dsc_handle, uuid = data
            elif event == _IRQ_GATTC_DESCRIPTOR_DONE:
                # Called once service discovery is complete.
                # Note: Status will be zero on success, implementation-specific value otherwise.
                conn_handle, status = data
            elif event == _IRQ_GATTC_READ_RESULT:
                # A gattc_read() has completed.
                conn_handle, value_handle, char_data = data
            elif event == _IRQ_GATTC_READ_DONE:
                # A gattc_read() has completed.
                # Note: The value_handle will be zero on btstack (but present on NimBLE).
                # Note: Status will be zero on success, implementation-specific value otherwise.
                conn_handle, value_handle, status = data
            elif event == _IRQ_GATTC_WRITE_DONE:
                # A gattc_write() has completed.
                # Note: The value_handle will be zero on btstack (but present on NimBLE).
                # Note: Status will be zero on success, implementation-specific value otherwise.
                conn_handle, value_handle, status = data
            elif event == _IRQ_GATTC_NOTIFY:
                # A server has sent a notify request.
                conn_handle, value_handle, notify_data = data
            elif event == _IRQ_GATTC_INDICATE:
                # A server has sent an indicate request.
                conn_handle, value_handle, notify_data = data
            elif event == _IRQ_GATTS_INDICATE_DONE:
                # A client has acknowledged the indication.
                # Note: Status will be zero on successful acknowledgment, implementation-specific value otherwise.
                conn_handle, value_handle, status = data
            elif event == _IRQ_MTU_EXCHANGED:
                # ATT MTU exchange complete (either initiated by us or the remote device).
                conn_handle, mtu = data
            elif event == _IRQ_L2CAP_ACCEPT:
                # A new channel has been accepted.
                # Return a non-zero integer to reject the connection, or zero (or None) to accept.
                conn_handle, cid, psm, our_mtu, peer_mtu = data
            elif event == _IRQ_L2CAP_CONNECT:
                # A new channel is now connected (either as a result of connecting or accepting).
                conn_handle, cid, psm, our_mtu, peer_mtu = data
            elif event == _IRQ_L2CAP_DISCONNECT:
                # Existing channel has disconnected (status is zero), or a connection attempt failed (non-zero status).
                conn_handle, cid, psm, status = data
            elif event == _IRQ_L2CAP_RECV:
                # New data is available on the channel. Use l2cap_recvinto to read.
                conn_handle, cid = data
            elif event == _IRQ_L2CAP_SEND_READY:
                # A previous l2cap_send that returned False has now completed and the channel is ready to send again.
                # If status is non-zero, then the transmit buffer overflowed and the application should re-send the data.
                conn_handle, cid, status = data
            elif event == _IRQ_CONNECTION_UPDATE:
                # The remote device has updated connection parameters.
                conn_handle, conn_interval, conn_latency, supervision_timeout, status = data
            elif event == _IRQ_ENCRYPTION_UPDATE:
                # The encryption state has changed (likely as a result of pairing or bonding).
                conn_handle, encrypted, authenticated, bonded, key_size = data
            elif event == _IRQ_GET_SECRET:
                # Return a stored secret.
                # If key is None, return the index'th value of this sec_type.
                # Otherwise return the corresponding value for this sec_type and key.
                sec_type, index, key = data
                return value
            elif event == _IRQ_SET_SECRET:
                # Save a secret to the store for this sec_type and key.
                sec_type, key, value = data
                return True
            elif event == _IRQ_PASSKEY_ACTION:
                # Respond to a passkey request during pairing.
                # See gap_passkey() for details.
                # action will be an action that is compatible with the configured "io" config.
                # passkey will be non-zero if action is "numeric comparison".
                conn_handle, action, passkey = data


The event codes are::

    from micropython import const
    _IRQ_CENTRAL_CONNECT = const(1)
    _IRQ_CENTRAL_DISCONNECT = const(2)
    _IRQ_GATTS_WRITE = const(3)
    _IRQ_GATTS_READ_REQUEST = const(4)
    _IRQ_SCAN_RESULT = const(5)
    _IRQ_SCAN_DONE = const(6)
    _IRQ_PERIPHERAL_CONNECT = const(7)
    _IRQ_PERIPHERAL_DISCONNECT = const(8)
    _IRQ_GATTC_SERVICE_RESULT = const(9)
    _IRQ_GATTC_SERVICE_DONE = const(10)
    _IRQ_GATTC_CHARACTERISTIC_RESULT = const(11)
    _IRQ_GATTC_CHARACTERISTIC_DONE = const(12)
    _IRQ_GATTC_DESCRIPTOR_RESULT = const(13)
    _IRQ_GATTC_DESCRIPTOR_DONE = const(14)
    _IRQ_GATTC_READ_RESULT = const(15)
    _IRQ_GATTC_READ_DONE = const(16)
    _IRQ_GATTC_WRITE_DONE = const(17)
    _IRQ_GATTC_NOTIFY = const(18)
    _IRQ_GATTC_INDICATE = const(19)
    _IRQ_GATTS_INDICATE_DONE = const(20)
    _IRQ_MTU_EXCHANGED = const(21)
    _IRQ_L2CAP_ACCEPT = const(22)
    _IRQ_L2CAP_CONNECT = const(23)
    _IRQ_L2CAP_DISCONNECT = const(24)
    _IRQ_L2CAP_RECV = const(25)
    _IRQ_L2CAP_SEND_READY = const(26)
    _IRQ_CONNECTION_UPDATE = const(27)
    _IRQ_ENCRYPTION_UPDATE = const(28)
    _IRQ_GET_SECRET = const(29)
    _IRQ_SET_SECRET = const(30)

For the ``_IRQ_GATTS_READ_REQUEST`` event, the available return codes are::

    _GATTS_NO_ERROR = const(0x00)
    _GATTS_ERROR_READ_NOT_PERMITTED = const(0x02)
    _GATTS_ERROR_WRITE_NOT_PERMITTED = const(0x03)
    _GATTS_ERROR_INSUFFICIENT_AUTHENTICATION = const(0x05)
    _GATTS_ERROR_INSUFFICIENT_AUTHORIZATION = const(0x08)
    _GATTS_ERROR_INSUFFICIENT_ENCRYPTION = const(0x0f)

For the ``_IRQ_PASSKEY_ACTION`` event, the available actions are::

    _PASSKEY_ACTION_NONE = const(0)
    _PASSKEY_ACTION_INPUT = const(2)
    _PASSKEY_ACTION_DISPLAY = const(3)
    _PASSKEY_ACTION_NUMERIC_COMPARISON = const(4)

In order to save space in the firmware, these constants are not included on the
:mod:`bluetooth` module. Add the ones that you need from the list above to your
program.


Broadcaster Role (Advertiser)
-----------------------------

.. method:: BLE.gap_advertise(interval_us, adv_data=None, *, resp_data=None, connectable=True)

    Starts advertising at the specified interval (in **micro**\ seconds). This
    interval will be rounded down to the nearest 625us. To stop advertising, set
    *interval_us* to ``None``.

    *adv_data* and *resp_data* can be any type that implements the buffer
    protocol (e.g. ``bytes``, ``bytearray``, ``str``). *adv_data* is included
    in all broadcasts, and *resp_data* is send in reply to an active scan.

    **Note:** if *adv_data* (or *resp_data*) is ``None``, then the data passed
    to the previous call to ``gap_advertise`` will be re-used. This allows a
    broadcaster to resume advertising with just ``gap_advertise(interval_us)``.
    To clear the advertising payload pass an empty ``bytes``, i.e. ``b''``.


Observer Role (Scanner)
-----------------------

.. method:: BLE.gap_scan(duration_ms, interval_us=1280000, window_us=11250, active=False, /)

    Run a scan operation lasting for the specified duration (in **milli**\ seconds).

    To scan indefinitely, set *duration_ms* to ``0``.

    To stop scanning, set *duration_ms* to ``None``.

    Use *interval_us* and *window_us* to optionally configure the duty cycle.
    The scanner will run for *window_us* **micro**\ seconds every *interval_us*
    **micro**\ seconds for a total of *duration_ms* **milli**\ seconds. The default
    interval and window are 1.28 seconds and 11.25 milliseconds respectively
    (background scanning).

    For each scan result the ``_IRQ_SCAN_RESULT`` event will be raised, with event
    data ``(addr_type, addr, adv_type, rssi, adv_data)``.

    ``addr_type`` values indicate public or random addresses:
        * 0x00 - PUBLIC
        * 0x01 - RANDOM (either static, RPA, or NRPA, the type is encoded in the address itself)

    ``adv_type`` values correspond to the Bluetooth Specification:

        * 0x00 - ADV_IND - connectable and scannable undirected advertising
        * 0x01 - ADV_DIRECT_IND - connectable directed advertising
        * 0x02 - ADV_SCAN_IND - scannable undirected advertising
        * 0x03 - ADV_NONCONN_IND - non-connectable undirected advertising
        * 0x04 - SCAN_RSP - scan response

    ``active`` can be set ``True`` if you want to receive scan responses in the results.

    When scanning is stopped (either due to the duration finishing or when
    explicitly stopped), the ``_IRQ_SCAN_DONE`` event will be raised.


Central Role
------------

A central device can connect to peripherals that it has discovered using the observer role (see :meth:`gap_scan<BLE.gap_scan>`) or with a known address.

.. method:: BLE.gap_connect(addr_type, addr, scan_duration_ms=2000, min_conn_interval_us=None, max_conn_interval_us=None, /)

    Connect to a peripheral.

    See :meth:`gap_scan <BLE.gap_scan>` for details about address types.

    On success, the ``_IRQ_PERIPHERAL_CONNECT`` event will be raised.

    The device will wait up to *scan_duration_ms* to receive an advertising
    payload from the device.

    The connection interval can be configured in **micro**\ seconds using either
    or both of *min_conn_interval_us* and *max_conn_interval_us*. Otherwise a
    default interval will be chosen, typically between 30000 and 50000
    microseconds. A shorter interval will increase throughput, at the expense
    of power usage.


Peripheral Role
---------------

A peripheral device is expected to send connectable advertisements (see
:meth:`gap_advertise<BLE.gap_advertise>`). It will usually be acting as a GATT
server, having first registered services and characteristics using
:meth:`gatts_register_services<BLE.gatts_register_services>`.

When a central connects, the ``_IRQ_CENTRAL_CONNECT`` event will be raised.


Central & Peripheral Roles
--------------------------

.. method:: BLE.gap_disconnect(conn_handle, /)

    Disconnect the specified connection handle. This can either be a
    central that has connected to this device (if acting as a peripheral)
    or a peripheral that was previously connected to by this device (if acting
    as a central).

    On success, the ``_IRQ_PERIPHERAL_DISCONNECT`` or ``_IRQ_CENTRAL_DISCONNECT``
    event will be raised.

    Returns ``False`` if the connection handle wasn't connected, and ``True``
    otherwise.


GATT Server
-----------

A GATT server has a set of registered services. Each service may contain
characteristics, which each have a value. Characteristics can also contain
descriptors, which themselves have values.

These values are stored locally, and are accessed by their "value handle" which
is generated during service registration. They can also be read from or written
to by a remote client device. Additionally, a server can "notify" a
characteristic to a connected client via a connection handle.

A device in either central or peripheral roles may function as a GATT server,
however in most cases it will be more common for a peripheral device to act
as the server.

Characteristics and descriptors have a default maximum size of 20 bytes.
Anything written to them by a client will be truncated to this length. However,
any local write will increase the maximum size, so if you want to allow larger
writes from a client to a given characteristic, use
:meth:`gatts_write<BLE.gatts_write>` after registration. e.g.
``gatts_write(char_handle, bytes(100))``.

.. method:: BLE.gatts_register_services(services_definition, /)

    Configures the server with the specified services, replacing any
    existing services.

    *services_definition* is a list of **services**, where each **service** is a
    two-element tuple containing a UUID and a list of **characteristics**.

    Each **characteristic** is a two-or-three-element tuple containing a UUID, a
    **flags** value, and optionally a list of *descriptors*.

    Each **descriptor** is a two-element tuple containing a UUID and a **flags**
    value.

    The **flags** are a bitwise-OR combination of the flags defined below. These
    set both the behaviour of the characteristic (or descriptor) as well as the
    security and privacy requirements.

    The return value is a list (one element per service) of tuples (each element
    is a value handle). Characteristics and descriptor handles are flattened
    into the same tuple, in the order that they are defined.

    The following example registers two services (Heart Rate, and Nordic UART)::

        HR_UUID = bluetooth.UUID(0x180D)
        HR_CHAR = (bluetooth.UUID(0x2A37), bluetooth.FLAG_READ | bluetooth.FLAG_NOTIFY,)
        HR_SERVICE = (HR_UUID, (HR_CHAR,),)
        UART_UUID = bluetooth.UUID('6E400001-B5A3-F393-E0A9-E50E24DCCA9E')
        UART_TX = (bluetooth.UUID('6E400003-B5A3-F393-E0A9-E50E24DCCA9E'), bluetooth.FLAG_READ | bluetooth.FLAG_NOTIFY,)
        UART_RX = (bluetooth.UUID('6E400002-B5A3-F393-E0A9-E50E24DCCA9E'), bluetooth.FLAG_WRITE,)
        UART_SERVICE = (UART_UUID, (UART_TX, UART_RX,),)
        SERVICES = (HR_SERVICE, UART_SERVICE,)
        ( (hr,), (tx, rx,), ) = bt.gatts_register_services(SERVICES)

    The three value handles (``hr``, ``tx``, ``rx``) can be used with
    :meth:`gatts_read <BLE.gatts_read>`, :meth:`gatts_write <BLE.gatts_write>`, :meth:`gatts_notify <BLE.gatts_notify>`, and
    :meth:`gatts_indicate <BLE.gatts_indicate>`.

    **Note:** Advertising must be stopped before registering services.

    Available flags for characteristics and descriptors are::

        from micropython import const
        _FLAG_BROADCAST = const(0x0001)
        _FLAG_READ = const(0x0002)
        _FLAG_WRITE_NO_RESPONSE = const(0x0004)
        _FLAG_WRITE = const(0x0008)
        _FLAG_NOTIFY = const(0x0010)
        _FLAG_INDICATE = const(0x0020)
        _FLAG_AUTHENTICATED_SIGNED_WRITE = const(0x0040)

        _FLAG_AUX_WRITE = const(0x0100)
        _FLAG_READ_ENCRYPTED = const(0x0200)
        _FLAG_READ_AUTHENTICATED = const(0x0400)
        _FLAG_READ_AUTHORIZED = const(0x0800)
        _FLAG_WRITE_ENCRYPTED = const(0x1000)
        _FLAG_WRITE_AUTHENTICATED = const(0x2000)
        _FLAG_WRITE_AUTHORIZED = const(0x4000)

    As for the IRQs above, any required constants should be added to your Python code.

.. method:: BLE.gatts_read(value_handle, /)

    Reads the local value for this handle (which has either been written by
    :meth:`gatts_write <BLE.gatts_write>` or by a remote client).

.. method:: BLE.gatts_write(value_handle, data, send_update=False, /)

    Writes the local value for this handle, which can be read by a client.

    If *send_update* is ``True``, then any subscribed clients will be notified
    (or indicated, depending on what they're subscribed to and which operations
    the characteristic supports) about this write.

.. method:: BLE.gatts_notify(conn_handle, value_handle, data=None, /)

    Sends a notification request to a connected client.

    If *data* is not ``None``, then that value is sent to the client as part of
    the notification. The local value will not be modified.

    Otherwise, if *data* is ``None``, then the current local value (as
    set with :meth:`gatts_write <BLE.gatts_write>`) will be sent.

    **Note:** The notification will be sent regardless of the subscription
    status of the client to this characteristic.

.. method:: BLE.gatts_indicate(conn_handle, value_handle, /)

    Sends an indication request containing the characteristic's current value to
    a connected client.

    On acknowledgment (or failure, e.g. timeout), the
    ``_IRQ_GATTS_INDICATE_DONE`` event will be raised.

    **Note:** The indication will be sent regardless of the subscription
    status of the client to this characteristic.

.. method:: BLE.gatts_set_buffer(value_handle, len, append=False, /)

    Sets the internal buffer size for a value in bytes. This will limit the
    largest possible write that can be received. The default is 20.

    Setting *append* to ``True`` will make all remote writes append to, rather
    than replace, the current value. At most *len* bytes can be buffered in
    this way. When you use :meth:`gatts_read <BLE.gatts_read>`, the value will
    be cleared after reading. This feature is useful when implementing something
    like the Nordic UART Service.

GATT Client
-----------

A GATT client can discover and read/write characteristics on a remote GATT server.

It is more common for a central role device to act as the GATT client, however
it's also possible for a peripheral to act as a client in order to discover
information about the central that has connected to it (e.g. to read the
device name from the device information service).

.. method:: BLE.gattc_discover_services(conn_handle, uuid=None, /)

    Query a connected server for its services.

    Optionally specify a service *uuid* to query for that service only.

    For each service discovered, the ``_IRQ_GATTC_SERVICE_RESULT`` event will
    be raised, followed by ``_IRQ_GATTC_SERVICE_DONE`` on completion.

.. method:: BLE.gattc_discover_characteristics(conn_handle, start_handle, end_handle, uuid=None, /)

    Query a connected server for characteristics in the specified range.

    Optionally specify a characteristic *uuid* to query for that
    characteristic only.

    You can use ``start_handle=1``, ``end_handle=0xffff`` to search for a
    characteristic in any service.

    For each characteristic discovered, the ``_IRQ_GATTC_CHARACTERISTIC_RESULT``
    event will be raised, followed by ``_IRQ_GATTC_CHARACTERISTIC_DONE`` on completion.

.. method:: BLE.gattc_discover_descriptors(conn_handle, start_handle, end_handle, /)

    Query a connected server for descriptors in the specified range.

    For each descriptor discovered, the ``_IRQ_GATTC_DESCRIPTOR_RESULT`` event
    will be raised, followed by ``_IRQ_GATTC_DESCRIPTOR_DONE`` on completion.

.. method:: BLE.gattc_read(conn_handle, value_handle, /)

    Issue a remote read to a connected server for the specified
    characteristic or descriptor handle.

    When a value is available, the ``_IRQ_GATTC_READ_RESULT`` event will be
    raised. Additionally, the ``_IRQ_GATTC_READ_DONE`` will be raised.

.. method:: BLE.gattc_write(conn_handle, value_handle, data, mode=0, /)

    Issue a remote write to a connected server for the specified
    characteristic or descriptor handle.

    The argument *mode* specifies the write behaviour, with the currently
    supported values being:

        * ``mode=0`` (default) is a write-without-response: the write will
          be sent to the remote server but no confirmation will be
          returned, and no event will be raised.
        * ``mode=1`` is a write-with-response: the remote server is
          requested to send a response/acknowledgement that it received the
          data.

    If a response is received from the remote server the
    ``_IRQ_GATTC_WRITE_DONE`` event will be raised.

.. method:: BLE.gattc_exchange_mtu(conn_handle, /)

    Initiate MTU exchange with a connected server, using the preferred MTU
    set using ``BLE.config(mtu=value)``.

    The ``_IRQ_MTU_EXCHANGED`` event will be raised when MTU exchange
    completes.

    **Note:** MTU exchange is typically initiated by the central. When using
    the BlueKitchen stack in the central role, it does not support a remote
    peripheral initiating the MTU exchange. NimBLE works for both roles.


L2CAP connection-oriented-channels
----------------------------------

    This feature allows for socket-like data exchange between two BLE devices.
    Once the devices are connected via GAP, either device can listen for the
    other to connect on a numeric PSM (Protocol/Service Multiplexer).

    **Note:** This is currently only supported when using the NimBLE stack on
    STM32 and Unix (not ESP32). Only one L2CAP channel may be active at a given
    time (i.e. you cannot connect while listening).

    Active L2CAP channels are identified by the connection handle that they were
    established on and a CID (channel ID).

    Connection-oriented channels have built-in credit-based flow control. Unlike
    ATT, where devices negotiate a shared MTU, both the listening and connecting
    devices each set an independent MTU which limits the maximum amount of
    outstanding data that the remote device can send before it is fully consumed
    in :meth:`l2cap_recvinto <BLE.l2cap_recvinto>`.

.. method:: BLE.l2cap_listen(psm, mtu, /)

    Start listening for incoming L2CAP channel requests on the specified *psm*
    with the local MTU set to *mtu*.

    When a remote device initiates a connection, the ``_IRQ_L2CAP_ACCEPT``
    event will be raised, which gives the listening server a chance to reject
    the incoming connection (by returning a non-zero integer).

    Once the connection is accepted, the ``_IRQ_L2CAP_CONNECT`` event will be
    raised, allowing the server to obtain the channel id (CID) and the local and
    remote MTU.

    **Note:** It is not currently possible to stop listening.

.. method:: BLE.l2cap_connect(conn_handle, psm, mtu, /)

    Connect to a listening peer on the specified *psm* with local MTU set to *mtu*.

    On successful connection, the the ``_IRQ_L2CAP_CONNECT`` event will be
    raised, allowing the client to obtain the CID and the local and remote (peer) MTU.

    An unsuccessful connection will raise the ``_IRQ_L2CAP_DISCONNECT`` event
    with a non-zero status.

.. method:: BLE.l2cap_disconnect(conn_handle, cid, /)

    Disconnect an active L2CAP channel with the specified *conn_handle* and
    *cid*.

.. method:: BLE.l2cap_send(conn_handle, cid, buf, /)

    Send the specified *buf* (which must support the buffer protocol) on the
    L2CAP channel identified by *conn_handle* and *cid*.

    The specified buffer cannot be larger than the remote (peer) MTU, and no
    more than twice the size of the local MTU.

    This will return ``False`` if the channel is now "stalled", which means that
    :meth:`l2cap_send <BLE.l2cap_send>` must not be called again until the
    ``_IRQ_L2CAP_SEND_READY`` event is received (which will happen when the
    remote device grants more credits, typically after it has received and
    processed the data).

.. method:: BLE.l2cap_recvinto(conn_handle, cid, buf, /)

    Receive data from the specified *conn_handle* and *cid* into the provided
    *buf* (which must support the buffer protocol, e.g. bytearray or
    memoryview).

    Returns the number of bytes read from the channel.

    If *buf* is None, then returns the number of bytes available.

    **Note:** After receiving the ``_IRQ_L2CAP_RECV`` event, the application should
    continue calling :meth:`l2cap_recvinto <BLE.l2cap_recvinto>` until no more
    bytes are available in the receive buffer (typically up to the size of the
    remote (peer) MTU).

    Until the receive buffer is empty, the remote device will not be granted
    more channel credits and will be unable to send any more data.


Pairing and bonding
-------------------

    Pairing allows a connection to be encrypted and authenticated via exchange
    of secrets (with optional MITM protection via passkey authentication).

    Bonding is the process of storing those secrets into non-volatile storage.
    When bonded, a device is able to resolve a resolvable private address (RPA)
    from another device based on the stored identity resolving key (IRK).
    To support bonding, an application must implement the ``_IRQ_GET_SECRET``
    and ``_IRQ_SET_SECRET`` events.

    **Note:** This is currently only supported when using the NimBLE stack on
    STM32 and Unix (not ESP32).

.. method:: BLE.gap_pair(conn_handle, /)

    Initiate pairing with the remote device.

    Before calling this, ensure that the ``io``, ``mitm``, ``le_secure``, and
    ``bond`` configuration options are set (via :meth:`config<BLE.config>`).

    On successful pairing, the ``_IRQ_ENCRYPTION_UPDATE`` event will be raised.

.. method:: BLE.gap_passkey(conn_handle, action, passkey, /)

    Respond to a ``_IRQ_PASSKEY_ACTION`` event for the specified *conn_handle*
    and *action*.

    The *passkey* is a numeric value and will depend on on the
    *action* (which will depend on what I/O capability has been set):

        * When the *action* is ``_PASSKEY_ACTION_INPUT``, then the application should
          prompt the user to enter the passkey that is shown on the remote device.
        * When the *action* is ``_PASSKEY_ACTION_DISPLAY``, then the application should
          generate a random 6-digit passkey and show it to the user.
        * When the *action* is ``_PASSKEY_ACTION_NUMERIC_COMPARISON``, then the application
          should show the passkey that was provided in the ``_IRQ_PASSKEY_ACTION`` event
          and then respond with either ``0`` (cancel pairing), or ``1`` (accept pairing).


class UUID
----------


Constructor
-----------

.. class:: UUID(value, /)

    Creates a UUID instance with the specified **value**.

    The **value** can be either:

    - A 16-bit integer. e.g. ``0x2908``.
    - A 128-bit UUID string. e.g. ``'6E400001-B5A3-F393-E0A9-E50E24DCCA9E'``.
