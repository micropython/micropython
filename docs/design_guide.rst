Design Guide
============

MicroPython has created a great foundation to build upon and to make it even
better for beginners we've created CircuitPython. This guide covers a number of
ways the core and libraries are geared towards beginners.

Start libraries with the cookiecutter
-------------------------------------

Cookiecutter is a cool tool that lets you bootstrap a new repo based on another
repo. We've made one `here <https://github.com/adafruit/cookiecutter-adafruit-circuitpython>`_
for CircuitPython libraries that include configs for Travis CI and ReadTheDocs
along with a setup.py, license, code of conduct and readme.

.. code-block::sh

    # The first time
    pip install cookiecutter

    cookiecutter gh:adafruit/cookiecutter-adafruit-circuitpython

Module Naming
-------------

Adafruit funded libraries should be under the
`adafruit organization <https://github.com/adafruit>`_ and have the format
``Adafruit_CircuitPython_<name>`` and have a corresponding ``adafruit_<name>``
directory (aka package) or ``adafruit_<name>.py`` file (aka module).

Community created libraries should have the format ``CircuitPython_<name>`` and
not have the ``adafruit_`` module or package prefix.

Both should have the CircuitPython repository topic on GitHub.

.. _lifetime-and-contextmanagers:

Lifetime and ContextManagers
--------------------------------------------------------------------------------

A driver should be initialized and ready to use after construction. If the
device requires deinitialization, then provide it through ``deinit()`` and also
provide ``__enter__`` and ``__exit__`` to create a context manager usable with
``with``.

For example, a user can then use ``deinit()```::

    import digitalio
    import board

    led = digitalio.DigitalInOut(board.D13)
    led.direction = digitalio.DigitalInOut.Direction.OUT

    for i in range(10):
        led.value = True
        time.sleep(0.5)

        led.value = False
        time.sleep(0.5)
    led.deinit()

This will deinit the underlying hardware at the end of the program as long as no
exceptions occur.

Alternatively, using a ``with`` statement ensures that the hardware is deinitialized::

    import digitalio
    import board

    with digitalio.DigitalInOut(board.D13) as led:
        led.direction = digitalio.DigitalInOut.Direction.OUT

        for i in range(10):
            led.value = True
            time.sleep(0.5)

            led.value = False
            time.sleep(0.5)

Python's ``with`` statement ensures that the deinit code is run regardless of
whether the code within the with statement executes without exceptions.

For small programs like the examples this isn't a major concern because all
user usable hardware is reset after programs are run or the REPL is run. However,
for more complex programs that may use hardware intermittently and may also
handle exceptions on their own, deinitializing the hardware using a with
statement will ensure hardware isn't enabled longer than needed.

Verify your device
--------------------------------------------------------------------------------

Whenever possible, make sure device you are talking to is the device you expect.
If not, raise a ValueError. Beware that I2C addresses can be identical on
different devices so read registers you know to make sure they match your
expectation. Validating this upfront will help catch mistakes.

Getters/Setters
--------------------------------------------------------------------------------

When designing a driver for a device, use properties for device state and use
methods for sequences of abstract actions that the device performs. State is a
property of the device as a whole that exists regardless of what the code is
doing. This includes things like temperature, time, sound, light and the state
of a switch. For a more complete list see the sensor properties bullet below.

Another way to separate state from actions is that state is usually something
the user can sense themselves by sight or feel for example. Actions are
something the user can watch. The device does this and then this.

Making this separation clear to the user will help beginners understand when to
use what.

Here is more info on properties from
`Python <https://docs.python.org/3/library/functions.html#property>`_.

Design for compatibility with CPython
--------------------------------------------------------------------------------

CircuitPython is aimed to be one's first experience with code.  It will be the
first step into the world of hardware and software. To ease one's exploration
out from this first step, make sure that functionality shared with CPython shares
the same API. It doesn't need to be the full API it can be a subset. However, do
not add non-CPython APIs to the same modules. Instead, use separate non-CPython
modules to add extra functionality. By distinguishing API boundaries at modules
you increase the likelihood that incorrect expectations are found on import and
not randomly during runtime.

