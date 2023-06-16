.. _psoc6_quickref:

Quick reference for the PSoC6™
==============================

.. image:: img/cy8cproto-062-4343w.jpg
    :alt: CY8CPROTO-062-4343W board
    :width: 640px

The CY8CPROTO-062-4343W PSoC6™ Board.

Below is a quick reference for PSoC6™ boards. If it is your first time
working with this port it may be useful to get an overview of the microcontroller:

.. toctree::
   :maxdepth: 1
   :includehidden:

   general.rst
   intro.rst
   installation.rst
   mpy-usage.rst
   feature_list.rst

.. warning::

    The PSoC6™ port is still in an early stage of development. It is expected any MicroPython built-in
    library to be supported, but not all libraries, modules and features have been implemented yet.
    For those modules relying on platform and hardware dependencies, only the listed and documented in this
    quick reference are certainyly supported.
    Check :ref:`here <psoc6_feature_list>` for a complete list of currently enabled or implemented modules as well as not yet implemented functionality.
    
    Please, consider opening an `issue <https://github.com/infineon/micropython/issues>`_ or
    `discussion <https://github.com/infineon/micropython/discussionsh>`_ on GitHub for any clarification
    required on available features or requests for missing ones. 

General board control
---------------------

The MicroPython REPL is accessed via the USB serial port. Paste mode (ctrl-E) is useful to paste a
large slab of Python code into the REPL. 

This port implements most of the methods described in the :mod:`machine` module. Tab-completion is useful to
find out what methods an instantiated object has. 

The :mod:`machine` module::

    import machine

    machine.freq()          # get the current frequency of the CPU
    
..
    TODO: add more machine module examples when implemented.    
    machine.freq(240000000) # set the CPU frequency to 240 MHz

..
    TODO: add ``psoc6`` module when implemented.    


Delay and timing
----------------

Use the :mod:`time <time>` module::

    import time

    time.sleep(1)           # sleep for 1 second
    time.sleep_ms(500)      # sleep for 500 milliseconds
    time.sleep_us(10)       # sleep for 10 microseconds
    start = time.ticks_ms() # get millisecond counter
    delta = time.ticks_diff(time.ticks_ms(), start) # compute time difference
    start = time.ticks_us() # get microsecond counter
    delta = time.ticks_diff(time.ticks_us(), start) # compute time difference


Timers
------

*Feature unavailable. Placeholder. To be completed.*

Pins and GPIO
-------------

Most of the methods (functions) and constants given in the :ref:`machine.Pin <machine.Pin>` class have been implemented in this port. Any functions in addition to those or function calls with ambiguous list of parameters have been documented here with suitable examples. 

The constructor
^^^^^^^^^^^^^^^
The constructor can be called in different flavors and configurations based on the number of arguments (parameters) passed. 

An instance of the :mod:`machine.Pin` class can be created by invoking the constructor with all the necessary parameters to fully configure the ``Pin``. 

::

    from machine import Pin

    p0 = Pin('P13_7', Pin.OUT, Pin.PULL_DOWN, value=Pin.STATE_LOW)   # create output pin on pin P13_7, 
                                                                     # with pull-down resistor enabled,
                                                                     # with initial value 0 (STATE_LOW)     


Additionally, with any combination of parameters (except the Pin number or ``id`` which should be passed mandatorily), a :mod:`machine.Pin` object with various configuration levels can be instantiated. In these cases, the :meth:`Pin.init` function has to be called proactively to set the other necessary configurations, as needed.

Moreover, a pre-configured pin object can be repurposed by calling the :meth:`Pin.init` function.


::

    from machine import Pin

    p0 = Pin('P13_7')                    # create pin object for pin P13_7. 
    p0.init(Pin.OUT, Pin.PULL_DOWN)      # set pin as output and enable pull-down resistor.
    p0.low()                             # set value low.     




Similar to CPython, the parameters can be passed in any order if keywords are used. On the other hand, in case of a non-keyword assignment if a parameter is not to be set, a ``None`` is to be passed in its place.


::

    from machine import Pin

    p0 = Pin(id='P13_7', value=Pin.STATE_LOW, pull=Pin.PULL_DOWN, mode=Pin.OUT)     # create output pin on pin P13_7, 
                                                                                    # with pull-down resistor enabled,
                                                                                    # with initial value 0 (STATE_LOW) 


    p1 = Pin('P0_0', Pin.OUT, None, value=Pin.STATE_HIGH)                           # create output pin on pin P0_0, 
                                                                                    # with pull as NONE,
                                                                                    # with initial value 1 (STATE_HIGH)                                                                       

Note that the parameters such as ``value`` can only be passed as keyword arguments.  

..
    TODO: add ``drive`` and ``alt`` when implemented

Methods
^^^^^^^

.. method:: Pin.toggle()

   Set pin value to its complement.


Constants
^^^^^^^^^
The following constants are used to configure the pin objects in addition to the ones mentioned in the :mod:`machine.Pin` class.

