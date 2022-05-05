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

Use the :mod:`time <time>` module::

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
- 0-14 for the Olimex RT1010Py board, or 'D0'-'Dxx' and 'A0'-'Ann'
- 'J3_xx', 'J4_xx', 'J5_xx' for the Seeed ARCH MIX board,

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
which can be used to invert a pin.  Useful for illuminating active-low LEDs
using ``on()`` or ``value(1)``.

UART (serial bus)
-----------------

See :ref:`machine.UART <machine.UART>`. ::

    from machine import UART

    uart1 = UART(1, baudrate=115200)
    uart1.write('hello')  # write 5 bytes
    uart1.read(5)         # read up to 5 bytes

The i.MXRT has up to eight hardware UARTs, but not every board exposes all
TX and RX pins for users.  The pin assignment of UARTs to pins is fixed.
The UARTs are numbered 1..8.  The rx/tx pins are assigned according to the
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
Olimex RT1010Py       -          RxD/TxD       D5/D6           -
Seeed ARCH MIX        -        J3_19/J3_20  J4_16/J4_17  J4_06/J4_07
================  ===========  ===========  ===========  ===========

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
Olimex RT1010Py        -            -         -        -        -
Seeed ARCH MIX    J4_10/J4_11  J5_08/J5_12    -        -        -
================  ===========  ===========  =======  =======  =====

PWM (pulse width modulation)
----------------------------

The i.MXRT has up to four dedicated PWM modules with four FLEXPWM submodules each
and up to four QTMR modules with four channels, which can be used to generate
a PWM signal or signal pair.

The PWM functions are provided by the :ref:`machine.PWM <machine.PWM>` class.
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


PWM Constructor
```````````````

.. class:: PWM(dest, freq, duty_u16, duty_ns, *, center, align, invert, sync, xor, deadtime)
  :noindex:

    Construct and return a new PWM object using the following parameters:

      - *dest* is the entity on which the PWM is output, which is usually a
        :ref:`machine.Pin <machine.Pin>` object, but a port may allow other values,
        like integers or strings, which designate a Pin in the machine.PIN class.
        *dest* is either a single object or a two element object tuple.
        If the object tuple is specified, the two pins act in complementary
        mode. These two pins must be the A/B channels of the same submodule.

    PWM objects are either provided by a FLEXPWM module or a QTMR module.
    The i.MXRT devices have either two or four FLEXPWM and QTMR modules.
    Each FLEXPWM module has four submodules with three channels, each,
    called A, B and X.  Each QTMR module has four channels.
    Each FLEXPWM submodule or QTMR channel may be set to different parameters.
    Not every channel is routed to a board pin.  Details are listed below.

    Setting *freq* affects the three channels of the same FLEXPWM submodule.
    Only one of *duty_u16* and *duty_ns* should be specified at a time.

    Keyword arguments:

      - *freq* should be an integer which sets the frequency in Hz for the
        PWM cycle. The valid frequency range is 15 Hz resp. 18Hz resp. 24Hz up to > 1 MHz.
      - *duty_u16* sets the duty cycle as a ratio ``duty_u16 / 65536``.
        The duty cycle of a X channel can only be changed, if the A and B channel
        of the respective submodule is not used. Otherwise the duty_16 value of the
        X channel is 32768 (50%).
      - *duty_ns* sets the pulse width in nanoseconds. The limitation for X channels
        apply as well.
      - *center*\=value. An integer sets the center of the pulse within the pulse period.
        The range is 0-65535. The resulting pulse will last from center - duty_u16/2 to
        center + duty_u16/2.
      - *align*\=value. Shortcuts for the pulse center setting, causing the pulse either at
        the center of the frame (value=0), the leading edge at the begin (value=1) or the
        trailing edge at the end of a pulse period (value=2).
      - *invert*\=True|False channel_mask. Setting a bit in the mask inverts the respective channel.
        Bit 0 inverts the first specified channel, bit 2 the second. The default is 0.
      - *sync*\=True|False. If a channel of a module's submodule 0 is already active, other
        submodules of the same module can be forced to be synchronous to submodule 0. Their
        pulse period start then at at same clock cycle. The default is False.
      - *xor*\=0|1|2. If set to 1 or 2, the channel will output the XOR'd signal from channels
        A or B. If set to 1 on channel A or B, both A and B will show the same signal. If set
        to 2, A and B will show alternating signals. For details and an illustration, please
        refer to the MCU's reference manual, chapter "Double Switching PWMs".
      - *deadtime*\=time_ns. This setting affects complementary channels and defines a deadtime
        between an edge of a first channel and the edge of the next channel, in which both
        channels are set to low. That allows connected H-bridges to switch off one side
        of a push-pull driver before switching on the other side.

PWM Methods
```````````

