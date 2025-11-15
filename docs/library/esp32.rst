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

    .. note:: This is only available for boards that have touch sensor support.

.. function:: wake_on_ulp(wake)

    Configure whether or not the Ultra-Low-Power co-processor can wake the
    device from sleep. *wake* should be a boolean value.

    .. note:: This is only available for boards that have ULP coprocessor support.

.. function:: wake_on_ext0(pin, level)

    Configure how EXT0 wakes the device from sleep.  *pin* can be ``None``
    or a valid Pin object.  *level* should be ``esp32.WAKEUP_ALL_LOW`` or
    ``esp32.WAKEUP_ANY_HIGH``.

    .. note:: This is only available for boards that have ext0 support.

.. function:: wake_on_ext1(pins, level)

    Configure how EXT1 wakes the device from sleep.  *pins* can be ``None``
    or a tuple/list of valid Pin objects.  *level* should be ``esp32.WAKEUP_ALL_LOW``
    or ``esp32.WAKEUP_ANY_HIGH``.

    .. note:: This is only available for boards that have ext1 support.

.. function:: wake_on_gpio(pins, level)

    Configure how GPIO wakes the device from sleep.  *pins* can be ``None``
    or a tuple/list of valid Pin objects.  *level* should be ``esp32.WAKEUP_ALL_LOW``
    or ``esp32.WAKEUP_ANY_HIGH``.

    .. note:: Some boards don't support waking on GPIO from deep sleep,
       on those boards, the pins set here can only be used to wake from light sleep.

.. function:: gpio_deep_sleep_hold(enable)

    Configure whether non-RTC GPIO pin configuration is retained during
    deep-sleep mode for held pads. *enable* should be a boolean value.

.. function:: raw_temperature()

    Read the raw value of the internal temperature sensor, returning an integer.

.. function:: idf_heap_info(capabilities)

    Returns information about the ESP-IDF heap memory regions. One of them contains
    the MicroPython heap and the others are used by ESP-IDF, e.g., for network
    buffers and other data. This data is useful to get a sense of how much memory
    is available to ESP-IDF and the networking stack in particular. It may shed
    some light on situations where ESP-IDF operations fail due to allocation failures.

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

    .. note:: Free IDF heap memory in the `esp32.HEAP_DATA` region is available
       to be automatically added to the MicroPython heap to prevent a
       MicroPython allocation from failing. However, the information returned
       here is otherwise *not* useful to troubleshoot Python allocation
       failures. :func:`micropython.mem_info()` and :func:`gc.mem_free()` should
       be used instead:

       The "max new split" value in :func:`micropython.mem_info()` output
       corresponds to the largest free block of ESP-IDF heap that could be
       automatically added on demand to the MicroPython heap.

       The result of :func:`gc.mem_free()` is the total of the current "free"
       and "max new split" values printed by :func:`micropython.mem_info()`.

.. function:: idf_task_info()

    Returns information about running ESP-IDF/FreeRTOS tasks, which include
    MicroPython threads. This data is useful to gain insight into how much time
    tasks spend running or if they are blocked for significant parts of time,
    and to determine if allocated stacks are fully utilized or might be reduced.

    ``CONFIG_FREERTOS_USE_TRACE_FACILITY=y`` must be set in the board
    configuration to make this method available. Additionally configuring
    ``CONFIG_FREERTOS_GENERATE_RUN_TIME_STATS=y`` and
    ``CONFIG_FREERTOS_VTASKLIST_INCLUDE_COREID=y`` is recommended to be able to
    retrieve the total and per-task runtime and the core ID respectively.

    The return value is a 2-tuple where the first value is the total runtime,
    and the second a list of tasks. Each task is a 7-tuple containing: the task
    ID, name, current state, priority, runtime, stack high water mark, and the
    ID of the core it is running on. Runtime and core ID will be None when the
    respective FreeRTOS configuration option is not enabled.

    .. note:: For an easier to use output based on this function you can use the
       `utop library <https://github.com/micropython/micropython-lib/tree/master/micropython/utop>`_,
       which implements a live overview similar to the Unix ``top`` command.


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
    :class:`vfs.AbstractBlockDev`.

.. method:: Partition.set_boot()

    Sets the partition as the boot partition.

    .. note:: Do not enter :func:`deepsleep<machine.deepsleep>` after changing
       the OTA boot partition, without first performing a hard
       :func:`reset<machine.reset>` or power cycle. This ensures the bootloader
       will validate the new image before booting.

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
    necessary when booting firmware that was loaded using esptool.

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


.. _esp32.PCNT:

PCNT
----

This class provides access to the ESP32 hardware support for pulse counting.
There are 8 pulse counter units, with id 0..7.

