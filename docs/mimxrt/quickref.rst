.. _mimxrt_quickref:

Quick reference for the i.MXRT family
=====================================

.. image:: img/teensy_4.1.jpg
    :alt: Teensy 4.1 board
    :width: 640px

The Teensy 4.1 board.

Below is a quick reference for i.MXRT-based boards.  If it is your first time
working with this board it may be useful to get an overview of the microcontroller:

.. toctree::
   :maxdepth: 1

   general.rst
   tutorial/intro.rst

Documentation for proprietary classes or class extensions:

.. toctree::
   :maxdepth: 1

   ../library/machine.PWMmimxrt.rst

Installing MicroPython
----------------------

See the corresponding section of tutorial: :ref:`mimxrt_intro`. It also includes
a troubleshooting subsection.

General board control
---------------------

The MicroPython REPL is on the USB port, configured in VCP mode.
Tab-completion is useful to find out what methods an object has.
Paste mode (ctrl-E) is useful to paste a large slab of Python code into
the REPL.

The :mod:`machine` module::

    import machine

    machine.freq()          # get the current frequency of the CPU



Delay and timing
----------------

Use the :mod:`time <utime>` module::

    import time

    time.sleep(1)           # sleep for 1 second
    time.sleep_ms(500)      # sleep for 500 milliseconds
    time.sleep_us(10)       # sleep for 10 microseconds
    start = time.ticks_ms() # get millisecond counter
    delta = time.ticks_diff(time.ticks_ms(), start) # compute time difference

Timers
------

The i.MXRT port has three hardware timers. Use the :ref:`machine.Timer <machine.Timer>` class
with a timer ID from 0 to 2 (inclusive)::

    from machine import Timer

    tim0 = Timer(0)
    tim0.init(period=5000, mode=Timer.ONE_SHOT, callback=lambda t:print(0))

    tim1 = Timer(1)
    tim1.init(period=2000, mode=Timer.PERIODIC, callback=lambda t:print(1))

The period is in milliseconds.

Virtual timers are not currently supported on this port.

.. _mimxrt_Pins_and_GPIO:

Pins and GPIO
-------------

Use the :ref:`machine.Pin <machine.Pin>` class::

    from machine import Pin

    p0 = Pin('D0', Pin.OUT) # create output pin on GPIO0
    p0.on()                 # set pin to "on" (high) level
    p0.off()                # set pin to "off" (low) level
    p0.value(1)             # set pin to on/high

    p2 = Pin('D2', Pin.IN)  # create input pin on GPIO2
    print(p2.value())       # get value, 0 or 1

    p4 = Pin('D4', Pin.IN, Pin.PULL_UP) # enable internal pull-up resistor
    p5 = Pin('D5', Pin.OUT, value=1) # set pin high on creation

    p6 = Pin(pin.cpu.GPIO_B1_15, Pin.OUT) # Use the cpu pin name.

Available Pins follow the ranges and labelling of the respective board, like:

- 0-33 for Teensy 4.0,
- 0-21 for the MIMXRT10xx-EVK board, or 'D0-Dxx', or 'A0-Ann',
- 'J3_xx', 'J4_xx', 'J5_xx' for the ARCH MIX board,

or the pin names of the Pin.board or Pin.cpu classes.


Notes:

* The MIMXRT1xxx-EVK boards may have other on-board devices connected to these
  pins, limiting it's use for input or output.
* At the MIMXRT1010_EVK, pins D4, D5 and D9 of the Arduino connector are by
  default not connected to the MCU. For details refer to the schematics.
* At the MIMXRT1170_EVK board, the inner rows of the Arduino connectors are assigned as follows:  
    - D16 - D23: J9, odd pin numbers; D17 is by default not connected.
    - D24 - D27: J26, odd pin numbers; J63-J66 have to be closed to enable these pins.
    - D29 - D36: J25, odd pin numbers; D29 and D30 are by default not connected.

There's a higher-level abstraction :ref:`machine.Signal <machine.Signal>`
which can be used to invert a pin. Useful for illuminating active-low LEDs
using ``on()`` or ``value(1)``.

UART (serial bus)
-----------------

See :ref:`machine.UART <machine.UART>`. ::

    from machine import UART

    uart1 = UART(1, baudrate=115200)
    uart1.write('hello')  # write 5 bytes
    uart1.read(5)         # read up to 5 bytes

The i.MXRT has up to eight hardware UARTs, but not every board exposes all
TX and RX pins for users. The pin assignment of UARTs to pins is fixed.
The UARTs are numbered 1..8. The rx/tx pins are assigned according to the
tables below:

