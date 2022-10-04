.. currentmodule:: esp32

:mod:`esp32` --- functionality specific to the ESP32
====================================================

.. module:: esp32
    :synopsis: functionality specific to the ESP32

The ``esp32`` module contains functions and classes specifically aimed at
controlling ESP32 modules.


Functions
---------

.. function:: wake_on_touch(wake)

    Configure whether or not a touch will wake the device from sleep.
    *wake* should be a boolean value.

.. function:: wake_on_ulp(wake)

    Configure whether or not the Ultra-Low-Power co-processor can wake the
    device from sleep. *wake* should be a boolean value.

.. function:: wake_on_ext0(pin, level)

    Configure how EXT0 wakes the device from sleep.  *pin* can be ``None``
    or a valid Pin object.  *level* should be ``esp32.WAKEUP_ALL_LOW`` or
    ``esp32.WAKEUP_ANY_HIGH``.

.. function:: wake_on_ext1(pins, level)

    Configure how EXT1 wakes the device from sleep.  *pins* can be ``None``
    or a tuple/list of valid Pin objects.  *level* should be ``esp32.WAKEUP_ALL_LOW``
    or ``esp32.WAKEUP_ANY_HIGH``.

.. function:: gpio_deep_sleep_hold(enable)

    Configure whether non-RTC GPIO pin configuration is retained during
    deep-sleep mode for held pads. *enable* should be a boolean value.

.. function:: raw_temperature()

    Read the raw value of the internal temperature sensor, returning an integer.

.. function:: hall_sensor()

    Read the raw value of the internal Hall sensor, returning an integer.

.. function:: idf_heap_info(capabilities)

    Returns information about the ESP-IDF heap memory regions. One of them contains
    the MicroPython heap and the others are used by ESP-IDF, e.g., for network
    buffers and other data. This data is useful to get a sense of how much memory
    is available to ESP-IDF and the networking stack in particular. It may shed
    some light on situations where ESP-IDF operations fail due to allocation failures.
    The information returned is *not* useful to troubleshoot Python allocation failures,
    use `micropython.mem_info()` instead.

    The capabilities parameter corresponds to ESP-IDF's ``MALLOC_CAP_XXX`` values but the
    two most useful ones are predefined as `esp32.HEAP_DATA` for data heap regions and
    `esp32.HEAP_EXEC` for executable regions as used by the native code emitter.

    The return value is a list of 4-tuples, where each 4-tuple corresponds to one heap
    and contains: the total bytes, the free bytes, the largest free block, and
    the minimum free seen over time.

    Example after booting::

        >>> import esp32; esp32.idf_heap_info(esp32.HEAP_DATA)
        [(240, 0, 0, 0), (7288, 0, 0, 0), (16648, 4, 4, 4), (79912, 35712, 35512, 35108),
         (15072, 15036, 15036, 15036), (113840, 0, 0, 0)]

Flash partitions
----------------

This class gives access to the partitions in the device's flash memory and includes
methods to enable over-the-air (OTA) updates.

.. class:: Partition(id, block_size=4096, /)

    Create an object representing a partition.  *id* can be a string which is the label
    of the partition to retrieve, or one of the constants: ``BOOT`` or ``RUNNING``.
    *block_size* specifies the byte size of an individual block.

.. classmethod:: Partition.find(type=TYPE_APP, subtype=0xff, label=None, block_size=4096)

    Find a partition specified by *type*, *subtype* and *label*.  Returns a
    (possibly empty) list of Partition objects. Note: ``subtype=0xff`` matches any subtype
    and ``label=None`` matches any label.

    *block_size* specifies the byte size of an individual block used by the returned
    objects.

.. method:: Partition.info()

    Returns a 6-tuple ``(type, subtype, addr, size, label, encrypted)``.

.. method:: Partition.readblocks(block_num, buf)
            Partition.readblocks(block_num, buf, offset)