See the :ref:`machine.Counter <machine.Counter>` and
:ref:`machine.Encoder <machine.Encoder>` classes for simpler and portable
abstractions of common pulse counting applications. These classes are
implemented as thin Python shims around :class:`PCNT`.

.. class:: PCNT(id, *, ...)

    Returns the singleton PCNT instance for the given unit ``id``.

    Keyword arguments are passed to the ``init()`` method as described
    below.

.. method:: PCNT.init(*, ...)

    (Re-)initialise a pulse counter unit. Supported keyword arguments are:

      - ``channel``: see description below
      - ``pin``: the input Pin to monitor for pulses
      - ``rising``: an action to take on a rising edge - one of
        ``PCNT.INCREMENT``, ``PCNT.DECREMENT`` or ``PCNT.IGNORE`` (the default)
      - ``falling``: an action to take on a falling edge (takes the save values
        as the ``rising`` argument).
      - ``mode_pin``: ESP32 pulse counters support monitoring a second pin and
        altering the behaviour of the counter based on its level - set this
        keyword to any input Pin
      - ``mode_low``: set to either ``PCNT.HOLD`` or ``PCNT.REVERSE`` to
        either suspend counting or reverse the direction of the counter (i.e.,
        ``PCNT.INCREMENT`` behaves as ``PCNT.DECREMENT`` and vice versa)
        when ``mode_pin`` is low
      - ``mode_high``: as ``mode_low`` but for the behaviour when ``mode_pin``
        is high
      - ``filter``: set to a value 1..1023, in ticks of the 80MHz clock, to
        enable the pulse width filter
      - ``min``: set to the minimum level of the counter value when
        decrementing (-32768..-1) or 0 to disable
      - ``max``: set to the maximum level of the counter value when
        incrementing (1..32767) or 0 to disable
      - ``threshold0``: sets the counter value for the
        ``PCNT.IRQ_THRESHOLD0`` event (see ``irq`` method)
      - ``threshold1``: sets the counter value for the
        ``PCNT.IRQ_THRESHOLD1`` event (see ``irq`` method)
      - ``value``: can be set to ``0`` to reset the counter value

    The hardware initialisation is done in stages and so some of the keyword
    arguments can be used in groups or in isolation to partially reconfigure a
    unit:

      - the ``pin`` keyword (optionally combined with ``mode_pin``) can be used
        to change just the bound pin(s)
      - ``rising``, ``falling``, ``mode_low`` and ``mode_high`` can be used
        (singly or together) to change the counting logic - omitted keywords
        use their default (``PCNT.IGNORE`` or ``PCNT.NORMAL``)
      - ``filter`` can be used to change only the pulse width filter (with 0
        disabling it)
      - each of ``min``, ``max``, ``threshold0`` and ``threshold1`` can
        be used to change these limit/event values individually; however,
        setting any will reset the counter to zero (i.e., they imply
        ``value=0``)

    Each pulse counter unit supports two channels, 0 and 1, each able to
    monitor different pins with different counting logic but updating the same
    counter value. Use ``channel=1`` with the ``pin``, ``rising``, ``falling``,
    ``mode_pin``, ``mode_low`` and ``mode_high`` keywords to configure the
    second channel.

    The second channel can be used to configure 4X quadrature decoding with a
    single counter unit::

        pin_a = Pin(2, Pin.INPUT, pull=Pin.PULL_UP)
        pin_b = Pin(3, Pin.INPUT, pull=Pin.PULL_UP)
        rotary = PCNT(0, min=-32000, max=32000)
        rotary.init(channel=0, pin=pin_a, falling=PCNT.INCREMENT, rising=PCNT.DECREMENT, mode_pin=pin_b, mode_low=PCNT.REVERSE)
        rotary.init(channel=1, pin=pin_b, falling=PCNT.DECREMENT, rising=PCNT.INCREMENT, mode_pin=pin_a, mode_low=PCNT.REVERSE)
        rotary.start()

.. method:: PCNT.value([value])

    Call this method with no arguments to return the current counter value.

    If the optional *value* argument is set to ``0`` then the counter is
    reset (but the previous value is returned). Read and reset is not atomic and
    so it is possible for a pulse to be missed. Any value other than ``0`` will
    raise an error.

