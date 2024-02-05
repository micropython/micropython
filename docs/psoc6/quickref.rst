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

Pins and GPIO
-------------

Most of the methods (functions) and constants given in the :ref:`machine.Pin <machine.Pin>` class have been implemented in this port. Any functions in addition to those or function calls with ambiguous list of parameters have been documented here with suitable examples. 

The constructor
^^^^^^^^^^^^^^^
The constructor can be called in different flavors and configurations based on the number of arguments (parameters) passed. 

An instance of the :mod:`machine.Pin` class can be created by invoking the constructor with all the necessary parameters to fully configure the ``Pin``. 

::

    from machine import Pin

    p0 = Pin('P13_7', Pin.OUT, Pin.PULL_DOWN, value=0)   # create output pin on pin P13_7, 
                                                         # with pull-down resistor enabled,
                                                         # with initial value 0 (low)     


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

    p0 = Pin(id='P13_7', value=0, pull=Pin.PULL_DOWN, mode=Pin.OUT)     # create output pin on pin P13_7, 
                                                                        # with pull-down resistor enabled,
                                                                        # with initial value 0 (low) 


    p1 = Pin('P0_0', Pin.OUT, None, value=1)                           # create output pin on pin P0_0, 
                                                                       # with pull as NONE,
                                                                       # with initial value 1 (high)                                                                       

Note that the parameters such as ``value`` can only be passed as keyword arguments.  


.. note::

    The following constructor arguments are NOT supported in this port:
        * ``drive``. This configuration is automatically handled by the constructor and abstracted to the user.
        * ``alt``. Alternative functionality is directly handled by the respective machine peripherals classes.

Methods
^^^^^^^
.. method:: Pin.irq(handler=None, trigger=Pin.IRQ_FALLING | Pin.IRQ_RISING)

Here two arguments should be passed mandatorily. 
Trigger can be ``Pin.IRQ_FALLING`` or ``Pin.IRQ_RISING`` or ``PIN.IRQ_FALLING||PIN.IRQ_RISING``.

::
    
    from machine import Pin

    p0 = Pin('P0_4', value=1, pull=Pin.PULL_UP, mode=Pin.IN)          
    p1 = Pin('P13_7', value=0, pull=Pin.PULL_DOWN, mode=Pin.OUT)      

    p0.irq(handler=lambda t:p1.high(),trigger=Pin.IRQ_RISING) #configure an IRQ callback function P1.high() when there is a rising edge on pin object p0.
    

.. method:: Pin.toggle()

Set pin value to its complement.

Signal 
^^^^^^

There's a higher-level abstraction :ref:`machine.Signal <machine.Signal>`
which can be used to invert a pin. Useful for illuminating active-low LEDs
using ``on()`` or ``value(1)``.

.. warning::

    The :ref:`machine.Signal value()<machine.Signal>` getter functionality is not supported in this port, and the returned value is undefined. This is the same behavior as the :ref:`machine.Pin value()<machine.Pin>` for Pin.OUT mode, which is the object supporting the Signal object.

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

.. 
    TODO: This is only applicable to the CY8CPROTO-062-4343W. This does not belong here. 
    TODO: Define approach on how the user gets to know the pinout diagram, alternate function of each board
    - From board manual? 
    - From datasheet?
    - To create a pinout diagram? 


The driver is accessed via :ref:`machine.I2C <machine.I2C>`

The constructor
^^^^^^^^^^^^^^^
An instance of the :mod:`machine.I2C` class can be created by invoking the constructor with all the 
necessary parameters to fully configure the ``I2C``. By invoking the constructor, I2C peripheral is 
initialized and configured to work in master mode. The maximum supported frequency is 1 MHz.

::
    
    from machine import I2C
    i2c = I2C(scl='P6_0', sda='P6_1', freq=400000)

The ``scl`` and ``sda`` pins are the only mandatory arguments. The frequency is optional, and if not passed will be set to 400KHz by default.

.. note::
    The ``timeout`` option is currently not implemented in this port.

::

    from machine import I2C
    i2c = I2C(scl='P6_0', sda='P6_1')  #I2C is initialized & configured with default frequency

::

    from machine import I2C
    i2c = I2C(scl='P9_0', sda='P9_1', freq=400000)  #I2C is initialised & configured with given scl, sda pins & frequency

Methods
^^^^^^^
All the methods(functions) given in :ref:`machine.I2C <machine.I2C>` class have been implemented in this port except:

.. method:: I2C.init()

All the initialization & configurations are handled by the constructor. Hence ``init()`` is not required.

Hardware I2C bus slave
----------------------

The PSoC6™ port offers an additional class to implement an I2C slave device. An I2C master node connected to the slave can exchange data over I2C for the configured slave address and frequency.

.. warning:: 
    This is not part of the core MicroPython libraries. Therefore, not mapping any existing machine class API and neither supported by other ports.

.. note:: 
    | Part of the functionality of the I2C slave is based on hardware interrupts and callbacks. 
    | As explained in this :ref:`section <isr_rules>`, writing interrupts handlers in MicroPython is subject to multiple considerations depending on the actual hardware capabilities and limitations, the specific port implementation, and the overall application design and implementation.
    | These must kept in mind when implementing accurate timing and event synchronization between master and slave. 