.. method:: Partition.writeblocks(block_num, buf)
            Partition.writeblocks(block_num, buf, offset)
.. method:: Partition.ioctl(cmd, arg)

    These methods implement the simple and :ref:`extended
    <block-device-interface>` block protocol defined by
    :class:`os.AbstractBlockDev`.

.. method:: Partition.set_boot()

    Sets the partition as the boot partition.

.. method:: Partition.get_next_update()

    Gets the next update partition after this one, and returns a new Partition object.
    Typical usage is ``Partition(Partition.RUNNING).get_next_update()``
    which returns the next partition to update given the current running one.

.. classmethod:: Partition.mark_app_valid_cancel_rollback()

    Signals that the current boot is considered successful.
    Calling ``mark_app_valid_cancel_rollback`` is required on the first boot of a new
    partition to avoid an automatic rollback at the next boot.
    This uses the ESP-IDF "app rollback" feature with "CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE"
    and  an ``OSError(-261)`` is raised if called on firmware that doesn't have the
    feature enabled.
    It is OK to call ``mark_app_valid_cancel_rollback`` on every boot and it is not
    necessary when booting firmare that was loaded using esptool.

Constants
~~~~~~~~~

.. data:: Partition.BOOT
          Partition.RUNNING

    Used in the `Partition` constructor to fetch various partitions: ``BOOT`` is the
    partition that will be booted at the next reset and ``RUNNING`` is the currently
    running partition.

.. data:: Partition.TYPE_APP
          Partition.TYPE_DATA

    Used in `Partition.find` to specify the partition type: ``APP`` is for bootable
    firmware partitions (typically labelled ``factory``, ``ota_0``, ``ota_1``), and
    ``DATA`` is for other partitions, e.g. ``nvs``, ``otadata``, ``phy_init``, ``vfs``.

.. data:: HEAP_DATA
          HEAP_EXEC

    Used in `idf_heap_info`.

.. _esp32.RMT:

RMT
---

The RMT (Remote Control) module, specific to the ESP32, was originally designed
to send and receive infrared remote control signals. However, due to a flexible
design and very accurate (as low as 12.5ns) pulse generation, it can also be
used to transmit or receive many other types of digital signals::

    import esp32
    from machine import Pin

    r = esp32.RMT(0, pin=Pin(18), clock_div=8)
    r  # RMT(channel=0, pin=18, source_freq=80000000, clock_div=8, idle_level=0)

    # To apply a carrier frequency to the high output
    r = esp32.RMT(0, pin=Pin(18), clock_div=8, tx_carrier=(38000, 50, 1))

    # The channel resolution is 100ns (1/(source_freq/clock_div)).
    r.write_pulses((1, 20, 2, 40), 0)  # Send 0 for 100ns, 1 for 2000ns, 0 for 200ns, 1 for 4000ns