.. method:: PCNT.irq(handler=None, trigger=PCNT.IRQ_ZERO)

    ESP32 pulse counters support interrupts on these counter events:

      - ``PCNT.IRQ_ZERO``: the counter has reset to zero
      - ``PCNT.IRQ_MIN``: the counter has hit the ``min`` value
      - ``PCNT.IRQ_MAX``: the counter has hit the ``max`` value
      - ``PCNT.IRQ_THRESHOLD0``: the counter has hit the ``threshold0`` value
      - ``PCNT.IRQ_THRESHOLD1``: the counter has hit the ``threshold1`` value

    ``trigger`` should be a bit-mask of the desired events OR'ed together. The
    ``handler`` function should take a single argument which is the
    :class:`PCNT` instance that raised the event.

    This method returns a callback object. The callback object can be used to
    access the bit-mask of events that are outstanding on the PCNT unit.::

        def pcnt_irq(pcnt):
            flags = pcnt.irq().flags()
            if flags & PCNT.IRQ_ZERO:
               # reset
            if flags & PCNT.IRQ_MAX:
               # overflow...
            ... etc

        pcnt.irq(handler=pcnt_irq, trigger=PCNT.IRQ_ZERO | PCNT.IRQ_MAX | ...)

    **Note:** Accessing ``irq.flags()`` will clear the flags, so only call it
    once per invocation of the handler.

    The handler is called with the MicroPython scheduler and so will run at a
    point after the interrupt. If another interrupt occurs before the handler
    has been called then the events will be coalesced together into a single
    call and the bit mask will indicate all events that have occurred.

    To avoid race conditions between a handler being called and retrieving the
    current counter value, the ``value()`` method will force execution of any
    pending events before returning the current counter value (and potentially
    resetting the value).

    Only one handler can be in place per-unit. Set ``handler`` to ``None`` to
    disable the event interrupt.

.. Note::
    ESP32 pulse counters reset to *zero* when reaching the minimum or maximum
    value. Thus the ``IRQ_ZERO`` event will also trigger when either of these
    events occurs.

See the :ref:`machine.Counter <machine.Counter>` and
:ref:`machine.Encoder <machine.Encoder>` classes for simpler abstractions of
common pulse counting applications.

.. _esp32.RMT:

RMT
---

The RMT (Remote Control) module, specific to the ESP32, was originally designed
to send and receive infrared remote control signals. However, due to a flexible
design and very accurate (as low as 12.5ns) pulse generation, it can also be
used to transmit or receive many other types of digital signals::

    import esp32
    from machine import Pin

    r = esp32.RMT(pin=Pin(18), resolution_hz=10000000)
    r  # RMT(pin=18, source_freq=80000000, resolution_hz=10000000, idle_level=0)

    # To apply a carrier frequency to the high output
    r = esp32.RMT(pin=Pin(18), resolution_hz=10000000, tx_carrier=(38000, 50, 1))

    # The channel resolution is 100ns (1/resolution_hz)
    r.write_pulses((1, 20, 2, 40), 0)  # Send 0 for 100ns, 1 for 2000ns, 0 for 200ns, 1 for 4000ns