The constructor
^^^^^^^^^^^^^^^

.. class:: I2CSlave(scl, sda, addr, freq)

    Constructs and returns a new I2C slave object using the following parameters.
   
    Required arguments: 
      - *scl* should be a pin name supporting the SCL functionality.
      - *sda* should be a pin name supporting the SDA functionality.
      - *addr* should be an 8 bits unsigned integer. 

    Optional arguments:
      - *freq* should be an integer which sets the maximum frequency
        for SCL. If not passed, by default is set to 400KHz.

    Example:
        ::
            
            from machine import I2CSlave
            
            i2c_slave = I2CSlave(scl="P6_4", sda="P6_5", addr=0x45)

Methods
^^^^^^^

.. method:: I2CSlave.deinit()

    Deinitialises the I2C slave.

.. method:: I2CSlave.conf_receive_buffer(buf)

    Configures the reception *buf* on an I2C slave. This is the buffer to which the master writes data to. 
    The user needs to setup a new buffer every time the buffer has been used up.

.. method:: I2CSlave.conf_transmit_buffer(buf)

    Configures the transmission *buf* on an I2C slave. This is the buffer from which the master reads data from.
    The user needs to setup a new buffer every time the buffer has been used up.

.. method:: I2CSlave.irq(callback, events, priority)

    Enables interrupts and the function handlers to be called upon different I2C bus events.

    Required arguments: 
      - *callback* should be a function handler that will be executed upon an interrupt event. The callback takes one argument which is the event causing the interrupt.
      - *events* is the I2C bus events that will be triggering the interrupt. Multiple ones can be configured by ORing the following constants:

        .. data:: I2CSlave.RD_EVENT
                I2CSlave.WR_EVENT
                I2CSlave.RD_BUF_IN_FIFO_EVENT
                I2CSlave.RD_BUF_EMPTY_EVENT
                I2CSlave.RD_CMPLT_EVENT
                I2CSlave.WR_CMPLT_EVENT
                I2CSlave.ERR_EVENT

    Optional arguments:
      - *priority* should be an unsigned integer with the priority of the interrupt.

    Example:
        ::

            def cback(event):
                if event == (event & I2CSlave.WR_EVENT):
                    print("i2c wr event")

            i2c_slave.irq(callback=cback, events=(I2CSlave.WR_EVENT | I2CSlave.WR_CMPLT_EVENT), priority=1)

.. method:: I2CSlave.irq_disable()

    Disables the I2C slave interrupts.


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

For some methods and constants, the PSoC6 network port implements certain specialization and slightly different behavior. This is explained in this section.

Methods
^^^^^^^

.. method:: WLAN.scan(ssid=None, bssid=None)

    The scan option accepts the following filters as keyword arguments, removing from scan results any network not matching these parameters values:

    * ``ssid``
    * ``bssid``

.. method:: WLAN.status('param')

    .. warning:: 
        The function does not provide status of the connection. Use the ``active()`` for that purpose. Any errors or failure are communicated when using the corresponding enable/disable or connect/disconnect functions.

    The following query parameters are allowed:
        * ``rssi``. Only for STA.
        * ``stations``. List of connected stations (only for AP).

.. method:: WLAN.config('param')
            WLAN.config(param=value, ...)

    Among the suggested parameters of the general network WLAN API, for this port, only these are available:

    * AP & STA query parameters

        - ``channel``
        - ``ssid``
        - ``security```
        - ``key/password``. Only for default AP key.
        - ``mac``
    * AP set parameters

        - ``channel``
        - ``ssid``
        - ``security```
        - ``key/password``. Only for default AP key.
        
    * STA has no configurable parameter.

Constants
^^^^^^^^^

Security modes constants:

.. data:: WLAN.OPEN
        WLAN.WEP
        WLAN.WPA
        WLAN.WPA2
        WLAN.WPA3
        WLAN.WPA2_WPA_PSK
        WLAN.SEC_UNKNOWN

.. note::
    Power modes configuration not implemented.
     
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

.. 
    TODO: This is only applicable to the CY8CPROTO-062-4343W. This does not belong here. 
    TODO: Define approach on how the user gets to know the pinout diagram, alternate function of each board
    - From board manual? 
    - From datasheet?
    - To create a pinout diagram? 

Refer `PSoC 6 MCU: CY8C62x8, CY8C62xA Datasheet <https://www.infineon.com/dgdl/Infineon-PSOC_6_MCU_CY8C62X8_CY8C62XA-DataSheet-v18_00-EN.pdf?fileId=8ac78c8c7d0d8da4017d0ee7d03a70b1>`_
for additional details regarding all the SPI capable pins. The pins ``sck``, ``mosi`` and ``miso`` *must* be specified when
initialising Software SPI.

The driver is accessed via :ref:`machine.SPI <machine.SPI>`

The constructor
^^^^^^^^^^^^^^^
An instance of the :mod:`machine.SPI` class can be created by invoking the constructor with all the 
necessary parameters to fully configure and initialize the ``SPI``. By invoking the constructor with no additional parameters 
SPI object is created with default settings or settings of previous initialization if any.

