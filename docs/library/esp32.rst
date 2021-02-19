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

.. function:: wake_on_ext0(pin, level)

    Configure how EXT0 wakes the device from sleep.  *pin* can be ``None``
    or a valid Pin object.  *level* should be ``esp32.WAKEUP_ALL_LOW`` or
    ``esp32.WAKEUP_ANY_HIGH``.

.. function:: wake_on_ext1(pins, level)

    Configure how EXT1 wakes the device from sleep.  *pins* can be ``None``
    or a tuple/list of valid Pin objects.  *level* should be ``esp32.WAKEUP_ALL_LOW``
    or ``esp32.WAKEUP_ANY_HIGH``.

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

.. class:: Partition(id)

    Create an object representing a partition.  *id* can be a string which is the label
    of the partition to retrieve, or one of the constants: ``BOOT`` or ``RUNNING``.

.. classmethod:: Partition.find(type=TYPE_APP, subtype=0xff, label=None)

    Find a partition specified by *type*, *subtype* and *label*.  Returns a
    (possibly empty) list of Partition objects. Note: ``subtype=0xff`` matches any subtype
    and ``label=None`` matches any label.

.. method:: Partition.info()

    Returns a 6-tuple ``(type, subtype, addr, size, label, encrypted)``.

.. method:: Partition.readblocks(block_num, buf)
            Partition.readblocks(block_num, buf, offset)
.. method:: Partition.writeblocks(block_num, buf)
            Partition.writeblocks(block_num, buf, offset)
.. method:: Partition.ioctl(cmd, arg)

    These methods implement the simple and :ref:`extended
    <block-device-interface>` block protocol defined by
    :class:`uos.AbstractBlockDev`.

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
    r  # RMT(channel=0, pin=18, source_freq=80000000, clock_div=8)

    # To use carrier frequency
    r = esp32.RMT(0, pin=Pin(18), clock_div=8, carrier_freq=38000)
    r  # RMT(channel=0, pin=18, source_freq=80000000, clock_div=8, carrier_freq=38000, carrier_duty_percent=50)

    # The channel resolution is 100ns (1/(source_freq/clock_div)).
    r.write_pulses((1, 20, 2, 40), start=0)  # Send 0 for 100ns, 1 for 2000ns, 0 for 200ns, 1 for 4000ns

The input to the RMT module is an 80MHz clock (in the future it may be able to
configure the input clock but, for now, it's fixed). ``clock_div`` *divides*
the clock input which determines the resolution of the RMT channel. The
numbers specificed in ``write_pulses`` are multiplied by the resolution to
define the pulses.

``clock_div`` is an 8-bit divider (0-255) and each pulse can be defined by
multiplying the resolution by a 15-bit (0-32,768) number. There are eight
channels (0-7) and each can have a different clock divider.

To enable the carrier frequency feature of the esp32 hardware, specify the
``carrier_freq`` as something like 38000, a typical IR carrier frequency.

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


.. class:: RMT(channel, *, pin=None, clock_div=8, carrier_freq=0, carrier_duty_percent=50)

    This class provides access to one of the eight RMT channels. *channel* is
    required and identifies which RMT channel (0-7) will be configured. *pin*,
    also required, configures which Pin is bound to the RMT channel. *clock_div*
    is an 8-bit clock divider that divides the source clock (80MHz) to the RMT
    channel allowing the resolution to be specified. *carrier_freq* is used to
    enable the carrier feature and specify its frequency, default value is ``0``
    (not enabled).  To enable, specify a positive integer.  *carrier_duty_percent*
    defaults to 50.

.. method:: RMT.source_freq()

    Returns the source clock frequency. Currently the source clock is not
    configurable so this will always return 80MHz.

.. method:: RMT.clock_div()

    Return the clock divider. Note that the channel resolution is
    ``1 / (source_freq / clock_div)``.

.. method:: RMT.wait_done(timeout=0)

    Returns ``True`` if the channel is currently transmitting a stream of pulses
    started with a call to `RMT.write_pulses`.

    If *timeout* (defined in ticks of ``source_freq / clock_div``) is specified
    the method will wait for *timeout* or until transmission is complete,
    returning ``False`` if the channel continues to transmit. If looping is
    enabled with `RMT.loop` and a stream has started, then this method will
    always (wait and) return ``False``.

.. method:: RMT.loop(enable_loop)

    Configure looping on the channel. *enable_loop* is bool, set to ``True`` to
    enable looping on the *next* call to `RMT.write_pulses`. If called with
    ``False`` while a looping stream is currently being transmitted then the
    current set of pulses will be completed before transmission stops.

.. method:: RMT.write_pulses(pulses, start)

    Begin sending *pulses*, a list or tuple defining the stream of pulses. The
    length of each pulse is defined by a number to be multiplied by the channel
    resolution ``(1 / (source_freq / clock_div))``. *start* defines whether the
    stream starts at 0 or 1.

    If transmission of a stream is currently in progress then this method will
    block until transmission of that stream has ended before beginning sending
    *pulses*.

    If looping is enabled with `RMT.loop`, the stream of pulses will be repeated
    indefinitely. Further calls to `RMT.write_pulses` will end the previous
    stream - blocking until the last set of pulses has been transmitted -
    before starting the next stream.


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
