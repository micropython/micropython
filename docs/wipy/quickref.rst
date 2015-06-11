.. only:: port_wipy

    .. _quickref_:

Quick reference for the WiPy
============================

.. image:: https://raw.githubusercontent.com/wipy/wipy/master/docs/PinOUT.png
    :alt: WiPy pinout and alternate functions table
    :width: 800px

General board control
---------------------

See :mod:`pyb`. ::

    import pyb

    help(pyb) # display all members from the pyb module
    pyb.delay(50) # wait 50 milliseconds
    pyb.millis() # number of milliseconds since boot-up
    pyb.freq() # get the CPU frequency
    pyb.unique_id() # return the 6-byte unique id of the board (it's MAC address)

Pins and GPIO
-------------

See :ref:`pyb.Pin <pyb.Pin>`. ::

    from pyb import Pin

    # initialize GPIO2 in gpio mode (af=0) and make it an output
    p_out = Pin('GPIO2', af=0, mode=Pin.OUT)
    p_out.high()
    p_out.low()
    p_out.toggle()

    # make GPIO1 an input with the pull-up enabled
    p_in = Pin('GPIO1', af = 0, mode=Pin.IN, type = Pin.STD_PU)
    p_in.value() # get value, 0 or 1

Timers
------

See :ref:`pyb.Timer <pyb.Timer>` and :ref:`pyb.Pin <pyb.Pin>`. ::

    from pyb import Timer
    from pyb import Pin

    tim = Timer(1, mode=Timer.PERIODIC)
    tim_a = tim.channel(Timer.A, freq=1000)
    tim_a.time() # get the value in microseconds
    tim_a.freq(1) # 1 Hz
    
    p_out = Pin('GPIO2', af=0, mode=Pin.OUT)
    tim_a.callback(handler=lambda t: p_out.toggle())

PWM (pulse width modulation)
----------------------------

See :ref:`pyb.Pin <pyb.Pin>` and :ref:`pyb.Timer <pyb.Timer>`. ::

    from pyb import Timer
    from pyb import Pin

    # assign GPIO25 to alternate function 5 (PWM)
    p_out = Pin('GPIO25', af=9, type=Pin.STD)

    # timer 2 in PWM mode and width must be 16 buts
    tim = Timer(2, mode=Timer.PWM, width=16)
    
    # enable channel A @1KHz with a 50% duty cycle
    tim_a = tim.channel(Timer.A, freq=1000, duty_cycle=50)

ADC (analog to digital conversion)
----------------------------------

See :ref:`pyb.ADC <pyb.ADC>`. ::

    from pyb import ADC

    adc = ADC(1)
    adc.read() # read value, 0-4095

UART (serial bus)
-----------------

See :ref:`pyb.Pin <pyb.Pin>` and :ref:`pyb.UART <pyb.UART>`. ::

    from pyb import Pin, UART

    # first assign TX and RX to the correct pins
    Pin('GPIO1', af=3, mode=Pin.STD_PU)    # TX
    Pin('GPIO2', af=3, mode=Pin.STD_PU)    # RX

    uart = UART(1, 9600)
    uart.write('hello')
    uart.read(5) # read up to 5 bytes

SPI bus
-------

See :ref:`pyb.Pin <pyb.Pin>` and :ref:`pyb.SPI <pyb.SPI>`. ::

    from pyb import Pin, SPI

    # first assign CLK, MISO, MOSI, CS to the correct pins
    Pin('GPIO14', af=7, mode=Pin.STD)    # CLK
    Pin('GPIO15', af=7, mode=Pin.STD)    # MISO
    Pin('GPIO16', af=7, mode=Pin.STD)    # MOSI
    Pin('GPIO17', af=7, mode=Pin.STD)    # NSS/CS

    # configure the SPI master @ 2MHz
    spi = SPI(1, SPI.MASTER, baudrate=200000, polarity=0, phase=0)
    spi.send('hello')
    spi.recv(5) # receive 5 bytes on the bus
    spi.send_recv('hello') # send a receive 5 bytes

