:mod:`ubluetooth` --- low-level Bluetooth
=========================================

.. module:: ubluetooth
   :synopsis: Low-level Bluetooth radio functionality

This module provides an interface to a Bluetooth controller on a board.
Currently this supports Bluetooth Low Energy (BLE) in Central, Peripheral,
Broadcaster, and Observer roles, and a device may operate in multiple
roles concurrently.

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

.. method:: BLE.active([active])

    Optionally changes the active state of the BLE radio, and returns the
    current state.

    The radio must be made active before using any other methods on this class.

.. method:: BLE.config('param')
            BLE.config(param=value, ...)

    Get or set configuration values of the BLE interface.  To get a value the
    parameter name should be quoted as a string, and just one parameter is
    queried at a time.  To set values use the keyword syntax, and one ore more
    parameter can be set at a time.

    Currently supported values are:

    - ``'mac'``: Returns the device MAC address. If a device has a fixed address
      (e.g. PYBD) then it will be returned. Otherwise (e.g. ESP32) a random
      address will be generated when the BLE interface is made active.
      Note: on some ports, accessing this value requires that the interface is
      active (so that the MAC address can be queried from the controller).

    - ``'gap_name'``: Get/set the GAP device name used by service 0x1800,
      characteristic 0x2a00.  This can be set at any time and changed multiple
      times.

    - ``'rxbuf'``: Get/set the size in bytes of the internal buffer used to store
      incoming events.  This buffer is global to the entire BLE driver and so
      handles incoming data for all events, including all characteristics.
      Increasing this allows better handling of bursty incoming data (for
      example scan results) and the ability for a central role to receive
      larger characteristic values.

Event Handling
--------------

.. method:: BLE.irq(handler, trigger=0xffff)

    Registers a callback for events from the BLE stack. The *handler* takes two
    arguments, ``event`` (which will be one of the codes below) and ``data``
    (which is an event-specific tuple of values).

    The optional *trigger* parameter allows you to set a mask of events that
    your program is interested in. The default is all events.

    Note: the ``addr``, ``adv_data``, ``char_data``, ``notify_data``, and
    ``uuid`` entries in the tuples are
    references to data managed by the :mod:`ubluetooth` module (i.e. the same
    instance will be re-used across multiple calls to the event handler). If
    your program wants to use this data outside of the handler, then it must
    copy them first, e.g. by using ``bytes(addr)`` or ``bluetooth.UUID(uuid)``.

    An event handler showing all possible events::

        def bt_irq(event, data):
            if event == _IRQ_CENTRAL_CONNECT:
                # A central has connected to this peripheral.
                conn_handle, addr_type, addr = data
            elif event == _IRQ_CENTRAL_DISCONNECT:
                # A central has disconnected from this peripheral.
                conn_handle, addr_type, addr = data
            elif event == _IRQ_GATTS_WRITE:
                # A central has written to this characteristic or descriptor.
                conn_handle, attr_handle = data
            elif event == _IRQ_GATTS_READ_REQUEST:
                # A central has issued a read. Note: this is a hard IRQ.
                # Return None to deny the read.
                # Note: This event is not supported on ESP32.
                conn_handle, attr_handle = data
            elif event == _IRQ_SCAN_RESULT:
                # A single scan result.
                addr_type, addr, adv_type, rssi, adv_data = data
            elif event == _IRQ_SCAN_COMPLETE:
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
            elif event == _IRQ_GATTC_CHARACTERISTIC_RESULT:
                # Called for each characteristic found by gattc_discover_services().
                conn_handle, def_handle, value_handle, properties, uuid = data
            elif event == _IRQ_GATTC_DESCRIPTOR_RESULT:
                # Called for each descriptor found by gattc_discover_descriptors().
                conn_handle, dsc_handle, uuid = data
            elif event == _IRQ_GATTC_READ_RESULT:
                # A gattc_read() has completed.
                conn_handle, value_handle, char_data = data
            elif event == _IRQ_GATTC_WRITE_STATUS:
                # A gattc_write() has completed.
                conn_handle, value_handle, status = data
            elif event == _IRQ_GATTC_NOTIFY:
                # A peripheral has sent a notify request.
                conn_handle, value_handle, notify_data = data
            elif event == _IRQ_GATTC_INDICATE:
                # A peripheral has sent an indicate request.
                conn_handle, value_handle, notify_data = data