::
    
    from machine import SPI
    spi = SPI(0, sck='P11_2', mosi='P11_0', miso='P11_1') # Default assignment: id=0, SCK=P11_2  ,MOSI=P11_0, MISO=P11_1
    spi.init()

Management of a CS signal should happen in user code (via machine.Pin class).

::
    
    from machine import Pin
    cs = Pin('P9_3', mode=Pin.OUT, value=1)      # Create chip-select on pin P9_3
    cs(0)                                        # select the peripheral

Here, ``id=0`` should be passed mandatorily which selects the ``master`` mode operation.
If the constructor is called with any additional parameters then SPI object is created & initialised.

::    
    
    spi = SPI(0, sck='P11_2', mosi='P11_0', miso='P11_1', baudrate=2000000) #object is created & initialised with baudrate=2000000 & default parameters
    spi = SPI(0, baudrate=1500000, polarity=1, phase=1, bits=8, firstbit=SPI.LSB, sck='P11_2', mosi='P11_0', miso='P11_1')

Methods
^^^^^^^
All the methods(functions) given in :ref:`machine.SPI <machine.SPI>` class have been implemented in this port.

Timers
------

Hardware timer is supported.

Use the :mod:`machine.Timer` class::

    from machine import Timer
    import time
    tim = Timer(0) #Default assignment: period=9999, frequency=10000
    tim.init(period=2000, mode=Timer.ONE_SHOT, callback=lambda t:print(2)) #mode=Timer.PERIODIC in case of periodic timer


Here id=0 should be passed mandatorily.

ADC (analog to digital conversion)
----------------------------------

On the PSoC6, a single ADC block with id - '0' is available. The ADC functionality is available on the
following pins : "P10_0" - "P10_5".

Use the :ref:`machine.ADC <machine.ADC>` class::

    from machine import ADC

    adc = ADC("P10_0")             # create an ADC object on ADC pin
    val = adc.read_u16()           # read a raw analog value in the range 0-65535
    val = adc.read_uv()            # read an analog value in micro volts

The PSoC6 port also supports :ref:`machine.ADCBlock <machine.ADCBlock>` API to have control over the ADC configuration. Currently 

PSoC6 supports only 1 12-bit SAR ADC with the following channel to pin mapping and the defaults are set accordingly:

+---------+-------+
| Channel |  Pin  |
+=========+=======+
|    0    | P10_0 |
+---------+-------+
|    1    | P10_1 |
+---------+-------+
|    2    | P10_2 |
+---------+-------+
|    3    | P10_3 |
+---------+-------+
|    4    | P10_4 |
+---------+-------+
|    5    | P10_5 |
+---------+-------+

.. 
    TODO: This is only applicable to the CY8CPROTO-062-4343W. This does not belong here. 
    TODO: Define approach on how the user gets to know the pinout diagram, alternate function of each board
    - From board manual? 
    - From datasheet?
    - To create a pinout diagram? 

.. note::
    Arbitrary connection of ADC channels to GPIO is not supported. Specifying a pin that is not connected to this block, 
    or specifying a mismatched channel and pin, will raise an exception.

To use the APIs:
::
    
    from machine import ADCBlock

    adcBlock = ADCBlock(0, bits=12)             # create an ADCBlock 0 object
    adc = adcBlock.connect(0, "P10_0")          # connect channel 0 to pin P10_0
    val = adc.read_uv()                         # read an analog value in micro volts

I2S bus
-------

See :ref:`machine.I2S <machine.I2S>`. 

PSoC6 implements additional method which is explained below.

Methods
^^^^^^^
    .. method:: I2S.stop() 
    
    This method is to stop either the transmission or reception. This function should be called by user once the readinto or 
    write is done. 

::

    from machine import I2S, Pin
    import array
    buf=bytearray(10) #Initilase buffer with required values for transmission & as empty buffer for reception

    audio_out = I2S(0, sck="P13_1", ws="P13_2", sd="P13_3", mode=I2S.TX, bits=16, format=I2S.STEREO, rate=22050, ibuf=20000) #create I2S object
    num_written = audio_out.write(buf) # write buffer of audio samples to I2S device 
    

    audio_in = I2S(1, sck="P5_4", ws="P5_5", sd="P5_6", mode=I2S.RX, bits=16, format=I2S.STEREO, rate=22050, ibuf=20000) # create I2S object
    num_read = audio_in.readinto(buf)# fill buffer with audio samples from I2S device
    audio_out.stop() # stop transmission
    audio_in.stop()  # stop reception

PSoc6 supports two I2S Bus. So id could be either 0 or 1. Please note that if both instances are running together then the sample rate should
be either 8/16/32/48 or 22.05/44.01. Both the set of frequencies can't work together(For eg eg. 8 & 22.01 will cause error)
Supported sample rates are 8KHz, 16KHz, 32KHz, 48KHz, 22.05KHz, 44.1KHz.
PSoC6 supports only STEREO Mode. 

.. note:: I2S Blocking mode is only supported 

