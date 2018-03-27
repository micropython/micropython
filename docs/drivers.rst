.. _adafruit-libndrivers:

Additional Adafruit Libraries and Drivers on GitHub
===================================================

These are libraries and drivers available in separate GitHub repos. They are
designed for use with CircuitPython and may or may not work with
`MicroPython <https://micropython.org>`_.

.. _bundle_installation:

Bundle
------

We provide a bundle of all our libraries to ease installation of drivers and
their dependencies. The bundle is primarily geared to the Adafruit Express line
of boards which feature a relatively large external flash. With Express boards,
its easy to copy them all onto the filesystem. However, if you don't have
enough space simply copy things over as they are needed.

The bundles are available `on GitHub <https://github.com/adafruit/Adafruit_CircuitPython_Bundle/releases>`_.

To install them:

#. `Download <https://github.com/adafruit/Adafruit_CircuitPython_Bundle/releases>`_
   and unzip the latest zip that's not a source zip.
#. Copy the ``lib`` folder to the ``CIRCUITPY`` or ``MICROPYTHON``.

Foundational
------------

These libraries provide critical functionality to many of the drivers below. It
is recommended to always have them installed onto the CircuitPython file system in
the ``lib/`` directory. Some drivers may not work without them.

.. toctree::

   Register Library <https://circuitpython.readthedocs.io/projects/register/en/latest/>
   BusDevice Library <https://circuitpython.readthedocs.io/projects/busdevice/en/latest/>

Board-specific Helpers
----------------------

These libraries tie lower-level libraries together to provide an easy, out-of-box experience for
specific boards.

.. toctree::

    Adafruit CircuitPlayground Express <https://circuitpython.readthedocs.io/projects/circuitplayground/en/latest/>
    Adafruit FeatherWings <https://circuitpython.readthedocs.io/projects/featherwing/en/latest/>

Helper Libraries
-----------------

These libraries build on top of the low level APIs to simplify common tasks.

.. toctree::

    USB Human Interface Device (Keyboard and Mouse) <https://circuitpython.readthedocs.io/projects/hid/en/latest/>
    Waveform Generation <https://circuitpython.readthedocs.io/projects/waveform/en/latest/>
    OneWire <https://circuitpython.readthedocs.io/projects/onewire/en/latest/>
    Ring Tone Text Transfer Language (RTTTL) <https://circuitpython.readthedocs.io/projects/rtttl/en/latest/>
    InfraRed Remote <https://circuitpython.readthedocs.io/projects/irremote/en/latest/>
    Fancy LED (similar to FastLED) <https://circuitpython.readthedocs.io/projects/fancyled/en/latest/>
    SimpleIO <https://circuitpython.readthedocs.io/projects/simpleio/en/latest/>
    AVR programming <https://circuitpython.readthedocs.io/projects/avrprog/en/latest/>
    DC Motor and Servo <https://circuitpython.readthedocs.io/projects/motor/en/latest/>
    SD Card <https://circuitpython.readthedocs.io/projects/sd/en/latest/>

Blinky
--------

Multi-color led drivers.

.. toctree::

    NeoPixel <https://circuitpython.readthedocs.io/projects/neopixel/en/latest/>
    DotStar <https://circuitpython.readthedocs.io/projects/dotstar/en/latest/>
    WS2801 <https://circuitpython.readthedocs.io/projects/ws2801/en/latest/>

Displays
-------------

Drivers used to display information. Either pixel or segment based.

.. toctree::

    RGB Displays <https://circuitpython.readthedocs.io/projects/rgb_display/en/latest/>
    Character LCD <https://circuitpython.readthedocs.io/projects/charlcd/en/latest/>
    HT16K33 LED Matrices and Segment Displays <https://circuitpython.readthedocs.io/projects/ht16k33/en/latest/>
    IS31FL3731 Charlieplexed LED Matrix <https://circuitpython.readthedocs.io/projects/is31fl3731/en/latest/>
    MAX7219 LED Matrix <https://circuitpython.readthedocs.io/projects/max7219/en/latest/>
    SSD1306 OLED Driver <https://circuitpython.readthedocs.io/projects/ssd1306/en/latest/>