The input to the RMT module is an 80MHz clock (in the future it may be able to
configure the input clock but, for now, it's fixed). ``resolution_hz`` determines
the resolution of the RMT channel. The numbers specified in ``write_pulses`` are
multiplied by the resolution to
define the pulses.

So, in the example above, the resolution is resolution is (1/10Mhz) = 100ns.
Since the ``start`` level is 0 and toggles with each number, the bitstream is
``0101`` with durations of [100ns, 2000ns, 100ns, 4000ns].

For more details see Espressif's `ESP-IDF RMT documentation.
<https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/peripherals/rmt.html>`_.

.. Warning::
   The current MicroPython RMT implementation lacks some features, most notably
   receiving pulses. RMT should be considered a
   *beta feature* and the interface may change in the future.


.. class:: RMT(channel, *, pin=None, resolution_hz=10000000, clock_div=None, idle_level=False, num_symbols=48|64, tx_carrier=None)

    This class provides access to one of the eight RMT channels. *channel* is
    optional and a dummy parameter for backward compatibility. *pin* is required
    and configures which Pin is bound to the RMT channel.
    *resolution_hz* defines the resolution/unit of the samples.
    For example, 1,000,000 means the unit is microsecond. The pulse widths can
    assume values up to *RMT.PULSE_MAX*, so the resolution should be selected
    accordingly to the signal to be transmitted.
    *clock_div* (deprecated) is equivalent to *resolution_hz*, but expressed as
    a clock divider that divides the source clock (80MHz) to the RMT
    channel allowing the resolution to be specified. Either *clock_div* and
    *resolution_hz* may be supplied, but not both.
    *num_symbols* specifies the
    RMT buffer allocated for this channel (minimum 48 or 64, depending on chip), from a small pool of
    symbols (192 to 512, depending on chip) that are shared by all channels. This buffer does not limit the
    size of the pulse train that you can send, but bigger buffers reduce the
    CPU load and the potential of glitches/imprecise pulse lengths. *idle_level* specifies
    what level the output will be when no transmission is in progress and can
    be any value that converts to a boolean, with ``True`` representing high
    voltage and ``False`` representing low.

    To enable the transmission carrier feature, *tx_carrier* should be a tuple
    of three positive integers: carrier frequency, duty percent (``0`` to
    ``100``) and the output level to apply the carrier to (a boolean as per
    *idle_level*).

.. classmethod:: RMT.source_freq()

    Returns the source clock frequency. Currently the source clock is not
    configurable so this will always return 80MHz.

.. method:: RMT.clock_div()

    Return the clock divider. Note that the channel resolution is
    ``1 / (source_freq / clock_div)``. (Method deprecated. The value may
    not be faithful if resolution was supplied as *resolution_hz*.)

.. method:: RMT.wait_done(*, timeout=0)

    Returns ``True`` if the channel is idle or ``False`` if a sequence of
    pulses started with `RMT.write_pulses` is being transmitted. If the
    *timeout* keyword argument is given then block for up to this many
    milliseconds for transmission to complete. Timeout of -1 blocks until
    transmission is complete (and blocks forever if loop is enabled).

.. method:: RMT.loop(enable_loop)

    Configure looping on the channel. *enable_loop* is bool, set to ``True`` to
    enable looping on the *next* call to `RMT.write_pulses`. If called with
    ``False`` while a looping sequence is currently being transmitted then the
    transmission will stop. (Method deprecated by `RMT.loop_count`.)

.. method:: RMT.loop_count(n)

    Configure looping on the channel. *n* is int. Affects the *next* call to
    `RMT.write_pulses`. Set to ``0`` to disable looping, ``-1`` to enable
    infinite looping, or a positive number to loop for a given number of times.
    If *n* is changed, the current transmission is stopped.

    Note: looping for a finite number of times is not supported by all flavors
    of ESP32.

.. method:: RMT.active([boolean])

    If called without parameters, returns *True* if there is an ongoing transmission.

    If called with parameter *False*, stops the ongoing transmission.
    This is useful to stop an infinite transmission loop.
    The current loop is finished and transmission stops.
    The object is not invalidated, and the RMT channel is again enabled when a new
    transmission is started.

    Calling with parameter *True* does not restart transmission. A new transmission
    should always be initiated by *write_pulses()*.

.. method:: RMT.deinit()

    Release all RMT resources and invalidate the object. All subsequent method
    calls will raise OSError. Useful to free RMT resources without having to wait
    for the object to be garbage-collected.

.. method:: RMT.write_pulses(duration, data=True)

    Begin transmitting a sequence. There are three ways to specify this:

    **Mode 1:** *duration* is a list or tuple of durations. The optional *data*
    argument specifies the initial output level. The output level will toggle
    after each duration.

    **Mode 2:** *duration* is a positive integer and *data* is a list or tuple
    of output levels. *duration* specifies a fixed duration for each.

    **Mode 3:** *duration* and *data* are lists or tuples of equal length,
    specifying individual durations and the output level for each.

    Durations are in integer units of the channel resolution (as
    described above), between 1 and ``PULSE_MAX`` units. Output levels
    are any value that can be converted to a boolean, with ``True``
    representing high voltage and ``False`` representing low.

    If transmission of an earlier sequence is in progress then this method will
    block until that transmission is complete before beginning the new sequence.

    If looping has been enabled with `RMT.loop`, the sequence will be
    repeated indefinitely. Further calls to this method will block until the
    end of the current loop iteration before immediately beginning to loop the
    new sequence of pulses. Looping sequences longer than 126 pulses is not
    supported by the hardware.

.. staticmethod:: RMT.bitstream_rmt([value])

    Configure RMT usage in the `machine.bitstream` implementation.

    If *value* is ``True``, bitstream tries to use RMT if possible. If *value*
    is ``False``, bitstream sticks to the bit-banging implementation.

    If no parameter is supplied, it returns the current state. The default state
    is ``True``.

.. staticmethod:: RMT.bitstream_channel([value])

    *This function is deprecated and will be replaced by `RMT.bitstream_rmt()`.*

    Passing in no argument will return ``1`` if RMT was enabled for the `machine.bitstream`
    feature, and ``None`` otherwise.

    Passing any non-negative integer argument is equivalent to calling ``RMT.bitstream_rmt(True)``.

    .. note:: In previous versions of MicroPython it was necessary to use this function to assign
              a specific RMT channel number for the bitstream, but the channel number is now assigned
              dynamically.

Constants
---------

.. data:: RMT.PULSE_MAX

   Maximum integer that can be set for a pulse duration.

Ultra-Low-Power co-processor
----------------------------

This class gives access to the Ultra Low Power (ULP) co-processor on the ESP32,
ESP32-S2 and ESP32-S3 chips.

.. warning::

    This class does not provide access to the RISCV ULP co-processor available
    on the ESP32-S2 and ESP32-S3 chips.

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