I2C bus
-------

See :ref:`pyb.Pin <pyb.Pin>` and :ref:`pyb.I2C <pyb.I2C>`. ::

    from pyb import Pin, I2C

    # first assign SCL and SDA to the correct pins
    Pin('GPIO23', af=9, mode=Pin.STD_PU)  # SCL
    Pin('GPIO24', af=9, mode=Pin.STD_PU)  # SDA

    # configure the I2C bus
    i2c = I2C(1, I2C.MASTER, baudrate=100000)
    i2c.scan() # returns list of slave addresses
    i2c.send('hello', 0x42) # send 5 bytes to slave with address 0x42
    i2c.recv(5, 0x42) # receive 5 bytes from slave
    i2c.mem_read(2, 0x42, 0x10) # read 2 bytes from slave 0x42, slave memory 0x10
    i2c.mem_write('xy', 0x42, 0x10) # write 2 bytes to slave 0x42, slave memory 0x10

Watchdog timer (WDT)
--------------------

See :ref:`pyb.WDT <pyb.WDT>`. ::

    from pyb import WDT

    # enable the WDT with a timeout of 5s (1s is the minimum)
    wdt = WDT(5000)
    wdt.kick()
    
Real time clock (RTC)
---------------------

See :ref:`pyb.RTC <pyb.RTC>` and ``pyb.Sleep``. ::

    from pyb import RTC, Sleep

    rtc = pyb.RTC()
    rtc.datetime((2014, 5, 1, 4, 13, 0, 0, 0))
    print(rtc.datetime())

    def some_handler (rtc_obj):
        # trigger the callback again in 30s
        rtc_obj.callback(value=30000, handler=some_handler)

    # create a RTC alarm that expires in 30s
    rtc.callback(value=30000, handler=some_handler, wakes=Sleep.SUSPENDED)

    # go into suspended mode waiting for the RTC alarm to expire and wake us up
    Sleep.suspend()

SD card
-------

See :ref:`pyb.SD <pyb.SD>`. ::

    from pyb import SD

    # SD card pins need special configuration so we pass 'em to the constructor
    # data pin, data af, clock pin, clock af, cmd pin, cmd af
    sd = SD('GPIO15', 8, 'GPIO16', 8, 'GPIO17', 8)
    sd.enable()

WLAN (WiFi) 
-----------

See :ref:`network.WLAN <network.WLAN>` and ``pyb.Sleep``. ::

    from network import WLAN
    from pyb import Sleep

    # configure the WLAN subsystem in station mode (the default is AP)
    wifi = WLAN(WLAN.STA)
    # go for fixed IP settings
    wifi.ifconfig('192.168.0.107', '255.255.255.0', '192.168.0.1', '8.8.8.8')
    wifi.scan()     # scan for available netrworks
    wifi.connect(ssid='mynetwork', security=2, key='mynetworkkey')
    while not wifi.isconnected():
        pass
    print(wifi.ifconfig())
    # enable wake on WLAN
    wifi.callback(wakes=Sleep.SUSPENDED)
    # go to sleep
    Sleep.suspend()
    # now, connect to the FTP or the Telnet server and the WiPy will wake-up

Sleep and power modes control
-----------------------------

See ``pyb.Sleep``. ::

    from pyb import Sleep

    Sleep.idle()        # lowest sleep mode (~12mA), any interrupts wakes it up
    Sleep.suspend()     # everything except for WLAN is powered down (~950uA)
                        # wakes from Pin, RTC or WLAN

    Sleep.hibernate()   # deepest sleep mode, mcu starts from reset. Wakes from Pin and RTC.

Heart beat LED
-----------------------------

See :ref:`pyb.HeartBeat <pyb.HeartBeat>`. ::

    from pyb import HeartBeat

    # disable the heart beat indication (you are free to use this led connected to GPIO25)
    HeartBeat.disable()
    # enable the heart beat again
    HeartBeat.enable()
