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


SD/MMC Card interface
---------------------

.. class:: SDCard(slot=1, spi=false, width=1, cd=None, wp=None, sck=None, miso=None, mosi=None, cs=None)

    This class provides access to SD or MMC storage cards using either
    the ESP32's dedicated SD/MMC interface hardware or through an SPI
    channel using DMA for the data transfers. The class implements
    that block protocol defined in the :class:`uos.AbstractBlockDev`
    class. This allows the mounting of an SD card to be as simple as::

      uos.mount(esp32.SDCard(), "/sd")

    The constrcutor takes the following paramters:

     - ``slot`` selects which of two interfaces to use. The ESP32
       provides two channels of SD/MMC hardware but it should be noted
       that slot 0 is often used to communicate with on-board
       flash memory on most ESP32 modules. As a result only slot 1
       will normally be available. When using SPI mode this selects
       between the two hardware SPI drivers.

     - ``spi`` selects the use of an SPI interface instead of the
       dedicated SD/MMC interface.

     - ``width`` selects the bus width for the SD/MMC interface. Slot
       0 supports 1, 4 or 8-bit wide access while slot 1 supports 1 ot
       4-bit access. Note that most ESP32 modules that provide an SD card slot
       using the dedicated hardware only wire up 1 data pin so the
       default is 1.

     - ``cd`` can be used to specify a card-detect pin.

     - ``wp`` can be used to specify a write-protect pin.

     - ``sck`` can be used to specify an SPI clock pin.

     - ``miso`` can be used to specify an SPI miso pin.

     - ``mosi`` can be used to specify an SPI mosi pin.

     - ``cs`` can be used to specify an SPI chip select pin.

    The pins used by the dedicated SD/MMC hardware are fixed; they are
    assigned as follows:

    ====== ====== ======
    SD/MMC Slot 0 Slot 1
    ------ ------ ------
    Signal   Pin    Pin
    ====== ====== ======
    cmd      11     15
    sck       6     14
    D0        7      2
    D1        8      4
    D2        9     12
    D3       10     13
    D4       16
    D5       17
    D6        5
    D7       18
    ====== ====== ======

    The pins used by the SPI hardware can be reassigned. It should
    however be noted that if any of the signals are remapped then all
    of the SPI signals will pass through a GPIO multiplexer unit
    which can limit the performance of high frequency signals. Since
    the normal operating speed for SD cards is 40MHz this can cause
    problems on some cards. The default (and preferred) pin assignment
    are as follows:

    ====== ====== ======
    SPI    Slot 0 Slot 1
    ------ ------ ------
    Signal   Pin    Pin
    ====== ====== ======
    sck      18     14
    cs        5     15
    miso     19     12
    mosi     23     13
    ====== ====== ======

Constants
---------

.. data:: esp32.WAKEUP_ALL_LOW
          esp32.WAKEUP_ANY_HIGH

   Selects the wake level for pins.