================  ===========  ===========  ===========  ===========
Board / Pin           UART0        UART1        UART2        UART3
================  ===========  ===========  ===========  ===========
Teensy 4.0             -            0/1          7/8         14/15
Teensy 4.1             -            0/1          7/8         14/15
MIMXRT1010-EVK     Debug USB      D0/D1        D7/D6           -
MIMXRT1015-EVK     Debug USB      D0/D1        D7/A1           -
MIMXRT1020-EVK     Debug USB      D0/D1        D9/D6       D10/D13
MIMXRT1050-EVK     Debug USB      D0/D1        D7/D6        D8/D9
MIMXRT1050-EVKB    Debug USB      D0/D1        D7/D6        D8/D9
MIMXRT1060-EVK     Debug USB      D0/D1        D7/D6        D8/D9
MIMXRT1064-EVK     Debug USB      D0/D1        D7/D6        D8/D9
MIMXRT1170-EVK     Debug USB      D0/D1       D12/D11      D10/D13
Seeed ARCH MIX        -        J3_19/J3_20  J4_16/J4_17  J4_06/J4_07
================  ===========  ===========  ===========  ===========

|
|

================  ===========  ===========  =======  =======  =====
Board / Pin           UART4        UART5      UART6    UART7   UART8
================  ===========  ===========  =======  =======  =====
Teensy 4.0            16/17        21/20     25/24    28/29      -
Teensy 4.1            16/17        21/20     25/24    28/29    34/35
MIMXRT1010-EVK         -            -         -        -        -
MIMXRT1015-EVK         -            -         -        -        -
MIMXRT1020-EVK      D15/D14       A1/A0       -        -        -
MIMXRT1050-EVK       A1/A0          -         -        -        -
MIMXRT1050-EVKB      A1/A0          -         -        -        -
MIMXRT1060-EVK       A1/A0          -         -        -        -
MIMXRT1064-EVK       A1/A0          -         -        -        -
MIMXRT1170-EVK      D15/D14      D25/D26    D33/D34  D35/D36    -
Seeed ARCH MIX    J4_10/J4_11  J5_08/J5_12    -        -        -
================  ===========  ===========  =======  =======  =====

|

PWM (pulse width modulation)
----------------------------

The i.MXRT has up to four dedicated PWM modules with four FLEXPWM submodules each
and up to four QTMR modules with four channels, which can be used to generate
a PWM signal of signal pair.

The PWM functions are provided by the :ref:`machine.PWM <mimxrt_machine.PWM>` class.
It supports all basic methods listed for that class and a few additional methods for
handling signal groups. ::

    # Samples for Teensy
    #

    from machine import Pin, PWM

    pwm2 = PWM(Pin(2))      # create PWM object from a pin
    pwm2.freq()             # get current frequency
    pwm2.freq(1000)         # set frequency
    pwm2.duty_u16()         # get current duty cycle, range 0-65535
    pwm2.duty_u16(200)      # set duty cycle, range 0-65535
    pwm2.deinit()           # turn off PWM on the pin
    # create a complementary signal pair on Pin 2 and 3
    pwm2 = PWM((2, 3), freq=2000, duty_ns=20000)

    # Create a group of four synchronized signals.
    # Start with Pin(4) at submodule 0, which creates the sync pulse.
    pwm4 = PWM(Pin(4), freq=1000, align=PWM.HEAD)
    # Pins 5, 6, and 9 are pins at the same module
    pwm5 = PWM(Pin(5), freq=1000, duty_u16=10000, align=PWM.HEAD, sync=True)
    pwm6 = PWM(Pin(6), freq=1000, duty_u16=20000, align=PWM.HEAD, sync=True)
    pwm9 = PWM(Pin(9), freq=1000, duty_u16=30000, align=PWM.HEAD, sync=True)

    pwm3                    # show the PWM objects properties

For further details, look at the specific documentation :ref:`machine.PWM <mimxrt_machine.PWM>`.

ADC (analog to digital conversion)
----------------------------------

On the i.MXRT ADC functionality is available on Pins labeled 'Ann'.

Use the :ref:`machine.ADC <machine.ADC>` class::

    from machine import ADC

    adc = ADC(Pin(32))          # create ADC object on ADC pin
    adc.read_u16()              # read value, 0-65536 across voltage range 0.0v - 3.3v

The resolution of the ADC is 12 bit with 10 to 11 bit accuracy, irrespective of the
value returned by read_u16(). If you need a higher resolution or better accuracy, use
an external ADC.

Software SPI bus
----------------