The event codes are::

    from micropython import const
    _IRQ_CENTRAL_CONNECT                 = const(1 << 0)
    _IRQ_CENTRAL_DISCONNECT              = const(1 << 1)
    _IRQ_GATTS_WRITE                     = const(1 << 2)
    _IRQ_GATTS_READ_REQUEST              = const(1 << 3)
    _IRQ_SCAN_RESULT                     = const(1 << 4)
    _IRQ_SCAN_COMPLETE                   = const(1 << 5)
    _IRQ_PERIPHERAL_CONNECT              = const(1 << 6)
    _IRQ_PERIPHERAL_DISCONNECT           = const(1 << 7)
    _IRQ_GATTC_SERVICE_RESULT            = const(1 << 8)
    _IRQ_GATTC_CHARACTERISTIC_RESULT     = const(1 << 9)
    _IRQ_GATTC_DESCRIPTOR_RESULT         = const(1 << 10)
    _IRQ_GATTC_READ_RESULT               = const(1 << 11)
    _IRQ_GATTC_WRITE_STATUS              = const(1 << 12)
    _IRQ_GATTC_NOTIFY                    = const(1 << 13)
    _IRQ_GATTC_INDICATE                  = const(1 << 14)

In order to save space in the firmware, these constants are not included on the
:mod:`ubluetooth` module. Add the ones that you need from the list above to your
program.


Broadcaster Role (Advertiser)
-----------------------------

.. method:: BLE.gap_advertise(interval_us, adv_data=None, resp_data=None, connectable=True)

    Starts advertising at the specified interval (in **micro**\ seconds). This
    interval will be rounded down to the nearest 625us. To stop advertising, set
    *interval_us* to ``None``.

    *adv_data* and *resp_data* can be any type that implements the buffer
    protocol (e.g. ``bytes``, ``bytearray``, ``str``). *adv_data* is included
    in all broadcasts, and *resp_data* is send in reply to an active scan.

    Note: if *adv_data* (or *resp_data*) is ``None``, then the data passed
    to the previous call to ``gap_advertise`` will be re-used. This allows a
    broadcaster to resume advertising with just ``gap_advertise(interval_us)``.
    To clear the advertising payload pass an empty ``bytes``, i.e. ``b''``.


Observer Role (Scanner)
-----------------------

.. method:: BLE.gap_scan(duration_ms, [interval_us], [window_us])

    Run a scan operation lasting for the specified duration (in **milli**\ seconds).

    To scan indefinitely, set *duration_ms* to ``0``.

    To stop scanning, set *duration_ms* to ``None``.

    Use *interval_us* and *window_us* to optionally configure the duty cycle.
    The scanner will run for *window_us* **micro**\ seconds every *interval_us*
    **micro**\ seconds for a total of *duration_ms* **milli**\ seconds. The default
    interval and window are 1.28 seconds and 11.25 milliseconds respectively
    (background scanning).

    For each scan result the ``_IRQ_SCAN_RESULT`` event will be raised, with event
    data ``(addr_type, addr, adv_type, rssi, adv_data)``.  ``adv_type`` values correspond
    to the Bluetooth Specification:

        * 0x00 - ADV_IND - connectable and scannable undirected advertising
        * 0x01 - ADV_DIRECT_IND - connectable directed advertising
        * 0x02 - ADV_SCAN_IND - scannable undirected advertising
        * 0x03 - ADV_NONCONN_IND - non-connectable undirected advertising
        * 0x04 - SCAN_RSP - scan response

    When scanning is stopped (either due to the duration finishing or when
    explicitly stopped), the ``_IRQ_SCAN_COMPLETE`` event will be raised.


Peripheral Role (GATT Server)
-----------------------------

A BLE peripheral has a set of registered services. Each service may contain
characteristics, which each have a value. Characteristics can also contain
descriptors, which themselves have values.

These values are stored locally, and are accessed by their "value handle" which
is generated during service registration. They can also be read from or written
to by a remote central device. Additionally, a peripheral can "notify" a
characteristic to a connected central via a connection handle.

Characteristics and descriptors have a default maximum size of 20 bytes.
Anything written to them by a central will be truncated to this length. However,
any local write will increase the maximum size, so if you want to allow larger
writes from a central to a given characteristic, use
:meth:`gatts_write<BLE.gatts_write>` after registration. e.g.
``gatts_write(char_handle, bytes(100))``.