Real-time clocks
-----------------

Chips that keep current calendar time with a backup battery. The current date and time is available
through ``datetime``.

.. toctree::

    DS1307 Real-time Clock (5V RTC Breakout) <https://circuitpython.readthedocs.io/projects/ds1307/en/latest/>
    DS3231 Real-time Clock (Precision RTC) <https://circuitpython.readthedocs.io/projects/ds3231/en/latest/>
    PCF8523 Real-time Clock (Adalogger RTC) <https://circuitpython.readthedocs.io/projects/pcf8523/en/latest/>

Motion Sensors
----------------

Motion relating sensing including ``acceleration``, ``magnetic``, ``gyro``, and ``orientation``.

.. toctree::

    BNO055 Accelerometer, Magnetometer, Gyroscope and Absolution Orientation <https://circuitpython.readthedocs.io/projects/bno055/en/latest/>
    FXAS21002C Gyroscope <https://circuitpython.readthedocs.io/projects/fxas21002c/en/latest/>
    FXOS8700 Accelerometer <https://circuitpython.readthedocs.io/projects/fxos8700/en/latest/>
    GPS Global Position <https://circuitpython.readthedocs.io/projects/gps/en/latest/>
    LIS3DH Accelerometer <https://circuitpython.readthedocs.io/projects/lis3dh/en/latest/>
    LSM303 Accelerometer and Magnetometer <https://circuitpython.readthedocs.io/projects/lsm303/en/latest/>
    LSM9DS0 Accelerometer, Magnetometer, Gyroscope and Temperature <https://circuitpython.readthedocs.io/projects/lsm9ds0/en/latest/>
    LSM9DS1 Accelerometer, Magnetometer, Gyroscope and Temperature <https://circuitpython.readthedocs.io/projects/lsm9ds1/en/latest/>
    MMA8451 3 axis accelerometer <https://circuitpython.readthedocs.io/projects/mma8451/en/latest/>

Environmental Sensors
----------------------

Sense attributes of the environment including ``temperature``, ``relative_humidity``, ``pressure``,
equivalent carbon dioxide (``eco2`` / ``eCO2``), and total volatile organic compounds (``tvoc`` /
``TVOC``).

.. toctree::

    AM2320 Temperature and Humidity <https://circuitpython.readthedocs.io/projects/am2320/en/latest/>
    BME280 Temperature, Humidity and Pressure <https://circuitpython.readthedocs.io/projects/bme280/en/latest/>
    BME680 Temperature, Humidity, Pressure and Gas <https://circuitpython.readthedocs.io/projects/bme680/en/latest/>
    BMP280 Barometric Pressure and Altitude <https://circuitpython.readthedocs.io/projects/bmp280/en/latest/>
    CCS811 Air Quality <https://circuitpython.readthedocs.io/projects/ccs811/en/latest/>
    DHT Temperature and Humidity <https://circuitpython.readthedocs.io/projects/dht/en/latest/>
    DS18x20 Temperature <https://circuitpython.readthedocs.io/projects/ds18x20/en/latest/>
    MAX31865 Thermocouple Amplifier, Temperature <https://circuitpython.readthedocs.io/projects/max31865/en/latest/>
    MAX31855 Thermocouple Amplifier, Temperature <https://circuitpython.readthedocs.io/projects/max31855/en/latest/>
    MCP9808 Temperature <https://circuitpython.readthedocs.io/projects/mcp9808/en/latest/>
    MPL3115A2 Barometric Pressure, Altitude and Temperature Sensor <https://circuitpython.readthedocs.io/projects/mpl3115a2/en/latest/>
    SGP30 Air Quality <https://circuitpython.readthedocs.io/projects/sgp30/en/latest/>
    SHT31-D Temperature and Humidity <https://circuitpython.readthedocs.io/projects/sht31d/en/latest/>
    Si7021 Temperature and Humidity <https://circuitpython.readthedocs.io/projects/si7021/en/latest/>
    Thermistor Temperature <https://circuitpython.readthedocs.io/projects/thermistor/en/latest/>

Light Sensors
---------------

These sensors detect light related attributes such as ``color``, ``light`` (unit-less), and
``lux`` (light in SI lux).