The methods are identical to the generic :ref:`machine.PWM <machine.PWM>` class,
with additional keyword arguments to the init() method, matchings those of the constructor.

Each FLEX submodule or QTMR module may run at different frequencies.  The PWM signal
is created by dividing the pwm_clk signal by an integral factor, according to the formula::

    f = pwm_clk / (2**n * m)

with n being in the range of 0..7, and m in the range of 2..65536. pmw_clk is 125Mhz
for MIMXRT1010/1015/1020, 150 MHz for MIMXRT1050/1060/1064 and 160MHz for MIMXRT1170.
The lowest frequency is pwm_clk/2**23 (15, 18, 20Hz). The highest frequency with
U16 resolution is pwm_clk/2**16 (1907, 2288, 2441 Hz), the highest frequency
with 1 percent resolution is pwm_clk/100 (1.25, 1.5, 1.6 MHz). The highest achievable
frequency is pwm_clk/3 for the A/B channels, and pwm_clk/2 for the X channels and QTMR
signal.

PWM Pin Assignment
``````````````````

Pins are specified in the same way as for the Pin class.  The following tables show
the assignment of the board Pins to PWM modules:

===========   ==========   ==========   ======    ==============   ======
Pin/ MIMXRT   1010         1015         1020      1050/1060/1064   1170
===========   ==========   ==========   ======    ==============   ======
D0            -            Q1/1         F1/1/B    -                -
D1            -            Q1/0         F1/1/A    -                -
D2            F1/3/B       F1/3/A       -         F1/3/B           -
D3            F1/3/A       F1/0/A       F2/3/B    F4/0/A           F1/2/A
D4            F1/3/A (*)   Q1/2         Q2/1      F2/3/A           Q4/2
D5            F1/0/B (*)   F1/0/B       F2/3/A    F1/3/A           F1/2/B
D6            -            F1/2/B       F2/0/A    Q3/2             F1/0/A
D7            -            -            F1/0/A    Q3/3             -
D8            F1/0/A       F1/1/B       F1/0/B    F1/1/X           Q4/3
D9            F1/1/B (*)   F1/2/A       F2/0/B    F1/0/X           F1/0/B
D10           F1/3/B       -            F2/2/B    F1/0/B (*)       F2/2/B
D11           F1/2/A       -            F2/1/A    F1/1/A (*)       -
D12           F1/2/B       -            F2/1/B    F1/1/B (*)       -
D13           F1/3/A       -            F2/2/A    F1/0/A (*)       F2/2/A
D14           F1/0/B       -            -         F2/3/B           -
D15           F1/0/A       -            -         F2/3/A           -
A0            -            -            F1/2/A    -                -
A1            F1/3/X       F1/3/B       F1/2/B    -                -
A2            F1/2/X       F1/3/A       F1/3/A    -                -
A3            -            F1/2/A       F1/3/B    -                -
A4            -            -            -         Q3/1             -
A5            -            -            -         Q3/0             -
D31           -            -            -         -                F1/2/B
D32           -            -            -         -                F1/2/A
D33           -            -            -         -                F1/1/B
D34           -            -            -         -                F1/1/A
D35           -            -            -         -                F1/0/B
D36           -            -            -         -                F1/0/A
===========   ==========   ==========   ======    ==============   ======

Pins denoted with (*) are by default not wired at the board.