.. method:: BLE.gatts_register_services(services_definition)

    Configures the peripheral with the specified services, replacing any
    existing services.

    *services_definition* is a list of **services**, where each **service** is a
    two-element tuple containing a UUID and a list of **characteristics**.

    Each **characteristic** is a two-or-three-element tuple containing a UUID, a
    **flags** value, and optionally a list of *descriptors*.

    Each **descriptor** is a two-element tuple containing a UUID and a **flags**
    value.

    The **flags** are a bitwise-OR combination of the
    :data:`ubluetooth.FLAG_READ`, :data:`ubluetooth.FLAG_WRITE` and
    :data:`ubluetooth.FLAG_NOTIFY` values defined below.

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
    :meth:`gatts_read <BLE.gatts_read>`, :meth:`gatts_write <BLE.gatts_write>`,
    and :meth:`gatts_notify <BLE.gatts_notify>`.

    **Note:** Advertising must be stopped before registering services.

.. method:: BLE.gatts_read(value_handle)

    Reads the local value for this handle (which has either been written by
    :meth:`gatts_write <BLE.gatts_write>` or by a remote central).

.. method:: BLE.gatts_write(value_handle, data)

    Writes the local value for this handle, which can be read by a central.

.. method:: BLE.gatts_notify(conn_handle, value_handle, [data])

    Notifies a connected central that this value has changed and that it should
    issue a read of the current value from this peripheral.

    If *data* is specified, then the that value is sent to the central as part
    of the notification, avoiding the need for a separate read request. Note
    that this will not update the local value stored.

.. method:: BLE.gatts_set_buffer(value_handle, len, append=False, /)

    Sets the internal buffer size for a value in bytes. This will limit the
    largest possible write that can be received. The default is 20.

    Setting *append* to ``True`` will make all remote writes append to, rather
    than replace, the current value. At most *len* bytes can be buffered in
    this way. When you use :meth:`gatts_read <BLE.gatts_read>`, the value will
    be cleared after reading. This feature is useful when implementing something
    like the Nordic UART Service.


Central Role (GATT Client)
--------------------------

.. method:: BLE.gap_connect(addr_type, addr, scan_duration_ms=2000, /)

    Connect to a peripheral.

    On success, the ``_IRQ_PERIPHERAL_CONNECT`` event will be raised.

.. method:: BLE.gap_disconnect(conn_handle)

    Disconnect the specified connection handle.

    On success, the ``_IRQ_PERIPHERAL_DISCONNECT`` event will be raised.

    Returns ``False`` if the connection handle wasn't connected, and ``True``
    otherwise.

.. method:: BLE.gattc_discover_services(conn_handle)

    Query a connected peripheral for its services.

    For each service discovered, the ``_IRQ_GATTC_SERVICE_RESULT`` event will be
    raised.

.. method:: BLE.gattc_discover_characteristics(conn_handle, start_handle, end_handle)

    Query a connected peripheral for characteristics in the specified range.

    For each characteristic discovered, the ``_IRQ_GATTC_CHARACTERISTIC_RESULT``
    event will be raised.

.. method:: BLE.gattc_discover_descriptors(conn_handle, start_handle, end_handle)

    Query a connected peripheral for descriptors in the specified range.

    For each descriptor discovered, the ``_IRQ_GATTC_DESCRIPTOR_RESULT`` event
    will be raised.

.. method:: BLE.gattc_read(conn_handle, value_handle)

    Issue a remote read to a connected peripheral for the specified
    characteristic or descriptor handle.

    On success, the ``_IRQ_GATTC_READ_RESULT`` event will be raised.

.. method:: BLE.gattc_write(conn_handle, value_handle, data, mode=0, /)

    Issue a remote write to a connected peripheral for the specified
    characteristic or descriptor handle.

    The argument *mode* specifies the write behaviour, with the currently
    supported values being:

        * ``mode=0`` (default) is a write-without-response: the write will
          be sent to the remote peripheral but no confirmation will be
          returned, and no event will be raised.
        * ``mode=1`` is a write-with-response: the remote peripheral is
          requested to send a response/acknowledgement that it received the
          data.

    If a response is received from the remote peripheral the
    ``_IRQ_GATTC_WRITE_STATUS`` event will be raised.


class UUID
----------


Constructor
-----------

.. class:: UUID(value)

    Creates a UUID instance with the specified **value**.

    The **value** can be either:

    - A 16-bit integer. e.g. ``0x2908``.
    - A 128-bit UUID string. e.g. ``'6E400001-B5A3-F393-E0A9-E50E24DCCA9E'``.


Constants
---------

.. data:: ubluetooth.FLAG_READ
          ubluetooth.FLAG_WRITE
          ubluetooth.FLAG_NOTIFY
