.. currentmodule:: machine
.. _machine.SDCard:

class SDCard -- secure digital memory card
==========================================

SD cards are one of the most common small form factor removable storage media.
SD cards come in a variety of sizes and physical form factors. MMC cards are
similar removable storage devices while eMMC devices are electrically similar
storage devices designed to be embedded into other systems. All three form
share a common protocol for communication with their host system and high-level
support looks the same for them all. As such in MicroPython they are implemented
in a single class called :class:`machine.SDCard` .

Both SD and MMC interfaces support being accessed with a variety of bus widths.
When being accessed with a 1-bit wide interface they can be accessed using the
SPI protocol. Different MicroPython hardware platforms support different widths
and pin configurations but for most platforms there is a standard configuration
for any given hardware. In general constructing an ``SDCard`` object with without
passing any parameters will initialise the interface to the default card slot
for the current hardware. The arguments listed below represent the common
arguments that might need to be set in order to use either a non-standard slot
or a non-standard pin assignment. The exact subset of arguments supported will
vary from platform to platform.

.. class:: SDCard(slot=1, width=1, cd=None, wp=None, sck=None, miso=None, mosi=None,
                  cs=None, cmd=None, data=None, freq=20000000)

    This class provides access to SD or MMC storage cards using either
    a dedicated SD/MMC interface hardware or through an SPI channel.
    The class implements the block protocol defined by :class:`vfs.AbstractBlockDev`.
    This allows the mounting of an SD card to be as simple as::

      vfs.mount(machine.SDCard(), "/sd")

    The constructor takes the following parameters:

     - *slot* selects which of the available interfaces to use. Leaving this
       unset will select the default interface.

     - *width* selects the bus width for the SD/MMC interface. This many data
       pins must be connected to the SD card.

     - *cd* can be used to specify a card-detect pin.

     - *wp* can be used to specify a write-protect pin.

     - *sck* can be used to specify an SPI clock pin.

     - *miso* can be used to specify an SPI miso pin.

     - *mosi* can be used to specify an SPI mosi pin.

     - *cs* can be used to specify an SPI chip select pin.

    The following additional parameters are only present on ESP32 port:

     - *cmd* can be used to specify the SD CMD pin (ESP32-S3 only).

     - *data* can be used to specify a list or tuple of SD data bus pins
       (ESP32-S3 only).

     - *freq* selects the SD/MMC interface frequency in Hz.

Implementation-specific details
-------------------------------

Different implementations of the ``SDCard`` class on different hardware support
varying subsets of the options above.

PyBoard
```````

The standard PyBoard has just one slot. No arguments are necessary or supported.

ESP32
`````

SD cards support access in both SD/MMC mode and the simpler (but slower) SPI
mode.

SPI mode makes use of a `SPI` host peripheral, which cannot concurrently be used
for other SPI interactions.

The ``slot`` argument determines which mode is used. Different values are
supported on different chips:

========== ======== ======== ============ ============
Chip       Slot 0   Slot 1   Slot 2       Slot 3
========== ======== ======== ============ ============
ESP32               SD/MMC   SPI (id=1)   SPI (id=0)
ESP32-C3                     SPI (id=0)
ESP32-C6                     SPI (id=0)
ESP32-S2                     SPI (id=1)   SPI (id=0)
ESP32-S3   SD/MMC   SD/MMC   SPI (id=1)   SPI (id=0)
========== ======== ======== ============ ============

Different slots support different data bus widths (number of data pins):

========== ========== =====================
Slot       Type       Supported data widths
========== ========== =====================
0          SD/MMC     1, 4, 8
1          SD/MMC     1, 4
2          SPI        1
3          SPI        1
========== ========== =====================

.. note:: Most ESP32 modules that provide an SD card slot using the
          dedicated hardware only wire up 1 data pin, so the default
          value for ``width`` is 1.

Additional details depend on which ESP32 family chip is in use:

Original ESP32
~~~~~~~~~~~~~~

In SD/MMC mode (slot 1), pin assignments in SD/MMC mode are fixed on the
original ESP32. The SPI mode slots (2 & 3) allow pins to be set to different
values in the constructor.

The default pin assignments are as follows:

    ====== ====== ====== ====== ============
    Slot   1      2      3      Can be set
    ------ ------ ------ ------ ------------
    Signal   Pin    Pin    Pin
    ====== ====== ====== ====== ============
    CLK      14                 No
    CMD      15                 No
    D0        2                 No
    D1        4                 No
    D2       12                 No
    D3       13                 No
    sck             18     14   Yes
    cs               5     15   Yes
    miso            19     12   Yes
    mosi            23     13   Yes
    ====== ====== ====== ====== ============