Example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

When adding extra functionality to CircuitPython to mimic what a normal
operating system would do, either copy an existing CPython API (for example file
writing) or create a separate module to achieve what you want. For example,
mounting and unmount drives is not a part of CPython so it should be done in a
module, such as a new ``filesystem``, that is only available in CircuitPython.
That way when someone moves the code to CPython they know what parts need to be
adapted.

Document inline
--------------------------------------------------------------------------------

Whenever possible, document your code right next to the code that implements it.
This makes it more likely to stay up to date with the implementation itself. Use
Sphinx's automodule to format these all nicely in ReadTheDocs. The cookiecutter
helps set these up.

Use `Sphinx flavor rST <http://www.sphinx-doc.org/en/stable/rest.html>`_ for markup.

Lots of documentation is a good thing but it can take a lot of space. To
minimize the space used on disk and on load, distribute the library as both .py
and .mpy, MicroPython and CircuitPython's bytecode format that omits comments.

Module description
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

After the license comment::

    """
    `<module name>` - <Short description>
    =================================================
    <Longer description.>
    """

Class description
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Documenting what the object does::

    class DS3231:
        """Interface to the DS3231 RTC."""

Renders as:

.. py:class:: DS3231
  :noindex:

  Interface to the DS3231 RTC.

Data descriptor description
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Comment is after even though its weird::

    lost_power = i2c_bit.RWBit(0x0f, 7)
    """True if the device has lost power since the time was set."""

Renders as:

.. py:attribute:: lost_power
  :noindex:

  True if the device has lost power since the time was set.

Method description
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

First line after the method definition::

    def turn_right(self, degrees):
        """Turns the bot ``degrees`` right.

           :param float degrees: Degrees to turn right
        """

Renders as:

.. py:method:: turn_right(degrees)
  :noindex:

  Turns the bot ``degrees`` right.

  :param float degrees: Degrees to turn right

Property description
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Comment comes from the getter::

    @property
    def datetime(self):
        """The current date and time"""
        return self.datetime_register

    @datetime.setter
    def datetime(self, value):
        pass

Renders as:

.. py:attribute:: datetime
  :noindex:

  The current date and time

Use BusDevice
--------------------------------------------------------------------------------

BusDevice is an awesome foundational library that manages talking on a shared
I2C or SPI device for you. The devices manage locking which ensures that a
transfer is done as a single unit despite CircuitPython internals and, in the
future, other Python threads. For I2C, the device also manages the device
address. The SPI device, manages baudrate settings, chip select line and extra
post-transaction clock cycles.

I2C Example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

  from adafruit_bus_device import i2c_device

  class Widget:
      """A generic widget."""

      def __init__(self, i2c):
          # Always on address 0x40.
          self.i2c_device = i2c_device.I2CDevice(i2c, 0x40)
          self.buf = bytearray(1)

      @property
      def register(self):
          """Widget's one register."""
          with self.i2c_device as i2c:
              i2c.writeto(b'0x00')
              i2c.readfrom_into(self.buf)
          return self.buf[0]


SPI Example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

  from adafruit_bus_device import spi_device

  class SPIWidget:
      """A generic widget with a weird baudrate."""

      def __init__(self, spi, chip_select):
          # chip_select is a pin reference such as board.D10.
          self.spi_device = spi_device.SPIDevice(spi, chip_select, baudrate=12345)
          self.buf = bytearray(1)

      @property
      def register(self):
          """Widget's one register."""
          with self.spi_device as spi:
              spi.write(b'0x00')
              i2c.readinto(self.buf)
          return self.buf[0]

Lots of small modules
--------------------------------------------------------------------------------

CircuitPython boards tend to have a small amount of internal flash and a small
amount of ram but large amounts of external flash for the file system. So, create
many small libraries that can be loaded as needed instead of one large file that
does everything.

Speed second
--------------------------------------------------------------------------------

Speed isn't as important as API clarity and code size. So, prefer simple APIs
like properties for state even if it sacrifices a bit of speed.

Avoid allocations in drivers
--------------------------------------------------------------------------------