.. data:: Pin.STATE_LOW
          Pin.STATE_HIGH
          
    Selects the pin value.


Software I2C bus
----------------
Software I2C (using bit-banging) works on all output-capable pins, and is
accessed via :ref:`machine.SoftI2C <machine.SoftI2C>` ::

    from machine import Pin, SoftI2C

    i2c = SoftI2C(scl='P5_0', sda='P5_1', freq=100000)

    i2c.scan()              # scan for devices

    i2c.readfrom(0x3a, 4)   # read 4 bytes from device with address 0x3a
    i2c.writeto(0x3a, '12') # write '12' to device with address 0x3a

    buf = bytearray(10)     # create a buffer with 10 bytes
    i2c.writeto(0x3a, buf)  # write the given buffer to the peripheral


Hardware I2C bus
----------------
Hardware I2C works on the following listed pair of I2C pins

=====  ===========  ============
\      Default  
=====  ===========  ============
scl    P6_0           P9_0
sda    P6_1           P9_1
=====  ===========  ============


The driver is accessed via :ref:`machine.I2C <machine.I2C>`

The constructor
^^^^^^^^^^^^^^^
An instance of the :mod:`machine.I2C` class can be created by invoking the constructor with all the 
necessary parameters to fully configure the ``I2C``. By invoking the constructor, I2C peripheral is 
initialized and configured to work in master mode. 

::
    
    from machine import I2C
    i2c = I2C(0,scl='P6_0',sda='P6_1',freq=4000000)

Here, ``id=0`` should be passed mandatorily which selects the ``master`` mode operation.

::

    from machine import I2C
    i2c = I2C(0)  #I2C is initialized & configured with default scl, sda pin & frequency

::

    from machine import I2C
    i2c = I2C(0,scl='P9_0',sda='P9_1',freq=4000000)  #I2C is initialised & configured with given scl,sda pins & frequency

Methods
^^^^^^^
All the methods(functions) given in :ref:`machine.I2C <machine.I2C>` class have been implemented in this port except:

.. method:: I2C.init()

All the initialization & configurations are handled by the constructor. Hence ``init()`` is not required.


Real time clock (RTC)
---------------------

See :ref:`machine.RTC <machine.RTC>` ::

    from machine import RTC

    rtc = RTC()
    rtc.init((2023, 1, 1, 0, 0, 0, 0, 0)) # initialize rtc with specific date and time,
                                          # eg. 2023/1/1 00:00:00
    rtc.datetime((2017, 8, 23, 2, 12, 48, 0, 0)) # set a specific date and
                                                 # time, eg. 2017/8/23 1:12:48
    rtc.datetime() # get date and time
    rtc.now() # get current date and time

.. note::
    Setting a random week day in 'wday' field is not valid. The underlying library implements the logic to always
    calculate the right weekday based on the year, date and month passed. However, datetime() will not raise an error 
    for this, but rather re-write the field with last calculated actual value.

Network Module
--------------

The :mod:`network` module

See :ref:`network.WLAN <network.WLAN>`

The network module is used to configure the WiFi connection.The WiFi interface for the station mode is only configured for
this port.Create WLAN interface object using ::

    import network
    wlan = network.WLAN(network.STA_IF) # create station interface

Scan for the available wireless networks using 

::

   wlan.scan()             
    
Scan function returns a list of tuple information about access points
(ssid, bssid, channel, RSSI, security, hidden) .There are 7 levels of security:

 * ``0 - open``,
 * ``1 - WEP``,
 * ``2 - WPA``,
 * ``3 - WPA2``,
 * ``4 - WPA2_WPA``,
 * ``5 - WPA3``,
 * ``6 - WPS``,
 * ``7 - Unknown security``.          
    
These are the other functions available in the network module

::   

  wlan.active(True)           # activate the interface
  wlan.scan()                 # scan for access points
  wlan.isconnected()          # check if the station is connected to an AP
  wlan.connect('ssid', 'key') # connect to an AP
  wlan.disconnect()           # disconnect from the connected AP
  wlan.status()               # check the link status and returns 1 for linkup & 0 for linkdown
  wlan.ifconfig()             # get the interface's IP/netmask/gateway/DNS addresses
     
   
Here is a function you can run (or put in your boot.py file) to automatically connect to your WiFi network:

::

    def network_connect() :
        import network
        from utime import sleep,sleep_ms
        wlan = network.WLAN(network.STA_IF)
        if wlan.isconnected():
            print('[Network] already connected')
            return

        # enable and connect wlan
        wlan.active(True)
        wlan.connect('<ssid>','<key>')

        # wait for connection to establish
        sleep(5)
        for i in range(0,100):
        if not wlan.isconnected() and wlan.status() >= 0:
            print("[Network] Waiting to connect..")
            sleep(2)

        # check connection
        if not wlan.isconnected():
            print("[Network] Connection failed!")
        else:
            print(wlan.ifconfig())