The ``cd`` and ``wp`` pins are not fixed in either mode and default to disabled, unless set.

ESP32-S3
~~~~~~~~

The ESP32-S3 chip allows pins to be set to different values for both SD/MMC and
SPI mode access.

If not set, default pin assignments are as follows:

    ======== ====== ====== ====== ======
    Slot     0      1      2      3
    -------- ------ ------ ------ ------
    Signal     Pin    Pin    Pin    Pin
    ======== ====== ====== ====== ======
    CLK      14     14
    CMD      15     15
    D0        2      2
    D1        4      4
    D2       12     12
    D3       13     13
    D4       33*
    D5       34*
    D6       35*
    D7       36*
    sck                    37*     14
    cs                     34*     13
    miso                   37*      2
    mosi                   35*     15
    ======== ====== ====== ====== ======

.. note:: Slots 0 and 1 cannot both be in use at the same time.

.. note:: Pins marked with an asterisk * in the table must be changed from the
          default if the ESP32-S3 board is configured for Octal SPI Flash or
          PSRAM.

To access a card in SD/MMC mode, set ``slot`` parameter value 0 or 1 and
parameters ``sck`` (for CLK), ``cmd`` and ``data`` as needed to assign pins. If
the ``data`` argument is passed then it should be a list or tuple of data pins
or pin numbers with length equal to the ``width`` argument. For example::

    sd = SDCard(slot=0, width=4, sck=8, cmd=9, data=(10, 11, 12, 13))

To access a card in SPI mode, set ``slot`` parameter value 2 or 3 and pass
parameters ``sck``, ``cs``, ``miso``, ``mosi`` as needed to assign pins.

In either mode the ``cd`` and ``wp`` pins default to disabled, unless set in the
constructor.

Other ESP32 chips
~~~~~~~~~~~~~~~~~

Other ESP32 family chips do not have hardware SD/MMC host controllers and can
only access SD cards in SPI mode.

To access a card in SPI mode, set ``slot`` parameter value 2 or 3 and pass
parameters ``sck``, ``cs``, ``miso``, ``mosi`` to assign pins.

.. note:: ESP32-C3 and ESP32-C6 only have one available `SPI` bus, so the only
          valid ``slot`` parameter value is 2. Using this bus for the SD card
          will prevent also using it for :class:`machine.SPI`.

cc3200
``````

You can set the pins used for SPI access by passing a tuple as the
*pins* argument.

*Note:* The current cc3200 SD card implementation names the this class
:class:`machine.SD` rather than :class:`machine.SDCard` .

mimxrt
``````

The SDCard module for the mimxrt port only supports access via dedicated SD/MMC
peripheral (USDHC) in 4-bit mode with 50MHz clock frequency exclusively.
Unfortunately the MIMXRT1011 controller does not support the USDHC peripheral.
Hence this controller does not feature the ``machine.SDCard`` module.

Due to the decision to only support 4-bit mode with 50MHz clock frequency the
interface has been simplified, and the constructor signature is:

.. class:: SDCard(slot=1)
  :noindex:

The pins used for the USDHC peripheral have to be configured in ``mpconfigboard.h``.
Most of the controllers supported by the mimxrt port provide up to two USDHC
peripherals.  Therefore the pin configuration is performed using the macro
``MICROPY_USDHCx`` with x being 1 or 2 respectively.

The following shows an example configuration for USDHC1::

  #define MICROPY_USDHC1 \
    { \
          .cmd   = { GPIO_SD_B0_02_USDHC1_CMD}, \
          .clk   = { GPIO_SD_B0_03_USDHC1_CLK }, \
          .cd_b  = { GPIO_SD_B0_06_USDHC1_CD_B },\
          .data0 = { GPIO_SD_B0_04_USDHC1_DATA0 },\
          .data1 = { GPIO_SD_B0_05_USDHC1_DATA1 },\
          .data2 = { GPIO_SD_B0_00_USDHC1_DATA2 },\
          .data3 = { GPIO_SD_B0_01_USDHC1_DATA3 },\
    }

If the card detect pin is not used (cb_b pin) then the respective entry has to be
filled with the following dummy value::

  #define USDHC_DUMMY_PIN NULL , 0

Based on the definition of macro ``MICROPY_USDHC1`` and/or ``MICROPY_USDHC2``
the ``machine.SDCard`` module either supports one or two slots.  If only one of
the defines is provided, calling ``machine.SDCard()`` or ``machine.SDCard(1)``
will return an instance using the respective USDHC peripheral.  When both macros
are defined, calling ``machine.SDCard(2)`` returns an instance using USDHC2.