====   ==========  ====   ==========
Pin    Teensy 4.0  Pin    Teensy 4.1
====   ==========  ====   ==========
0      F1/1/X      0      F1/1/X
1      F1/0/X      1      F1/0/X
2      F4/2/A      2      F4/2/A
3      F4/2/B      3      F4/2/B
4      F2/0/A      4      F2/0/A
5      F2/1/A      5      F2/1/A
6      F2/2/A      6      F2/2/A
7      F1/3/B      7      F1/3/B
8      F1/3/A      8      F1/3/A
9      F2/2/B      9      F2/2/B
10     Q1/0        10     Q1/0
11     Q1/2        11     Q1/2
12     Q1/1        12     Q1/1
13     Q2/0        13     Q2/0
14     Q3/2        14     Q3/2
15     Q3/3        15     Q3/3
18     Q3/1        18     Q3/1
19     Q3/0        19     Q3/0
22     F4/0/A      22     F4/0/A
23     F4/1/A      23     F4/1/A
24     F1/2/X      24     F1/2/X
25     F1/3/X      25     F1/3/X
28     F3/1/B      28     F3/1/B
29     F3/1/A      29     F3/1/A
33     F2/0/B      33     F2/0/B
-      -           36     F2/3/A
-      -           37     F2/3/B
DAT1   F1/1/B      42     F1/1/B
DAT0   F1/1/A      43     F1/1/A
CLK    F1/0/B      44     F1/0/B
CMD    F1/0/A      45     F1/0/A
DAT2   F1/2/A      46     F1/2/A
DAT3   F1/2/B      47     F1/2/B
-      -           48     F1/0/B
-      -           49     F1/2/A
-      -           50     F1/2/B
-      -           51     F3/3/B
-      -           52     F1/1/B
-      -           53     F1/1/A
-      -           54     F3/0/A
====   ==========  ====   ==========

=========  ==============
Pin        Seeed ARCH MIX
=========  ==============
J3_04      Q4/3
J3_10      Q1/3
J3_12      Q2/3
J3_13      Q3/3
J3_16      Q3/0
J3_17      Q3/1
J3_19      F1/3/X
J3_20      F1/2/X
J4_08      F4/0/A
J4_09      F4/1/A
J4_16      Q3/2
J4_17      Q3/3
J5_32      Q1/0
J5_28      Q1/1
J5_29      Q1/2
J5_30      Q2/0
J5_04      Q2/1
J5_05      Q2/3
J5_06      F2/0/A
J5_07      F2/0/B
J5_08      F2/1/A
J5_12      F2/1/B
J5_13      F2/2/A
J5_14      F2/2/B
J5_23      F1/3/A
J5_24      F1/3/B
J5_25      F2/3/A
J5_26      F2/3/B
J5_42      Q3/0
J5_43      Q3/1
J5_50      F1/0/X
LED_RED    F2/3/A
LED_GREEN  F1/3/A
LED_BLUE   F1/3/B
=========  ==============

=========  ===============
Pin        Olimex RT1010PY
=========  ===============
D0           -
D1         F1/0/B
D2         F1/0/A
D3         F1/1/B
D4         F1/1/A
D5         F1/2/B
D6         F1/2/A
D7         F1/3/B
D8         F1/3/A
D9            -
D10        F1/0/B
D11        F1/0/A
D12        F1/1/B
D13        F1/1/A
D14           -
A0            -
A1         F1/2/B
A2         F1/2/A
A3         F1/3/B
A4         F1/3/A
SDI        F1/3/X
SDO        F1/2/X
CS0        F1/1/X
SCK        F1/0/X
=========  ===============

Legend:

* Qm/n:    QTMR module m, channel n
* Fm/n/l:  FLEXPWM module m, submodule n, channel l. The pulse at a X channel
  is always aligned to the period start.

Pins without a PWM signal are not listed.  A signal may be available at more
than one Pin.  FlexPWM pins may also be pure CPU pin, not assigned to a board
signal.  In that case the PWM output is disabled.  The PWM channel of a submodule
0 may still be used as synchronization source for other channels of the same
module, unless used by another peripheral.

Submodule 0 pins for i.MX RT1011:

==================  =======
Pin                 Channel
==================  =======
Pin.cpu.GPIO_01     B
Pin.cpu.GPIO_02     A
Pin.cpu.GPIO_AD_12  X
Pin.cpu.GPIO_SD_01  B
Pin.cpu.GPIO_SD_02  A
==================  =======

Submodule 0 pins for i.MX RT1021:

