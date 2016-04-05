.. _quickref:

Quick reference for the ESP8266
===============================

.. image:: https://learn.adafruit.com/system/assets/assets/000/028/689/medium640/adafruit_products_pinoutstop.jpg
    :alt: Adafruit Feather HUZZAH board
    :width: 640px

The Adafruit Feather HUZZAH board (image attribution: Adafruit).

General board control
---------------------

The MicroPython REPL is on UART0 (GPIO1=TX, GPIO3=RX) at baudrate 115200.
Tab-completion is useful to find out what methods an object has.
Paste mode (ctrl-E) is useful to paste a large slab of Python code into
the REPL.

The ``machine`` module::

    import machine

    machine.freq()          # get the current frequency of the CPU
    machine.freq(160000000) # set the CPU frequency to 160 MHz

The ``esp`` module::

    import esp

    esp.osdebug(None)       # turn off vendor O/S debugging messages
    esp.osdebug(0)          # redirect vendor O/S debugging messages to UART(0)

Networking
----------

The ``network`` module::

    import network

    wlan = network.WLAN(network.STA_IF) # create station interface
    wlan.active(True)       # activate the interface
    wlan.scan()             # scan for access points
    wlan.isconnected()      # check if the station is connected to an AP
    wlan.connect('essid', 'password') # connect to an AP
    wlan.mac()              # get the interface's MAC adddress
    wlan.ifconfig()         # get the interface's IP/netmask/gw/DNS addresses

    ap = network.WLAN(network.AP_IF) # create access-point interface
    ap.active(True)         # activate the interface
    ap.config(essid='ESP-AP') # set the ESSID of the access point

A useful function for connecting to your local WiFi network is::

    def do_connect():
        import network
        wlan = network.WLAN(network.STA_IF)
        wlan.active(True)
        if not wlan.isconnected():
            print('connecting to network...')
            wlan.connect('essid', 'password')
            while not wlan.isconnected():
                pass
        print('network config:', wlan.ifconfig())

Once the network is established the ``socket`` module can be used
to create and use TCP/UDP sockets as usual.

Delay and timing
----------------

Use the ``time`` module::

    import time

    time.sleep(1)           # sleep for 1 second
    time.sleep_ms(500)      # sleep for 500 milliseconds
    time.sleep_us(10)       # sleep for 10 microseconds
    start = time.ticks_ms() # get millisecond counter
    delta = time.ticks_diff(start, time.ticks_ms()) # compute time difference

Timers
------

Virtual (RTOS-based) timers are supported. Use the ``machine.Timer`` class
with timer ID of -1::

    from machine import Timer

    tim = Timer(-1)
    tim.init(period=5000, mode=Timer.ONE_SHOT, callback=lambda t:print(1))
    tim.init(period=2000, mode=Timer.PERIODIC, callback=lambda t:print(2))

The period is in milliseconds.

Pins and GPIO
-------------

Use the ``machine.Pin`` class::

    from machine import Pin

    p0 = Pin(0, Pin.OUT)    # create output pin on GPIO0
    p0.high()               # set pin to high
    p0.low()                # set pin to low
    p0.value(1)             # set pin to high

    p2 = Pin(2, Pin.IN)     # create input pin on GPIO2
    print(p2.value())       # get value, 0 or 1

    p4 = Pin(4, Pin.IN, Pin.PULL_UP) # enable internal pull-up resistor
    p5 = Pin(5, Pin.OUT, value=1) # set pin high on creation

Available pins are: 0, 1, 2, 3, 4, 5, 12, 13, 14, 15, 16, which correspond
to the actual GPIO pin numbers of ESP8266 chip. Note that many end-user
boards use their own adhoc pin numbering (marked e.g. D0, D1, ...). As
MicroPython supports different boards and modules, physical pin numbering
was chosen as the lowest common denominator. For mapping between board
logical pins and physical chip pins, consult your board documentation.

Note that Pin(1) and Pin(3) are REPL UART TX and RX respectively.
Also note that Pin(16) is a special pin (used for wakeup from deepsleep
mode) and may be not available for use with higher-level classes like
``Neopixel``.

PWM (pulse width modulation)
----------------------------

PWM can be enabled on all pins except Pin(16).  There is a single frequency
for all channels, with range between 1 and 1000 (measured in Hz).  The duty
cycle is between 0 and 1023 inclusive.

