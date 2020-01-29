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

Flash partitions
----------------

This class gives access to the partitions in the device's flash memory.

.. class:: Partition(id)

    Create an object representing a partition.  *id* can be a string which is the label
    of the partition to retrieve, or one of the constants: ``BOOT`` or ``RUNNING``.

.. classmethod:: Partition.find(type=TYPE_APP, subtype=0xff, label=None)

    Find a partition specified by *type*, *subtype* and *label*.  Returns a
    (possibly empty) list of Partition objects.

.. method:: Partition.info()

    Returns a 6-tuple ``(type, subtype, addr, size, label, encrypted)``.

.. method:: Partition.readblocks(block_num, buf)
.. method:: Partition.readblocks(block_num, buf, offset)
.. method:: Partition.writeblocks(block_num, buf)
.. method:: Partition.writeblocks(block_num, buf, offset)
.. method:: Partition.ioctl(cmd, arg)

    These methods implement the simple and :ref:`extended
    <block-device-interface>` block protocol defined by
    :class:`uos.AbstractBlockDev`.

.. method:: Partition.set_boot()

    Sets the partition as the boot partition.

.. method:: Partition.get_next_update()

    Gets the next update partition after this one, and returns a new Partition object.

Constants
~~~~~~~~~

.. data:: Partition.BOOT
          Partition.RUNNING

    Used in the `Partition` constructor to fetch various partitions.

.. data:: Partition.TYPE_APP
          Partition.TYPE_DATA

    Used in `Partition.find` to specify the partition type.


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

So, in the example above, the 80MHz clock is divided by 8. Thus the
resolution is (1/(80Mhz/8)) 100ns. Since the ``start`` level is 0 and toggles
with each number, the bitstream is ``0101`` with durations of [100ns, 2000ns,
100ns, 4000ns].

For more details see Espressif's `ESP-IDF RMT documentation.
<https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/peripherals/rmt.html>`_.

.. Warning::
   The current MicroPython RMT implementation lacks some features, most notably
   receiving pulses and carrier transmit. RMT should be considered a
   *beta feature* and the interface may change in the future.


.. class:: RMT(channel, \*, pin=None, clock_div=8)

    This class provides access to one of the eight RMT channels. *channel* is
    required and identifies which RMT channel (0-7) will be configured. *pin*,
    also required, configures which Pin is bound to the RMT channel. *clock_div*
    is an 8-bit clock divider that divides the source clock (80MHz) to the RMT
    channel allowing the resolution to be specified.

.. method:: RMT.source_freq()

    Returns the source clock frequency. Currently the source clock is not
    configurable so this will always return 80MHz.

.. method:: RMT.clock_div()

    Return the clock divider. Note that the channel resolution is
    ``1 / (source_freq / clock_div)``.

.. method:: RMT.wait_done(timeout=0)

    Returns True if `RMT.write_pulses` has completed.

    If *timeout* (defined in ticks of ``source_freq / clock_div``) is specified
    the method will wait for *timeout* or until `RMT.write_pulses` is complete,
    returning ``False`` if the channel continues to transmit.

.. Warning::
    Avoid using ``wait_done()`` if looping is enabled.

.. method:: RMT.loop(enable_loop)

    Configure looping on the channel, allowing a stream of pulses to be
    indefinitely repeated. *enable_loop* is bool, set to True to enable looping.

.. method:: RMT.write_pulses(pulses, start)

    Begin sending *pulses*, a list or tuple defining the stream of pulses. The
    length of each pulse is defined by a number to be multiplied by the channel
    resolution ``(1 / (source_freq / clock_div))``. *start* defines whether the
    stream starts at 0 or 1.


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