=====================  ==================
Pin                    Module & Channel
=====================  ==================
Pin.cpu.GPIO_AD_B1_06  FLEXPWM1 Channel A
Pin.cpu.GPIO_AD_B1_07  FLEXPWM1 Channel B
Pin.cpu.GPIO_EMC_26    FLEXPWM1 Channel A
Pin.cpu.GPIO_EMC_27    FLEXPWM1 Channel B
Pin.cpu.GPIO_AD_B0_14  FLEXPWM2 Channel A
Pin.cpu.GPIO_AD_B0_15  FLEXPWM2 Channel B
Pin.cpu.GPIO_EMC_10    FLEXPWM2 Channel X
Pin.cpu.GPIO_EMC_38    FLEXPWM2 Channel A
Pin.cpu.GPIO_EMC_39    FLEXPWM2 Channel B
=====================  ==================

Submodule 0 pins for i.MX RT1052, i.MX RT1062 and i.MX RT1064:

=====================  ==================
Pin                    Module & Channel
=====================  ==================
Pin.cpu.GPIO_AD_B0_02  FLEXPWM1 Channel X
Pin.cpu.GPIO_EMC_23    FLEXPWM1 Channel A
Pin.cpu.GPIO_EMC_24    FLEXPWM1 Channel B
Pin.cpu.GPIO_SD_B0_00  FLEXPWM1 Channel A
Pin.cpu.GPIO_SD_B0_01  FLEXPWM1 Channel B
Pin.cpu.GPIO_B0_06     FLEXPWM2 Channel A
Pin.cpu.GPIO_B0_07     FLEXPWM2 Channel B
Pin.cpu.GPIO_EMC_06    FLEXPWM2 Channel A
Pin.cpu.GPIO_EMC_07    FLEXPWM2 Channel B
Pin.cpu.GPIO_EMC_29    FLEXPWM3 Channel A
Pin.cpu.GPIO_EMC_30    FLEXPWM3 Channel B
Pin.cpu.GPIO_AD_B1_08  FLEXPWM4 Channel A
Pin.cpu.GPIO_EMC_00    FLEXPWM4 Channel A
Pin.cpu.GPIO_EMC_01    FLEXPWM4 Channel B
=====================  ==================

Submodule 0 pins for i.MX RT1176

======================  ======================
Pin                     Module & Channel
======================  ======================
Pin.cpu.GPIO_EMC_B1_00  FLEXPWM4 Channel A (*)
Pin.cpu.GPIO_EMC_B1_01  FLEXPWM4 Channel B (*)
Pin.cpu.GPIO_EMC_B1_06  FLEXPWM2 Channel A (*)
Pin.cpu.GPIO_EMC_B1_07  FLEXPWM2 Channel B (*)
Pin.cpu.GPIO_EMC_B1_23  FLEXPWM1 Channel A (*)
Pin.cpu.GPIO_EMC_B1_24  FLEXPWM1 Channel B (*)
Pin.cpu.GPIO_EMC_B1_29  FLEXPWM3 Channel A (*)
Pin.cpu.GPIO_EMC_B1_30  FLEXPWM3 Channel B (*)
Pin.cpu.GPIO_AD_00      FLEXPWM1 Channel A
Pin.cpu.GPIO_AD_01      FLEXPWM1 Channel B
Pin.cpu.GPIO_AD_24      FLEXPWM2 Channel A
Pin.cpu.GPIO_AD_25      FLEXPWM2 Channel B
======================  ======================

(*) Pin used for SDRAM

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

Hardware SPI bus
----------------

There are up to four hardware SPI channels that allow faster transmission
rates (up to 90Mhz).  The SPI signals have fixed assignments to GPIO pins.
It depends on the board design, which SPI's signals are exposed to the user, as
detailed in the table below.  The signal order in the table is: CS0, CS1, MOSI, MISO, CLK.

=================  =========================  =======================  ===============
Board / Pin        SPI0                       SPI1                     SPI2
=================  =========================  =======================  ===============
Teensy 4.0         10/-/11/12/13              0/-/26/1/27                    -
Teensy 4.1         10/37/11/12/13             0/-/26/1/27              -/29/50/54/49
MIXMXRT1010-EVK    D10/D7/D11/D12/D13                -                       -
MIXMXRT1015-EVK    D10/-/D11/D12/D13                 -                       -
MIXMXRT1020-EVK    D10/-/D11/D12/D13          A3/D0/A5/A4/A0                 -
MIXMXRT1050-EVK    D10/-/D11/D12/D13 (*)            -                        -
MIXMXRT1050-EVKB   D10/-/D11/D12/D13 (*)            -                        -
MIXMXRT1060-EVK    D10/-/D11/D12/D13 (*)            -                        -
MIXMXRT1064-EVK    D10/-/D11/D12/D13 (*)            -                        -
MIXMXRT1170-EVK    D10/-/D11/D12/D13          D28/-/D25/D24/D26        -/-/D14/D15/D24
Olimex RT1010Py             -                 CS0/-/SDO/SDI/SCK        SDCARD with CS1
Seeed ARCH MIX     J4_12/-/J4_14/J4_13/J4_15  J3_09/J3_05/J3_08_J3_11
=================  =========================  =======================  ===============

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
   only worked up to 60 MHz.

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