Software SPI (using bit-banging) works on all pins, and is accessed via the
:ref:`machine.SoftSPI <machine.SoftSPI>` class. ::

    from machine import Pin, SoftSPI

    # construct a SoftSPI bus on the given pins
    # polarity is the idle state of SCK
    # phase=0 means sample on the first edge of SCK, phase=1 means the second
    spi = SoftSPI(baudrate=100000, polarity=1, phase=0, sck=Pin(0), mosi=Pin(2), miso=Pin(4))

    spi.init(baudrate=200000) # set the baudrate

    spi.read(10)            # read 10 bytes on MISO
    spi.read(10, 0xff)      # read 10 bytes while outputting 0xff on MOSI

    buf = bytearray(50)     # create a buffer
    spi.readinto(buf)       # read into the given buffer (reads 50 bytes in this case)
    spi.readinto(buf, 0xff) # read into the given buffer and output 0xff on MOSI

    spi.write(b'12345')     # write 5 bytes on MOSI

    buf = bytearray(4)      # create a buffer
    spi.write_readinto(b'1234', buf) # write to MOSI and read from MISO into the buffer
    spi.write_readinto(buf, buf) # write buf to MOSI and read MISO back into buf

The highest supported baud rate is 500000.

.. Warning::
   Currently *all* of ``sck``, ``mosi`` and ``miso`` *must* be specified when
   initialising Software SPI.

Hardware SPI bus
----------------

There are up to four hardware SPI channels that allow faster transmission
rates (up to 90Mhz). The SPI signals are fixed assigned to GPIO pins.
It depends on the board design, which SPI's signals are exposed to
the user, as detailed in the table below.
The signal order in the table is CS, MOSI, MISO and CLK.

=================  =======================  =======================  =============
Board / Pin        SPI0                     SPI1                     SPI2
=================  =======================  =======================  =============
Teensy 4.0         10/11/12/13              0/26/1/27                     -
Teensy 4.1         10/11/12/13              38/26/39/27              -/50/54/49
MIXMXRT1010-EVK    D10/D11/D12/D13              -                        -
MIXMXRT1015-EVK    D10/D11/D12/D13              -                        -
MIXMXRT1020-EVK    D10/D11/D12/D13              -                        -
MIXMXRT1050-EVK    D10/D11/D12/D13 (*)          -                        -
MIXMXRT1050-EVKB   D10/D11/D12/D13 (*)          -                        -
MIXMXRT1060-EVK    D10/D11/D12/D13 (*)          -                        -
MIXMXRT1064-EVK    D10/D11/D12/D13 (*)          -                        -
MIXMXRT1170-EVK    D10/D11/D12/D13          D28/D25/D24/D26          -/D14/D15/D24
Seeed ARCH MIX     J4_12/J4_14/J4_13/J4_15  J3_09/J3_05/J3_08_J3_11
=================  =======================  =======================  =============

Pins denoted with (*) are by default not wired at the board.


Hardware SPI is accessed via the :ref:`machine.SPI <machine.SPI>` class and
has the same methods as software SPI above::

    from machine import SPI

    spi = SPI(0, 10000000)
    spi.write('Hello World')

Notes:

1. Even if the highest supported baud rate at the moment is 90 Mhz,
setting a baud rate will not always result in exactly that
frequency, especially at high baud rates.

2. Sending at 90 MHz is possible, but in the tests receiving
only worked up to 45 MHz.


Software I2C bus
----------------

Software I2C (using bit-banging) works on all output-capable pins, and is
accessed via the :ref:`machine.SoftI2C <machine.SoftI2C>` class::

    from machine import Pin, SoftI2C

    i2c = SoftI2C(scl=Pin(5), sda=Pin(4), freq=100000)

    i2c.scan()              # scan for devices

    i2c.readfrom(0x3a, 4)   # read 4 bytes from device with address 0x3a
    i2c.writeto(0x3a, '12') # write '12' to device with address 0x3a

    buf = bytearray(10)     # create a buffer with 10 bytes
    i2c.writeto(0x3a, buf)  # write the given buffer to the slave

The highest supported freq is 400000.

Hardware I2C bus
----------------

There are up to four hardware I2C channels that allow faster transmission
rates and support the full I2C protocol. The I2C signals are fixed assigned to GPIO pins.
It depends on the board design, which I2C's signals are exposed to
the user, as detailed in the table below.
The signal order in the table is SDA, SCL.

=================  ===========  ===========  ===========  =======  =======
Board / Pin        I2C 0        I2C 1        I2C 2        I2C 3    I2C 4
=================  ===========  ===========  ===========  =======  =======
Teensy 4.0         18/19        17/16        25/24         -        -
Teensy 4.1         18/19        17/16        25/24         -        -
MIXMXRT1010-EVK    D14/D15      D0/D1         -            -        -
MIXMXRT1015-EVK    D14/D15       -            -            -        -
MIXMXRT1020-EVK    D14/D15      A4/A5        D0/D1         -        -
MIXMXRT1050-EVK    A4/A5        D1/D0         -            -        -
MIXMXRT1050-EVKB   A4/A5        D1/D0         -            -        -
MIXMXRT1060-EVK    A4/A5        D1/D0         -            -        -
MIXMXRT1064-EVK    A4/A5        D1/D0         -            -        -
MIXMXRT1170-EVK    D14/D15      D1/D0        A4/A5        D26/D25  D19/D18
Seeed ARCH MIX     J3_17/J3_16  J4_06/J4_07  J5_05/J5_04   -        -
=================  ===========  ===========  ===========  =======  =======