.. toctree::

    APDS9960 Proximity, Light, RGB, and Gesture <https://circuitpython.readthedocs.io/projects/apds9960/en/latest/>
    AS726x Color Spectrum Sensor <https://circuitpython.readthedocs.io/projects/as726x/en/latest/>
    TCS34725 Color Sensor <https://circuitpython.readthedocs.io/projects/tcs34725/en/latest/>
    TSL2561 Light Sensor <https://circuitpython.readthedocs.io/projects/tsl2561/en/latest/>
    TSL2591 High Dynamic Range Light Sensor <https://circuitpython.readthedocs.io/projects/tsl2591/en/latest/>
    VCNL4010 Proximity and Light <https://circuitpython.readthedocs.io/projects/vcnl4010/en/latest/>
    VEML6070 UV Index <https://circuitpython.readthedocs.io/projects/veml6070/en/latest/>

Distance Sensors
------------------

These sensors measure the ``distance`` to another object and may also measure light level (``light`` and ``lux``).

.. toctree::

    VL6180x 5 - 100 mm <https://circuitpython.readthedocs.io/projects/vl6180x/en/latest/>
    VL53L0x ~30 - 1000 mm <https://circuitpython.readthedocs.io/projects/vl53l0x/en/latest/>

Radio
--------

These chips communicate to other's over radio.

.. toctree::

    RFM9x LoRa <https://circuitpython.readthedocs.io/projects/rfm9x/en/latest/>
    RFM69 Packet Radio <https://circuitpython.readthedocs.io/projects/rfm69/en/latest/>

IO Expansion
--------------

These provide functionality similar to `analogio`, `digitalio`, `pulseio`, and `touchio`.

.. toctree::

    Adafruit SeeSaw <https://circuitpython.readthedocs.io/projects/seesaw/en/latest/>
    ADS1x15 Analog-to-Digital Converter  <https://circuitpython.readthedocs.io/projects/ads1x15/en/latest/>
    DS2413 OneWire GPIO Expander <https://circuitpython.readthedocs.io/projects/ds2413/en/latest/>
    FocalTech Capacitive Touch <https://circuitpython.readthedocs.io/projects/focaltouch/en/latest/>
    MCP230xx GPIO Expander <https://circuitpython.readthedocs.io/projects/mcp230xx/en/latest/>
    MCP4725 Digital-to-Analog Converter <https://circuitpython.readthedocs.io/projects/mcp4725/en/latest/>
    PCA9685 16 x 12-bit PWM Driver <https://circuitpython.readthedocs.io/projects/pca9685/en/latest/>
    TLC5947 24 x 12-bit PWM Driver <https://circuitpython.readthedocs.io/projects/tlc5947/en/latest/>
    TLC59711 12 x 16-bit PWM Driver <https://circuitpython.readthedocs.io/projects/tlc59711/en/latest/>
    MPR121 Capacitive Touch Sensor <https://circuitpython.readthedocs.io/projects/mpr121/en/latest/>


Miscellaneous
----------------

.. toctree::

    Si4713 Stereo FM Transmitter <https://circuitpython.readthedocs.io/projects/si4713/en/latest/>
    AMG88xx Grid-Eye IR Camera <https://circuitpython.readthedocs.io/projects/amg88xx/en/latest/>
    Trellis 4x4 Keypad <https://circuitpython.readthedocs.io/projects/trellis/en/latest/>
    DRV2605 Haptic Motor Controller <https://circuitpython.readthedocs.io/projects/drv2605/en/latest/>
    MAX9744 Audio Amplifier  <https://circuitpython.readthedocs.io/projects/max9744/en/latest/>
    Si5351 Clock Generator <https://circuitpython.readthedocs.io/projects/si5351/en/latest/>
    Thermal Printer <https://circuitpython.readthedocs.io/projects/thermal_printer/en/latest/>
    VC0706 TTL Camera <https://circuitpython.readthedocs.io/projects/vc0706/en/latest/>
    INA219 High Side Current <https://circuitpython.readthedocs.io/projects/ina219/en/latest/>
    Fingerprint <https://circuitpython.readthedocs.io/projects/fingerprint/en/latest/>