PWM (pulse width modulation)
----------------------------

PWM can be enabled on all output capable pins.The frequency can range from 1Hz to 100MHz. As the frequency
increases the PWM resolution decreases. Refer `PSoC 6 MCU: CY8C62x8, CY8C62xA Datasheet <https://www.infineon.com/dgdl/Infineon-PSOC_6_MCU_CY8C62X8_CY8C62XA-DataSheet-v18_00-EN.pdf?fileId=8ac78c8c7d0d8da4017d0ee7d03a70b1>`_
for additional details regarding board specific PWM.

Use the :ref:`machine.PWM <machine.PWM>` class: 

The constructor can be called by passing the required arguments. All initialization and the configurations are handled by the constructor. Create PWM object using 

::  

    pwm = PWM('P9_0', freq=50, duty_u16=8192, invert=0) # PWM is initialised for the given pin with respective frequency & duty cycle.
    pwm1 = PWM('P9_1', freq=50, duty_ns=1000, invert=1)  # PWM is initialised for the given pin with respective frequency & pulse width & inverts the output.

    All four arguments has to be passed manadatorily to create PWM object. duty_u16 or duty_ns should be specified at a time.  

::

    from machine import PWM

    pwm = PWM('P9_0', freq=50, duty_u16=8192, invert=0) 
    print(pwm)                                # view PWM settings

    pwm.freq()                                # get current frequency
    pwm.freq(100)                             # set PWM frequency to 100 Hz

    pwm.duty_u16()                            # get current duty cycle, range 0-65535
    pwm.duty_u16(8192)                        # set duty cycle from 0 to 65535 as a ratio of duty_u16/65535, now 25%
    
    pwm.duty_ns()                             # get current pulse width in ns
    pwm.duty_ns(1000)                         # set the current pulse width in ns from 0 to 1000000000/freq

    pwm.init(freq=90,duty_us=100,invert=1)    # Modify the settings of PWM object
    pwm.deinit()                              # Deinitialisation of PWM pin

Software SPI bus
----------------

Software SPI (using bit-banging) works on all pins, and is accessed via the
:ref:`machine.SoftSPI <machine.SoftSPI>` class::

    from machine import Pin, SoftSPI

    # construct a SoftSPI bus on the given pins
    # polarity is the idle state of SCK
    # phase=0 means sample on the first edge of SCK, phase=1 means the second edge
    spi = SoftSPI(baudrate=100_000, polarity=1, phase=0, sck='P0_2', mosi='P0_0', miso='P0_1')

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

.. Warning::
   Currently *all* of ``sck``, ``mosi`` and ``miso`` *must* be specified when
   initialising Software SPI.

Hardware SPI bus
----------------
Hardware SPI works on the following listed pair of SPI pins. 

=====  ===========  ============  ============
\      Default  
=====  ===========  ============  ============
MOSI    P9_0           P6_0         P10_0
MISO    P9_1           P6_1         P10_1
SCK     P9_2           P6_2         P10_2
=====  ===========  ============  ============

Refer `PSoC 6 MCU: CY8C62x8, CY8C62xA Datasheet <https://www.infineon.com/dgdl/Infineon-PSOC_6_MCU_CY8C62X8_CY8C62XA-DataSheet-v18_00-EN.pdf?fileId=8ac78c8c7d0d8da4017d0ee7d03a70b1>`_
for additional details regarding all the SPI capable pins. 

The driver is accessed via :ref:`machine.SPI <machine.SPI>`

The constructor
^^^^^^^^^^^^^^^
An instance of the :mod:`machine.SPI` class can be created by invoking the constructor with all the 
necessary parameters to fully configure and initialise the ``SPI``. By invoking the constructor with no additional parameters 
SPI object is created with default settings or settings of previous initialisation if any. 

::
    
    from machine import SPI
    spi = SPI(0) # Default assignment: id=0, sck=P9_2  ,MOSI=P9_0, MISO=P9_1, baudrate=1000000, Polarity=0, Phase=0, bits=8, firstbit=SPI.MSB
    spi.init()

Management of a CS signal should happen in user code (via machine.Pin class).

::
    from machine import Pin
    cs = Pin('P9_3', mode=Pin.OUT, value=1)      # Create chip-select on pin P9_3
    cs(0)                                        # select the peripheral

Here, ``id=0`` should be passed mandatorily which selects the ``master`` mode operation.
If the constructor is called with any additional parameters then SPI object is created & initialised.

::    
    spi = SPI(0, baudrate=2000000) #object is created & initialised with baudrate=2000000 & default parameters
    spi = SPI(0, baudrate=1500000, polarity=1, phase=1, bits=8, firstbit=SPI.LSB, sck='P11_2', mosi='P11_0', miso='P11_1')

Methods
^^^^^^^
All the methods(functions) given in :ref:`machine.SPI <machine.SPI>` class have been implemented in this port