There are up to four hardware I2C channels that allow faster transmission rates
and support the full I2C protocol.  The I2C signals have fixed assignments to GPIO pins.
It depends on the board design, which I2C's signals are exposed to the user, as
detailed in the table below.  The signal order in the table is: SDA, SCL.

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
Olimex RT1010Py      -          SDA1/SCL1    SDA2/SCL2     -        -
Seeed ARCH MIX     J3_17/J3_16  J4_06/J4_07  J5_05/J5_04   -        -
=================  ===========  ===========  ===========  =======  =======

Hardware I2C is accessed via the :ref:`machine.I2C <machine.I2C>` class and
has the same methods as software SPI above::

    from machine import I2C

    i2c = I2C(0, 400_000)
    i2c.writeto(0x76, b"Hello World")

I2S bus
-------

See :ref:`machine.I2S <machine.I2S>`. Example using a Teensy 4.1 board with a simple
external Codec like UDA1334.::

    from machine import I2S, Pin
    i2s = I2S(2, sck=Pin(26), ws=Pin(27), sd=Pin(7),
        mode=I2S.TX, bts=16,format=I2S.STEREO,
        rate=44100,ibuf=40000)
    i2s.write(buf)             # write buffer of audio samples to I2S device


Example for using I2S with a MIMXRT10xx_DEV board::

    from machine import I2S, I2C, Pin
    import wm8960

    i2c=I2C(0)

    wm=wm8960.WM8960(i2c, sample_rate=SAMPLE_RATE_IN_HZ,
        adc_sync=wm8960.sync_dac,
        swap=wm8960.swap_input)

    i2s = I2S(1, sck=Pin("SCK_TX"), ws=Pin("WS_TX"), sd=Pin("SD_RX"),
        mck=Pin("MCK),mode=I2S.RX, bts=16,format=I2S.MONO,
        rate=32000,ibuf=10000)
    i2s.readinto(buf)          # fill buffer with audio samples from I2S device

In this example, the input channels are swapped in the WM8960 driver, since the
on-board microphone is connected to the right channel, but mono audio is taken
from the left channel.  Note, that the sck and ws pins are connected to the TX
signals of the I2S bus.  That is intentional, since at the MW8960 codec these
signals are shared for RX and TX.

Example using the Teensy audio shield::

    from machine import I2C, I2S, Pin
    from sgtl5000 import CODEC
    i2s = I2S(1, sck=Pin(21), ws=Pin(20), sd=Pin(7), mck=Pin(23),
        mode=I2S.TX, bits=16,rate=44100,format=I2S.STEREO,
        ibuf=40000,
    )

    # configure the SGTL5000 codec
    i2c = I2C(0, freq=400000)
    codec = CODEC(0x0A, i2c)
    codec.mute_dac(False)
    codec.dac_volume(0.9, 0.9)
    codec.headphone_select(0)
    codec.mute_headphone(False)
    codec.volume(0.7, 0.7)

    i2s.write(buf)             # write buffer of audio samples to I2S device

The SGTL5000 codec used by the Teensy Audio shield uses the RX signals for both
RX and TX.  Note that the codec is initialized after the I2S device.  That is
essential since MCK is needed for its I2C operation and is provided by the I2S
controller.

MIMXRT boards may have 1 or 2 I2S buses available at the board connectors.
On MIMXRT1010 devices the bus numbers are 1 and 3.

Pin assignments for a few MIMXRT boards:

===============  ==  =====  ======== ======= ======= ======== ======= =======
Board            ID  MCK    SCK_TX   WS_TX   SD_TX   SCK_RX   WS_RX   SD_RX
===============  ==  =====  ======== ======= ======= ======== ======= =======
Teensy 4.0       1   23     26       27      7       21       20      8
Teensy 4.0       2   33     4        3       2       -        -       5
Teensy 4.1       1   23     26       27      7       21       20      8
Teensy 4.1       2   33     4        3       2       -        -       5
Seeed Arch MIX   1   J4_09  J4_14    J4_15   J14_13  J4_11    J4_10   J4_10
Olimex RT1010Py  1   D8     D6       D7      D4      D1       D2      D3
Olimex RT1010Py  3   -      D10      D9      D11     -        -       -
MIMXRT_DEV       1   "MCK"  "SCK_TX" "WS_TX" "SD_TX" "SCK_RX" "WS_RX" "SD_RX"
===============  ==  =====  ======== ======= ======= ======== ======= =======

Symbolic pin names are provided for the MIMXRT_10xx_DEV boards.
These are provided for the other boards as well.

Real time clock (RTC)
---------------------

See :ref:`machine.RTC <machine.RTC>`::

    from machine import RTC

    rtc = RTC()
    rtc.datetime((2017, 8, 23, 1, 12, 48, 0, 0)) # set a specific date and time
    rtc.datetime() # get date and time
    rtc.now() # return date and time in CPython format.

The i.MXRT MCU supports battery backup of the RTC.  By connecting a battery of
1.5-3.6V, time and date are maintained in the absence of the main power.  The
current drawn from the battery is ~20µA, which is rather high.  A CR2032 coin
cell will last for about one year.

SD card
-------

See :ref:`machine.SDCard <machine.SDCard>`::

    import machine, os

    sd = machine.SDCard()
    fs = os.VfsFat(sd)
    os.mount(fs, "/sd")  # mount
    os.listdir('/sd')    # list directory contents
    os.umount('/sd')     # eject

Note: The i.mx-rt 1011 and 1015 based boards do not support the ``machine.SDCard``
class.  For these, the SPI based driver ``sdcard.py`` from the MicroPython drivers
can be used.  When using it, you have to overdrive the CS pin of the SPI hardware
module.  Example::

    import os, sdcard, machine

    cs_pin = "D10"
    spi = machine.SPI(0) # SPI0 with cs at Pin "D10" used for SDCARD
    cs = machine.Pin(cs_pin, machine.Pin.OUT, value=1)
    sd = sdcard.SDCard(spi, cs)
    vfs = os.VfsFat(sd)
    os.mount(vfs, "/sdcard")

OneWire driver
--------------

The OneWire driver is implemented in software and works on all pins::

    from machine import Pin
    import onewire

    ow = onewire.OneWire(Pin(12)) # create a OneWire bus on GPIO12
    ow.scan()                     # return a list of devices on the bus
    ow.reset()                    # reset the bus
    ow.readbyte()                 # read a byte
    ow.writebyte(0x12)            # write a byte on the bus
    ow.write('123')               # write bytes on the bus
    ow.select_rom(b'12345678')    # select a specific device by its ROM code

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

Be sure to have a 4.7k pull-up resistor on the data line.  Some
DHT modules may already have one.

Ethernet driver
---------------

All MIMXRT boards except the MIMXRT1011 based boards and Teensy 4.0 support
Ethernet.  Example usage::

    import network

    lan = network.LAN(0)
    lan.active(True)

If there is a DHCP server in the LAN, the IP address is supplied by that server.
Otherwise, the IP address can be set with lan.ifconfig().  The default address
is 192.168.0.1.

Teensy 4.1 does not have an Ethernet jack on the board, but PJRC offers an
adapter for self-assembly.  The Seeed ARCH MIX board has no PHY hardware on the
board, however you can attach external PHY interfaces.  By default, the firmware
for Seeed Arch Mix uses the driver for a LAN8720 PHY.  The MIMXRT1170_EVK is
equipped with two Ethernet ports, which are addressed as LAN(0) for the 100M
port and LAN(1) for the 1G port.

For details of the network interface refer to the class :ref:`network.LAN <network.LAN>`.

Transferring files
------------------

Files can be transferred to the i.MXRT devices for instance with the ``mpremote``
tool or using an SD card.  If Ethernet is available, you can also use ftp.
See the MicroPython forum for the FTP server or other community-supported
alternatives to transfer files to an i.MXRT board, like rshell or Thonny.
