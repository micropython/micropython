.. _mimxrt_pinout:

Pinout for the i.MXRT machine modules
=====================================

.. _mimxrt_uart_pinout:

|

UART pin assignment
-------------------

The pin assignment of UARTs to pins is fixed.
The UARTs are numbered 0..8.  The rx/tx pins are assigned according to the
tables below:

=================  ===========  ===========  ===========  ===========
Board / Pin           UART0        UART1        UART2        UART3
=================  ===========  ===========  ===========  ===========
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
Adafruit Metro M7     -           D0/D1        D7/D3        A1/A0
Olimex RT1010Py       -          RxD/TxD       D7/D8        D5/D6
Seeed ARCH MIX        -         J3_19/J3_20  J4_16/J4_17  J4_06/J4_07
=================  ===========  ===========  ===========  ===========

|

================  ===========  ===========  =======  =======  =====
Board / Pin           UART4        UART5      UART6   UART7   UART8
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

.. _mimxrt_pwm_pinout:

|
|

PWM pin assignment
------------------

Pins are specified in the same way as for the Pin class.  The following tables show
the assignment of the board Pins to PWM modules:

===========  ==========  ==========  ======  ==========  ======  ========
Pin/ MIMXRT  1010        1015        1020    1050/60/64  1170    Metro M7
===========  ==========  ==========  ======  ==========  ======  ========
D0           -           Q1/1        F1/1/B  -           -       -
D1           -           Q1/0        F1/1/A  -           -       -
D2           F1/3/B      F1/3/A      -       F1/3/B      -       -
D3           F1/3/A      F1/0/A      F2/3/B  F4/0/A      F1/2/A  -
D4           F1/3/A (*)  Q1/2        Q2/1    F2/3/A      Q4/2    F1/0/B
D5           F1/0/B (*)  F1/0/B      F2/3/A  F1/3/A      F1/2/B  F1/0/A
D6           -           F1/2/B      F2/0/A  Q3/2        F1/0/A  -
D7           -           -           F1/0/A  Q3/3        -       -
D8           F1/0/A      F1/1/B      F1/0/B  F1/1/X      Q4/3    F1/3/A
D9           F1/1/B (*)  F1/2/A      F2/0/B  F1/0/X      F1/0/B  F1/3/B
D10          F1/3/B      -           F2/2/B  F1/0/B (*)  F2/2/B  F1/2/A
D11          F1/2/A      -           F2/1/A  F1/1/A (*)  -       F1/2/B
D12          F1/2/B      -           F2/1/B  F1/1/B (*)  -       F1/1/A
D13          F1/3/A      -           F2/2/A  F1/0/A (*)  F2/2/A  F1/1/B
D14          F1/0/B      -           -       F2/3/B      -       F1/0/B
D15          F1/0/A      -           -       F2/3/A      -       F1/0/A
A0           -           -           F1/2/A  -           -       -
A1           F1/3/X      F1/3/B      F1/2/B  -           -       -
A2           F1/2/X      F1/3/A      F1/3/A  -           -       -
A3           -           F1/2/A      F1/3/B  -           -       F1/3/B
A4           -           -           -       Q3/1        -       F1/2/X
A5           -           -           -       Q3/0        -       -
D31          -           -           -       -           F1/2/B  -
D32          -           -           -       -           F1/2/A  -
D33          -           -           -       -           F1/1/B  -
D34          -           -           -       -           F1/1/A  -
D35          -           -           -       -           F1/0/B  -
D36          -           -           -       -           F1/0/A  -
===========  ==========  ==========  ======  ==========  ======  ========

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

|

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

|

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

.. _mimxrt_spi_pinout:

|
|

Hardware SPI pin assignment
---------------------------

The SPI signals have fixed assignments to GPIO pins.
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
Adafruit Metro M7  -/-/MOSI/MISO/SCK                -                        -
Olimex RT1010Py             -                 CS0/-/SDO/SDI/SCK        SDCARD with CS1
Seeed ARCH MIX     J4_12/-/J4_14/J4_13/J4_15  J3_09/J3_05/J3_08_J3_11
=================  =========================  =======================  ===============

Pins denoted with (*) are by default not wired at the board. The CS0 and CS1 signals
are enabled with the keyword option cs=0 or cs=1 of the SPI object constructor.

.. _mimxrt_i2c_pinout:

|
|

Hardware I2C pin assignment
---------------------------

The I2C signals have fixed assignments to GPIO pins.
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
Adafruit Metro M7  D14/D15      D0/D1
Olimex RT1010Py      -          SDA1/SCL1    SDA2/SCL2     -        -
Seeed ARCH MIX     J3_17/J3_16  J4_06/J4_07  J5_05/J5_04   -        -
=================  ===========  ===========  ===========  =======  =======

.. _mimxrt_i2s_pinout:

|
|

Hardware I2S pin assignment
---------------------------

Pin assignments for a few MIMXRT boards:

=================  ==  =====  ======== ======= ======= ======== ======= =======
Board              ID  MCK    SCK_TX   WS_TX   SD_TX   SCK_RX   WS_RX   SD_RX
=================  ==  =====  ======== ======= ======= ======== ======= =======
Teensy 4.0         1   23     26       27      7       21       20      8
Teensy 4.0         2   33     4        3       2       -        -       5
Teensy 4.1         1   23     26       27      7       21       20      8
Teensy 4.1         2   33     4        3       2       -        -       5
Seeed Arch MIX     1   J4_09  J4_14    J4_15   J14_13  J4_11    J4_10   J4_10
Adafruit Metro M7  1   D8     D10      D9      D12     D14      D15     D13
Olimex RT1010Py    1   D8     D6       D7      D4      D1       D2      D3
Olimex RT1010Py    3   -      D10      D9      D11     -        -       -
MIMXRT_DEV         1   "MCK"  "SCK_TX" "WS_TX" "SD_TX" "SCK_RX" "WS_RX" "SD_RX"
=================  ==  =====  ======== ======= ======= ======== ======= =======

Symbolic pin names are provided for the MIMXRT_10xx_DEV boards.
These are provided for the other boards as well.