The input to the RMT module is an 80MHz clock (in the future it may be able to
configure the input clock but, for now, it's fixed). ``clock_div`` *divides*
the clock input which determines the resolution of the RMT channel. The
numbers specified in ``write_pulses`` are multiplied by the resolution to
define the pulses.

``clock_div`` is an 8-bit divider (0-255) and each pulse can be defined by
multiplying the resolution by a 15-bit (0-32,768) number. There are eight
channels (0-7) and each can have a different clock divider.

So, in the example above, the 80MHz clock is divided by 8. Thus the
resolution is (1/(80Mhz/8)) 100ns. Since the ``start`` level is 0 and toggles
with each number, the bitstream is ``0101`` with durations of [100ns, 2000ns,
100ns, 4000ns].

For more details see Espressif's `ESP-IDF RMT documentation.
<https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/peripherals/rmt.html>`_.

.. Warning::
   The current MicroPython RMT implementation lacks some features, most notably
   receiving pulses. RMT should be considered a
   *beta feature* and the interface may change in the future.


.. class:: RMT(channel, *, pin=None, clock_div=8, idle_level=False, tx_carrier=None)

    This class provides access to one of the eight RMT channels. *channel* is
    required and identifies which RMT channel (0-7) will be configured. *pin*,
    also required, configures which Pin is bound to the RMT channel. *clock_div*
    is an 8-bit clock divider that divides the source clock (80MHz) to the RMT
    channel allowing the resolution to be specified. *idle_level* specifies
    what level the output will be when no transmission is in progress and can
    be any value that converts to a boolean, with ``True`` representing high
    voltage and ``False`` representing low.

    To enable the transmission carrier feature, *tx_carrier* should be a tuple
    of three positive integers: carrier frequency, duty percent (``0`` to
    ``100``) and the output level to apply the carrier to (a boolean as per
    *idle_level*).

.. method:: RMT.source_freq()

    Returns the source clock frequency. Currently the source clock is not
    configurable so this will always return 80MHz.

.. method:: RMT.clock_div()

    Return the clock divider. Note that the channel resolution is
    ``1 / (source_freq / clock_div)``.

.. method:: RMT.wait_done(*, timeout=0)

    Returns ``True`` if the channel is idle or ``False`` if a sequence of
    pulses started with `RMT.write_pulses` is being transmitted. If the
    *timeout* keyword argument is given then block for up to this many
    milliseconds for transmission to complete.

.. method:: RMT.loop(enable_loop)

    Configure looping on the channel. *enable_loop* is bool, set to ``True`` to
    enable looping on the *next* call to `RMT.write_pulses`. If called with
    ``False`` while a looping sequence is currently being transmitted then the
    current loop iteration will be completed and then transmission will stop.

.. method:: RMT.write_pulses(duration, data=True)

    Begin transmitting a sequence. There are three ways to specify this:

    **Mode 1:** *duration* is a list or tuple of durations. The optional *data*
    argument specifies the initial output level. The output level will toggle
    after each duration.

    **Mode 2:** *duration* is a positive integer and *data* is a list or tuple
    of output levels. *duration* specifies a fixed duration for each.

    **Mode 3:** *duration* and *data* are lists or tuples of equal length,
    specifying individual durations and the output level for each.

    Durations are in integer units of the channel resolution (as described
    above), between 1 and 32767 units. Output levels are any value that can
    be converted to a boolean, with ``True`` representing high voltage and
    ``False`` representing low.

    If transmission of an earlier sequence is in progress then this method will
    block until that transmission is complete before beginning the new sequence.

    If looping has been enabled with `RMT.loop`, the sequence will be
    repeated indefinitely. Further calls to this method will block until the
    end of the current loop iteration before immediately beginning to loop the
    new sequence of pulses. Looping sequences longer than 126 pulses is not
    supported by the hardware.

.. staticmethod:: RMT.bitstream_channel([value])

    Select which RMT channel is used by the `machine.bitstream` implementation.
    *value* can be ``None`` or a valid RMT channel number.  The default RMT
    channel is the highest numbered one.

    Passing in ``None`` disables the use of RMT and instead selects a bit-banging
    implementation for `machine.bitstream`.

    Passing in no argument will not change the channel.  This function returns
    the current channel number.

Ultra-Low-Power co-processor
----------------------------

.. class:: ULP()

    This class provides access to the Ultra-Low-Power co-processor.

.. method:: ULP.set_wakeup_period(period_index, period_us)

    Set the wake-up period.

.. method:: ULP.load_binary(load_addr, program_binary)

    Load a *program_binary* into the ULP at the given *load_addr*.

.. method:: ULP.run(entry_point)

    Start the ULP running at the given *entry_point*.


Constants
---------

.. data:: esp32.WAKEUP_ALL_LOW
          esp32.WAKEUP_ANY_HIGH

   Selects the wake level for pins.

Non-Volatile Storage
--------------------

This class gives access to the Non-Volatile storage managed by ESP-IDF. The NVS is partitioned
into namespaces and each namespace contains typed key-value pairs. The keys are strings and the
values may be various integer types, strings, and binary blobs. The driver currently only
supports 32-bit signed integers and blobs.

.. warning::

    Changes to NVS need to be committed to flash by calling the commit method. Failure
    to call commit results in changes being lost at the next reset.

.. class:: NVS(namespace)

    Create an object providing access to a namespace (which is automatically created if not
    present).

.. method:: NVS.set_i32(key, value)

    Sets a 32-bit signed integer value for the specified key. Remember to call *commit*!

.. method:: NVS.get_i32(key)

    Returns the signed integer value for the specified key. Raises an OSError if the key does not
    exist or has a different type.

.. method:: NVS.set_blob(key, value)

    Sets a binary blob value for the specified key. The value passed in must support the buffer
    protocol, e.g. bytes, bytearray, str. (Note that esp-idf distinguishes blobs and strings, this
    method always writes a blob even if a string is passed in as value.)
    Remember to call *commit*!

.. method:: NVS.get_blob(key, buffer)

    Reads the value of the blob for the specified key into the buffer, which must be a bytearray.
    Returns the actual length read. Raises an OSError if the key does not exist, has a different
    type, or if the buffer is too small.

.. method:: NVS.erase_key(key)

    Erases a key-value pair.

.. method:: NVS.commit()

    Commits changes made by *set_xxx* methods to flash.

.. _esp32.CAN:

class CAN -- controller area network communication bus
======================================================

CAN implements the standard CAN communications protocol. At
the physical level it consists of 2 lines: RX and TX. Note that
to connect the microcontroller to a CAN bus you must use a CAN transceiver
to convert the CAN logic signals from the microcontroller to the correct
voltage levels on the bus.

Example usage (works without anything connected)::

    from esp32 import CAN
    BAUDRATE_500k = 500000
    can = CAN(0, tx=5, rx=4, mode=CAN.NORMAL, baudrate=BAUDRATE_500k)
    can.setfilter(0, CAN.FILTER_ADDRESS, params=[0x123], extframe=False)  # set a filter to receive messages with id = 0x102
    can.send([1,2,3], 0x102, extframe=False)   # send a message with id 123
    if can.any():
        can.recv()  # receive message


Constructors
------------

.. class:: esp32.CAN(bus, ...)

   Construct a CAN object on the given bus(controller). *bus* must be 0 for ESP32.
   With no additional parameters, the CAN object is created but not
   initialised (it has the settings from the last initialisation of
   the bus, if any). If extra arguments are given, the bus is initialised.
   See :meth:`CAN.init` for parameters of initialisation.

   The physical pins of the CAN bus can be assigned during init.

Methods
-------

.. method:: CAN.init(mode, *, tx=5, rx=4, baudrate=500000, prescaler=8, sjw=3, bs1=15, bs2=4, auto_restart=False, tx_queue=1, rx_queue=1)

   Initialise the CAN bus with the given parameters:

    - *mode* is one of:  NORMAL, LOOPBACK, SILENT, SILENT_LOOPBACK
    - *tx* defines the gpio used for transmission
    - *rx* defines the gpio used for receiving
    - *baudrate* is used to define a standard speed. If it is defined, the *prescaler*, *sjw*, *bs1*, *bs2*
      will be ignored. Standard speeds are 25000, 50000, 100000, 125000, 250000, 500000, 1000000. Some versions
      of esp32 supports non-standard speeds: 1000, 5000, 10000, 12500, 16000, 20000.
    - *prescaler* is used to set the duration of 1 time quanta; the time quanta
      will be the input clock divided by the prescaler
    - *sjw* is the resynchronisation jump width in units of the time quanta;
      it can be 1, 2, 3, 4
    - *bs1* defines the location of the sample point in units of the time quanta;
      it can be between 1 and 1024 inclusive
    - *bs2* defines the location of the transmit point in units of the time quanta;
      it can be between 1 and 16 inclusive
    - *bus_off* defines the gpio used for BUS-OFF signal line(optional)
    - *clkout* defines the gpio used for CLKOUT signal line(optional)
    - *tx_queue* defines the number of waiting tx messages can be stored
    - *rx_queue* defines the number of received messages can be stored
    - *auto_restart* sets whether the controller will automatically try and restart
      communications after entering the bus-off state; if this is disabled then
      :meth:`~CAN.restart()` can be used to leave the bus-off state.
      This parameter is currently not implemented and it must be set to False


.. method:: CAN.deinit()

   Turn off the CAN bus.

.. method:: CAN.restart()

   Force a software restart of the CAN controller without resetting its
   configuration.

   If the controller enters the bus-off state then it will no longer participate
   in bus activity.  If the controller is not configured to automatically restart
   (see :meth:`~CAN.init()`) then this method can be used to trigger a restart,
   and the controller will follow the CAN protocol to leave the bus-off state and
   go into the error active state.

.. method:: CAN.state()

   Return the state of the controller.  The return value can be one of:

   - ``CAN.STOPPED`` -- the controller is completely off and reset;
   - ``CAN.ERROR_ACTIVE`` -- the controller is on and in the Error Active state
     (both TEC and REC are less than 96);
   - ``CAN.BUS_OFF`` -- the controller is on but not participating in bus activity
     (TEC overflowed beyond 255).
   - ``CAN.RECOVERING`` -- the controller is under recover from bus-off state;


.. method:: CAN.info()

   Get information about the controller's error states and TX and RX buffers.
   If *list* is provided then it should be a list object with at least 8 entries,
   which will be filled in with the information.  Otherwise a new list will be
   created and filled in.  In both cases the return value of the method is the
   populated list.

   The values in the list are:

    - TEC value
    - REC value
    - number of times the controller enterted the Error Warning state (wrapped around to 0 after 65535)
    - number of times the controller enterted the Error Passive state (wrapped around to 0 after 65535)
    - number of times the controller enterted the Bus Off state (wrapped around to 0 after 65535)
    - number of pending TX messages
    - number of pending RX messages


.. method:: CAN.setfilter(bank, mode, params, *, rtr=False, extframe=False)

   Configure a filter bank:

   - *bank* is the filter bank that is to be configured (esp32 supports only 0 bank)
   - *mode* is the mode the filter should operate in.
   - *params* is an array of values the defines the filter.
     The contents of the array depends on the *mode* and *extframe* arguments.

   +-----------------------+----------------------------------------------------------------------------+
   | *mode*                | contents of *params* array                                                 |
   +=======================+============================================================================+
   | CAN.FILTER_RAW_SINGLE | *params* will be copied in hardware variable                               |
   |                       | and single_filter_mode will be selected                                    |
   |                       | In this mode, *bank* will be ignored                                       |
   +-----------------------+----------------------------------------------------------------------------+
   | CAN.FILTER_RAW_DUAL   | *params* will be copied in hardware variable                               |
   |                       | and single_filter_mode will be cleared                                     |
   |                       | In this mode, *bank* will be ignored                                       |
   +-----------------------+----------------------------------------------------------------------------+
   | CAN.FILTER_ADDRESS    | *params* could be:                                                         |
   |                       |                                                                            |
   |                       | If ``extframe=True`` and *params* length of 1 -- filter 29 bit identifier  |
   |                       | of message.                                                                |
   |                       |                                                                            |
   |                       | if ``extframe=False``:                                                     |
   |                       |                                                                            |
   |                       |  * length of 1 filter 11 bit identifier of message                         |
   |                       |  * length of 2 filter 11 bit identifier and first byte of message          |
   |                       |  * length of 3 filter 11 bit identifier first and second bytes of message  |
   +-----------------------+----------------------------------------------------------------------------+

   - *rtr* For classic CAN controllers, this is an array of booleans that states if
     a filter should accept a remote transmission request message. If this argument
     is not given then it defaults to ``False`` for all entries.

.. method:: CAN.clearfilter(bank)

   Clear and disables all filters

.. method:: CAN.any(fifo)

   Return ``True`` if any message waiting on the FIFO, else ``False``.

.. method:: CAN.recv(list=None, *, timeout=5000)

   Receive data on the bus:

     - *list* is an optional list object to be used as the return value
     - *timeout* is the timeout in milliseconds to wait for the receive.

   Return value: A tuple containing four values.

     - The id of the message.
     - A boolean that indicates if the message is an RTR message.
     - Reserved.
     - An array containing the data.

   If *list* is ``None`` then a new tuple will be allocated, as well as a new
   bytes object to contain the data (as the fourth element in the tuple).

   If *list* is not ``None`` then it should be a list object with at least four
   elements.  The fourth element should be a memoryview object which is created
   from either a bytearray or an array of type 'B' or 'b', and this array must
   have enough room for at least 8 bytes.  The list object will then be
   populated with the first three return values above, and the memoryview object
   will be resized inplace to the size of the data and filled in with that data.
   The same list and memoryview objects can be reused in subsequent calls to
   this method, providing a way of receiving data without using the heap.
   For example::

        buf = bytearray(8)
        lst = [0, 0, 0, memoryview(buf)]
        # No heap memory is allocated in the following call
        can.recv(lst, timeout=0)

*list* values are:

- identifier of can packet (int)
- extended packet (bool)
- rtr packet (bool)
- data frame (0..8 bytes)


.. method:: CAN.send(data, id, *, timeout=0, rtr=False, extframe=false)

   Send a message on the bus:

     - *data* is the data to send (an integer to send, or a buffer object).
     - *id* is the id of the message to be sent.
     - *timeout* is the timeout in milliseconds to wait for the send.
     - *rtr* is a boolean that specifies if the message shall be sent as
       a remote transmission request.  If *rtr* is True then only the length
       of *data* is used to fill in the DLC slot of the frame; the actual
       bytes in *data* are unused.

     If timeout is 0 the message is placed in a buffer and the method returns
     immediately. If all three buffers are in use an exception is thrown.
     If timeout is not 0, the method waits until the message is transmitted.
     If the message can't be transmitted within the specified time an exception
     is thrown.

   Return value: ``None``.

.. method:: CAN.clear_tx_queue()

  Clear all messages from transmitting queue.

.. method:: CAN.clear_rx_queue()

  Clear all messages from receiving queue.


Constants
---------

.. data:: CAN.NORMAL
          CAN.LOOPBACK
          CAN.SILENT
          CAN.SILENT_LOOPBACK


   The mode of the CAN bus used in :meth:`~CAN.init()`.

   +---------------------+---------------------------------------------+-------+-------+
   | *mode*              | \                                           | STM32 | ESP32 |
   +=====================+=============================================+=======+=======+
   | CAN.NORMAL          | .. image:: img/can_mode_normal.png          |   +   |   +   |
   +---------------------+---------------------------------------------+-------+-------+
   | CAN.LOOPBACK        | .. image:: img/can_mode_loopback.png        |   +   |   +   |
   +---------------------+---------------------------------------------+-------+-------+
   | CAN.SILENT          | .. image:: img/can_mode_silent.png          |   +   |   +   |
   +---------------------+---------------------------------------------+-------+-------+
   | CAN.SILENT_LOOPBACK | .. image:: img/can_mode_silent_loopback.png |   +   |   +   |
   +---------------------+---------------------------------------------+-------+-------+


.. data:: CAN.STOPPED
          CAN.ERROR_ACTIVE
          CAN.BUS_OFF
          CAN.RECOVERING

   Possible states of the CAN controller returned from :meth:`~CAN.state()`.

.. data:: CAN.FILTER_RAW_SINGLE
          CAN.FILTER_RAW_DUAL
          CAN.FILTER_ADDRESS

   The operation mode of a filter used in :meth:`~CAN.setfilter()`.