Hardware I2C is accessed via the :ref:`machine.I2C <machine.I2C>` class and
has the same methods as software SPI above::

    from machine import I2C

    i2c = I2C(0, 400_000)
    i2c.writeto(0x76, b"Hello World")

Real time clock (RTC)
---------------------

See :ref:`machine.RTC <machine.RTC>` ::

    from machine import RTC

    rtc = RTC()
    rtc.datetime((2017, 8, 23, 1, 12, 48, 0, 0)) # set a specific date and time
    rtc.datetime() # get date and time
    rtc.now() # return date and time in CPython format.

The i.MXRT MCU supports battery backup of the RTC. By connecting a battery of 1.5-3.6V,
time and date are maintained in the absence of the main power. The current drawn
from the battery is ~20µA, which is rather high. A CR2032 coin cell will
last for about one year.


SD card
-------

See :ref:`machine.SDCard <machine.SDCard>`. ::

    import machine, uos

    sd = machine.SDCard()
    fs = uos.VfsFat(sd)
    uos.mount(fs, "/sd")  # mount

    uos.listdir('/sd')    # list directory contents

    uos.umount('/sd')     # eject

Note: The i.mx-rt 1011 and 1015 based boards do not support the machine.SDCard() class. For these,
      the SPI based driver sdcard.py from the MicroPython Library can be used. When using it, you
      have to overdrive the CS pin of the SPI hardware module. Example::

        import os, sdcard, machine

        cs_pin = "D10"
        spi = machine.SPI(0) # SPI0 with cs at Pin "D10" used for SDCARD
        cs = machine.Pin(cs_pin, machine.Pin.OUT, value=1)
        sd = sdcard.SDCard(spi, cs)
        vfs = os.VfsFat(sd)
        os.mount(vfs, "/sdcard")

|

OneWire driver
--------------

The OneWire driver is implemented in software and works on all pins::

    from machine import Pin
    import onewire

    ow = onewire.OneWire(Pin(12)) # create a OneWire bus on GPIO12
    ow.scan()               # return a list of devices on the bus
    ow.reset()              # reset the bus
    ow.readbyte()           # read a byte
    ow.writebyte(0x12)      # write a byte on the bus
    ow.write('123')         # write bytes on the bus
    ow.select_rom(b'12345678') # select a specific device by its ROM code

There is a specific driver for DS18S20 and DS18B20 devices::

    import time, ds18x20
    ds = ds18x20.DS18X20(ow)
    roms = ds.scan()
    ds.convert_temp()
    time.sleep_ms(750)
    for rom in roms:
        print(ds.read_temp(rom))

Be sure to put a 4.7k pull-up resistor on the data line.  Note that
the ``convert_temp()`` method must be called each time you want to
sample the temperature.

DHT driver
----------

The DHT driver is implemented in software and works on all pins::

    import dht
    import machine

    d = dht.DHT11(machine.Pin(4))
    d.measure()
    d.temperature() # eg. 23 (°C)
    d.humidity()    # eg. 41 (% RH)

    d = dht.DHT22(machine.Pin(4))
    d.measure()
    d.temperature() # eg. 23.6 (°C)
    d.humidity()    # eg. 41.3 (% RH)

Be sure to have a 4.7k pull-up resistor on the data line. Some
DHT module have that already on their board.


Ethernet driver
---------------

All MIMXRT boards except the MIMXRT1011 based boards and Teensy4.0 support Ethernet. Example on usage::

    import network

    lan = network.LAN(0)
    lan.active(True)

If there is a DHCP server in the LAN, the IP address is supplied by that server. Otherwise, the 
IP address can be set with lan.ifconfig(). The default address is 192.168.0.1.
Teensy 4.1 does not have an Ethernet jack on the board, but PJRC offers a low cost
adapter for self-assembly. The Seeed ARCH MIX board has no PHY hardware on the board.
However, you can attach external PHY interfaces. By default, the firmware for Seeed ARCH MIX
uses the driver for a LAN8720 PHY. The MIMXRT1170_EVK is equipped with two Ethernet ports.
They are addressed as LAN(0) for the 100M port and LAN(1) for the 1G port.

For details of the network interface refer to the class :std:ref:`network.LAN <mimxrt_network.LAN>`.

Transferring files
------------------

Files can be transferred to the i.MXRT devices for instance with the mpremote
tool or using an SD card. If Ethernet is available, you can as well use
ftp. See the MicroPython forum for the FTP server or other community-supported
alternatives to transfer files to an i.MXRT board, like rshell or Thonny.