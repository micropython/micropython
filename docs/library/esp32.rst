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
.. method:: Partition.writeblocks(block_num, buf)
.. method:: Partition.ioctl(cmd, arg)

    These methods implement the block protocol defined by :class:`uos.AbstractBlockDev`.

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

The Ultra-Low-Power co-processor
--------------------------------

.. class:: ULP()

    This class provides access to the Ultra-Low-Power co-processor.

.. method:: ULP.set_wakeup_period(period_index, period_us)

    Set the wake-up period.

.. method:: ULP.load_binary(load_addr, program_binary)

    Load a *program_binary* into the ULP at the given *load_addr*.

.. method:: ULP.run(entry_point)

    Start the ULP running at the given *entry_point*.

RMT
---

.. class:: RMT(channel, pin, clock_div)

    This class provides access to one of the eight RMT channels. *channel* is
    required and identifies which RMT channel (0-7) will be configured. *pin*,
    also required, configures which Pin is bound to the RMT channel. *clock_div*
    is an 8-bit clock divider that divides the source clock (80MHz) to the RMT
    channel allowing the *resolution* to be specified.

.. method:: RMT.source_freq()

    Returns the source clock frequency. Currently the source clock is not
    configurable so this will always return 80MHz.

.. method:: RMT.clock_div()

    Return the clock divider. Note that *resolution* is
    ``1/(source_freq/clock_div)``.

.. method:: RMT.wait_done(timeout=0)

    Returns True if ``send_pulses`` has completed.

    If *timeout* (defined in ticks of ``source_freq/clock_div``) is specified
    the method will wait for *timeout* or until ``send_pulses`` is complete,
    returning False if the channel continues to transmit.

.. Warning::
    Avoid using ``wait_done()`` if looping is enabled.

.. method:: RMT.loop(enable_loop)

    Configure looping on the channel, allowing a stream of pulses to be
    indefinitely repeated. *enable_loop* is bool, set to True to enable looping.

.. method:: RMT.send_pulses(pulses, start)

    Begin sending *pulses*, a list or tuple defining the stream of pulses. The
    length of each pulse is defined by a number to be multiplied by the channel
    resolution (1/(source_freq/clock_div)). *start* defines whether the stream
    starts at 0 or 1.


Constants
---------

.. data:: esp32.WAKEUP_ALL_LOW
          esp32.WAKEUP_ANY_HIGH

   Selects the wake level for pins.
