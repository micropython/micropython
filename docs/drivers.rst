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
their dependencies. he bundle is primarily geared to the Adafruit Express line
of boards which will feature a relatively large external flash. With Express
boards, its easy to copy them all onto the filesystem. However, if you don't
have enough space simply copy things over as they are needed.

The bundles are available `on GitHub <https://github.com/adafruit/Adafruit_CircuitPython_Bundle/releases>`_.

To install them:

#. `Download <https://github.com/adafruit/Adafruit_CircuitPython_Bundle/releases>`_
   and unzip the latest zip that's not a source zip.
#. Copy the ``lib`` folder to the ``CIRCUITPY`` or ``MICROPYTHON``.

Foundational Libraries
----------------------

These libraries provide critical functionality to many of the drivers below. It
is recommended to always have them installed onto the CircuitPython file system in
the ``lib/`` directory. Some drivers may not work without them.

.. toctree::

   Register Library <https://circuitpython.readthedocs.io/projects/register/en/latest/>
   BusDevice Library <https://circuitpython.readthedocs.io/projects/bus_device/en/latest/>

Helper Libraries
----------------

These libraries build on top of the low level APIs to simplify common tasks.

.. toctree::

   USB Human Interface Device (Keyboard and Mouse) <https://circuitpython.readthedocs.io/projects/hid/en/latest/>

Drivers
-------

Drivers provide easy access to sensors and other chips without requiring a
knowledge of the interface details of the chip itself.

.. toctree::

   NeoPixel <https://circuitpython.readthedocs.io/projects/neopixel/en/latest/>
   SimpleIO <https://circuitpython.readthedocs.io/projects/simpleio/en/latest/>
   RGB Displays <http://micropython-rgb.readthedocs.io/>
   SD Card <https://circuitpython.readthedocs.io/projects/sdcard/en/latest/>
   Analog-to-digital converters: ADS1015 and ADS1115 <http://micropython-ads1015.readthedocs.io/>
   DS3231 Real-time Clock (Precision RTC) <https://circuitpython.readthedocs.io/projects/ds3231/en/latest/>
   DS1307 Real-time Clock (5V RTC Breakout) <https://circuitpython.readthedocs.io/projects/ds1307/en/latest/>
   PCF8523 Real-time Clock (Adalogger RTC) <https://circuitpython.readthedocs.io/projects/pcf8523/en/latest/>
   TCS34725 Color Sensor <http://micropython-tcs34725.readthedocs.io/>
   TSL2561 Light Sensor <http://micropython-tsl2561.readthedocs.io/>
   PCA9685 Motor and Servo Controllers <http://micropython-pca9685.readthedocs.io/>
   HT16K33 LED Matrices and Segment Displays <http://micropython-ht16k33.readthedocs.io/>
   IS31FL3731 Charlieplexed LED Matrix <http://micropython-is31fl3731.readthedocs.io/>
   MAX7219 LED Matrix <http://micropython-max7219.readthedocs.io/>