Although Python doesn't require managing memory, its still a good practice for
library writers to think about memory allocations. Avoid them in drivers if
you can because you never know how much something will be called. Fewer
allocations means less time spent cleaning up. So, where you can, prefer
bytearray buffers that are created in ``__init__`` and used throughout the
object with methods that read or write into the buffer instead of creating new
objects. Unified hardware API classes such as `busio.SPI` are design to read and
write to subsections of buffers.

Its ok to allocate an object to return to the user. Just beware of causing more
than one allocation per call due to internal logic.

**However**, this is a memory tradeoff so do not do it for large or rarely used
buffers.

Examples
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ustruct.pack
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

Use `ustruct.pack_into` instead of `ustruct.pack`.

Sensor properties and units
--------------------------------------------------------------------------------

The `Adafruit Unified Sensor Driver Arduino library <https://learn.adafruit.com/using-the-adafruit-unified-sensor-driver/introduction>`_ has a
`great list <https://learn.adafruit.com/using-the-adafruit-unified-sensor-driver?view=all#standardised-si-units-for-sensor-data>`_
of measurements and their units. Use the same ones including the property name
itself so that drivers can be used interchangeably when they have the same
properties.

+-----------------------+-----------------------+-------------------------------------------------------------------------+
| Property name         | Python type           | Units                                                                   |
+=======================+=======================+=========================================================================+
| ``acceleration``      | (float, float, float) | x, y, z meter per second per second                                     |
+-----------------------+-----------------------+-------------------------------------------------------------------------+
| ``magnetic``          | float                 | micro-Tesla (uT)                                                        |
+-----------------------+-----------------------+-------------------------------------------------------------------------+
| ``orientation``       | (float, float, float) | x, y, z degrees                                                         |
+-----------------------+-----------------------+-------------------------------------------------------------------------+
| ``gyro``              | (float, float, float) | x, y, z radians per second                                              |
+-----------------------+-----------------------+-------------------------------------------------------------------------+
| ``temperature``       | float                 | degrees centigrade                                                      |
+-----------------------+-----------------------+-------------------------------------------------------------------------+
| ``distance``          | float                 | centimeters                                                             |
+-----------------------+-----------------------+-------------------------------------------------------------------------+
| ``light``             | float                 | SI lux                                                                  |
+-----------------------+-----------------------+-------------------------------------------------------------------------+
| ``pressure``          | float                 | hectopascal (hPa)                                                       |
+-----------------------+-----------------------+-------------------------------------------------------------------------+
| ``relative_humidity`` | float                 | percent                                                                 |
+-----------------------+-----------------------+-------------------------------------------------------------------------+
| ``current``           | float                 | milliamps (mA)                                                          |
+-----------------------+-----------------------+-------------------------------------------------------------------------+
| ``voltage``           | float                 | volts (V)                                                               |
+-----------------------+-----------------------+-------------------------------------------------------------------------+
| ``color``             | int                   | RGB, eight bits per channel (0xff0000 is red)                           |
+-----------------------+-----------------------+-------------------------------------------------------------------------+
| ``alarm``             | (time.struct, str)    | Sample alarm time and string to characterize frequency such as "hourly" |
+-----------------------+-----------------------+-------------------------------------------------------------------------+
| ``datetime``          | time.struct           | date and time                                                           |
+-----------------------+-----------------------+-------------------------------------------------------------------------+

Common APIs
--------------------------------------------------------------------------------

Outside of sensors, having common methods amongst drivers for similar devices
such as devices can be really useful. Its early days however. For now, try to
adhere to guidelines in this document. Once a design is settled on, add it as a
subsection to this one.

Adding native modules
--------------------------------------------------------------------------------

The Python API for a new module should be defined and documented in
``shared-bindings`` and define an underlying C API. If the implementation is
port-agnostic or relies on underlying APIs of another module, the code should
live in ``shared-module``. If it is port specific then it should live in ``common-hal``
within the port's folder. In either case, the file and folder structure should
mimic the structure in ``shared-bindings``.

MicroPython compatibility
--------------------------------------------------------------------------------

Keeping compatibility with MicroPython isn't a high priority. It should be done
when its not in conflict with any of the above goals.