Use the ``machine.PWM`` class::

    from machine import Pin, PWM

    pwm0 = PWM(Pin(0))      # create PWM object from a pin
    pwm0.freq()             # get current frequency
    pwm0.freq(1000)         # set frequency
    pwm0.duty()             # get current duty cycle
    pwm0.duty(200)          # set duty cycle
    pwm0.deinit()           # turn off PWM on the pin

    pwm2 = PWM(Pin(2), freq=500, duty=512) # create and configure in one go

ADC (analog to digital conversion)
----------------------------------

ADC is available on a dedicated pin.
Note that input voltages on the ADC pin must be between 0v and 1.0v.

Use the ``machine.ADC`` class::

    from machine import ADC

    adc = ADC(0)            # create ADC object on ADC pin
    adc.read()              # read value, 0-1024

SPI bus
-------

The SPI driver is implemented in software and works on all pins::

    from machine import Pin, SPI

    # construct an SPI bus on the given pins
    # polarity is the idle state of SCK
    # phase=0 means sample on the first edge of SCK, phase=1 means the second
    spi = SPI(baudrate=100000, polarity=1, phase=0, sck=Pin(0), mosi=Pin(2), miso=Pin(4))

    spi.init(baudrate=200000) # set the baudrate

    spi.read(10)            # read 10 bytes on MISO
    spi.read(10, 0xff)      # read 10 bytes while outputing 0xff on MOSI

    buf = bytearray(50)     # create a buffer
    spi.readinto(buf)       # read into the given buffer (reads 50 bytes in this case)
    spi.readinto(buf, 0xff) # read into the given buffer and output 0xff on MOSI

    spi.write(b'12345')     # write 5 bytes on MOSI

    buf = bytearray(4)      # create a buffer
    spi.write_readinto(b'1234', buf) # write to MOSI and read from MISO into the buffer
    spi.write_readinto(buf, buf) # write buf to MOSI and read MISO back into buf

I2C bus
-------

The I2C driver is implemented in software and works on all pins::

    from machine import Pin, I2C

    # construct an I2C bus
    i2c = I2C(scl=Pin(5), sda=Pin(4), freq=100000)

    i2c.readfrom(0x3a, 4)   # read 4 bytes from slave device with address 0x3a
    i2c.writeto(0x3a, '12') # write '12' to slave device with address 0x3a

    buf = bytearray(10)     # create a buffer with 10 bytes
    i2c.writeto(0x3a, buf)  # write the given buffer to the slave

    i2c.readfrom(0x3a, 4, stop=False) # don't send a stop bit after reading
    i2c.writeto(0x3a, buf, stop=False) # don't send a stop bit after writing

OneWire driver
--------------

The OneWire driver is implemented in software and works on all pins::

    from machine import Pin
    import onewire

    ow = onewire.OneWire(Pin(12)) # create a OneWire bus on GPIO12
    ow.scan()               # return a list of devices on the bus
    ow.reset()              # reset the bus
    ow.read_byte()          # read a byte
    ow.read_bytes(5)        # read 5 bytes
    ow.write_byte(0x12)     # write a byte on the bus
    ow.write_bytes('123')   # write bytes on the bus
    ow.select_rom(b'12345678') # select a specific device by its ROM code

There is a specific driver for DS18B20 devices::

    import time
    ds = onewire.DS18B20(ow)
    roms = ds.scan()
    ds.start_measure()
    time.sleep_ms(750)
    for rom in roms:
        print(ds.get_temp(rom))

Be sure to put a 4.7k pull-up resistor on the data line.

NeoPixel driver
---------------

Use the ``neopixel`` module::

    from machine import Pin
    from neopixel import NeoPixel

    pin = Pin(0, Pin.OUT)   # set GPIO0 to output to drive NeoPixels
    np = NeoPixel(pin, 8)   # create NeoPixel driver on GPIO0 for 8 pixels
    np[0] = (255, 255, 255) # set the first pixel to white
    np.write()              # write data to all pixels
    r, g, b = np[0]         # get first pixel colour

    import neopixel
    neopixel.demo(np)       # run a demo

For low-level driving of a NeoPixel::

    import esp
    esp.neopixel_write(pin, grb_buf, is800khz)
