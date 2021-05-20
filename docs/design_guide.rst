.. role:: strike

Design Guide
============

This guide covers a variety of development practices for CircuitPython core and library APIs. These
APIs are both `built-into CircuitPython
<https://github.com/adafruit/circuitpython/tree/main/shared-bindings>`_ and those that are
`distributed on GitHub <https://github.com/search?utf8=%E2%9C%93&q=topic%3Acircuitpython&type=>`_
and in the `Adafruit <https://github.com/adafruit/Adafruit_CircuitPython_Bundle>`_ and `Community
<https://github.com/adafruit/CircuitPython_Community_Bundle/>`_ bundles. Consistency with these
practices ensures that beginners can learn a pattern once and apply it throughout the CircuitPython
ecosystem.

Start libraries with the cookiecutter
-------------------------------------

Cookiecutter is a tool that lets you bootstrap a new repo based on another repo.
We've made one `here <https://github.com/adafruit/cookiecutter-adafruit-circuitpython>`_
for CircuitPython libraries that include configs for Travis CI and ReadTheDocs
along with a setup.py, license, code of conduct, readme among other files.

.. code-block::sh

    # The first time
    pip install cookiecutter

    cookiecutter gh:adafruit/cookiecutter-adafruit-circuitpython

Cookiecutter will provide a series of prompts relating to the library and then create a new
directory with all of the files. See `the CircuitPython cookiecutter README
<https://github.com/adafruit/cookiecutter-adafruit-circuitpython#introduction>`_ for more details.

Module Naming
-------------

Adafruit funded libraries should be under the
`adafruit organization <https://github.com/adafruit>`_ and have the format
``Adafruit_CircuitPython_<name>`` and have a corresponding ``adafruit_<name>``
directory (aka package) or ``adafruit_<name>.py`` file (aka module).

If the name would normally have a space, such as "Thermal Printer", use an underscore instead
("Thermal_Printer"). This underscore will be used everywhere even when the separation between
"adafruit" and "circuitpython" is done with a ``-``. Use the underscore in the cookiecutter prompts.

Community created libraries should have the repo format ``CircuitPython_<name>`` and
not have the ``adafruit_`` module or package prefix.

Both should have the CircuitPython repository topic on GitHub.

Terminology
-----------

As our Code of Conduct states, we strive to use "welcoming and inclusive
language." Whether it is in documentation or in code, the words we use matter.
This means we disfavor language that due to historical and social context can
make community members and potential community members feel unwelcome.

There are specific terms to avoid except where technical limitations require it.
While specific cases may call for other terms, consider using these suggested
terms first:

+--------------------+---------------------+
| Preferred          | Deprecated          |
+====================+=====================+
| Main (device)      | :strike:`Master`    |
+--------------------+---------------------+
| Peripheral         | :strike:`Slave`     |
+--------------------+                     +
| Sensor             |                     |
+--------------------+                     +
| Secondary (device) |                     |
+--------------------+---------------------+
| Denylist           | :strike:`Blacklist` |
+--------------------+---------------------+
| Allowlist          | :strike:`Whitelist` |
+--------------------+---------------------+

Note that "technical limitations" refers e.g., to the situation where an
upstream library or URL has to contain those substrings in order to work.
However, when it comes to documentation and the names of parameters and
properties in CircuitPython, we will use alternate terms even if this breaks
tradition with past practice.


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
    import time

    led = digitalio.DigitalInOut(board.D13)
    led.direction = digitalio.Direction.OUTPUT

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
    import time

    with digitalio.DigitalInOut(board.D13) as led:
        led.direction = digitalio.Direction.OUTPUT

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
If not, raise a RuntimeError. Beware that I2C addresses can be identical on
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

Exceptions and asserts
--------------------------------------------------------------------------------

Raise an appropriate `Exception <https://docs.python.org/3/library/exceptions.html#bltin-exceptions>`_,
along with a useful message, whenever a critical test or other condition fails.

Example::

    if not 0 <= pin <= 7:
        raise ValueError("Pin number must be 0-7.")

If memory is constrained and a more compact method is needed, use `assert`
instead.

Example::

    assert 0 <= pin <= 7, "Pin number must be 0-7."

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

When adding a new module for additional functionality related to a CPython
module do NOT simply prefix it with u. This is not a large enough differentiation
from CPython. This is the MicroPython convention and they use u* modules
interchangeably with the CPython name. This is confusing. Instead, think up a
new name that is related to the extra functionality you are adding.

For example, storage mounting and unmounting related functions were moved from
``uos`` into a new `storage` module. Terminal related functions were moved into
`multiterminal`. These names better match their functionality and do not
conflict with CPython names. Make sure to check that you don't conflict with
CPython libraries too. That way we can port the API to CPython in the future.

Example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

When adding extra functionality to CircuitPython to mimic what a normal
operating system would do, either copy an existing CPython API (for example file
writing) or create a separate module to achieve what you want. For example,
mounting and unmount drives is not a part of CPython so it should be done in a
module, such as a new ``storage`` module, that is only available in CircuitPython.
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
    `<module name>`
    =================================================

    <Longer description>

    * Author(s):

    Implementation Notes
    --------------------


    **Hardware:**

    * `Adafruit Device Description
      <hyperlink>`_ (Product ID: <Product Number>)

    **Software and Dependencies:**

    * Adafruit CircuitPython firmware for the supported boards:
      https://circuitpython.org/downloads

    * Adafruit's Bus Device library:
      https://github.com/adafruit/Adafruit_CircuitPython_BusDevice

    * Adafruit's Register library:
      https://github.com/adafruit/Adafruit_CircuitPython_Register

    """


Class description
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

At the class level document what class does and how to initialize it::

    class DS3231:
        """DS3231 real-time clock.

           :param ~busio.I2C i2c_bus: The I2C bus the DS3231 is connected to.
           :param int address: The I2C address of the device. Defaults to :const:`0x40`
        """

        def __init__(self, i2c_bus, address=0x40):
            self._i2c = i2c_bus


Renders as:

.. py:class:: DS3231(i2c_bus, address=64)
    :noindex:

    DS3231 real-time clock.

    :param ~busio.I2C i2c_bus: The I2C bus the DS3231 is connected to.
    :param int address: The I2C address of the device. Defaults to :const:`0x40`


Documenting Parameters
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Although there are different ways to document class and functions definitions in Python,
the following is the prevalent method of documenting parameters
for CircuitPython libraries. When documenting class parameters you should use the
following structure:

.. code-block:: sh

    :param param_type param_name: Parameter_description


param_type
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The type of the parameter. This could be among other `int`, `float`, `str` `bool`, etc.
To document an object in the CircuitPython domain, you need to include a ``~`` before the
definition as shown in the following example:

.. code-block:: sh

    :param ~busio.I2C i2c_bus: The I2C bus the DS3231 is connected to.


To include references to CircuitPython modules, cookiecutter creates an entry in the
intersphinx_mapping section in the ``conf.py`` file located within the ``docs`` directory.
To add different types outside CircuitPython you need to include them in the intersphinx_mapping::


    intersphinx_mapping = {
        "python": ("https://docs.python.org/3.4", None),
        "BusDevice":("https://circuitpython.readthedocs.io/projects/busdevice/en/latest/", None,),
        "CircuitPython": ("https://circuitpython.readthedocs.io/en/latest/", None),
    }

The intersphinx_mapping above includes references to Python, BusDevice and CircuitPython
Documentation

When the parameter have two different types, you should reference them as follows::


    class Character_LCD:
        """Base class for character LCD

           :param ~digitalio.DigitalInOut rs: The reset data line
           :param ~pwmio.PWMOut,~digitalio.DigitalInOut blue: Blue RGB Anode

        """

        def __init__(self, rs, blue):
            self._rc = rs
            self.blue = blue


Renders as:

.. py:class:: Character_LCD(rs, blue)
    :noindex:

    Base class for character LCD

    :param ~digitalio.DigitalInOut rs: The reset data line
    :param ~pwmio.PWMOut,~digitalio.DigitalInOut blue: Blue RGB Anode


param_name
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Parameter name used in the class or method definition


Parameter_description
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Parameter description. When the parameter defaults to a particular value, it is good
practice to include the default::

    :param int pitch: Pitch value for the servo. Defaults to :const:`4500`


Attributes
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Attributes are state on objects. (See `Getters/Setters`_ above for more discussion
about when to use them.) They can be defined internally in a number of different
ways. Each approach is enumerated below with an explanation of where the comment
goes.

Regardless of how the attribute is implemented, it should have a short
description of what state it represents including the type, possible values and/or
units. It should be marked as ``(read-only)`` or ``(write-only)`` at the end of
the first line for attributes that are not both readable and writable.

Instance attributes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Comment comes from after the assignment::

    def __init__(self, drive_mode):
        self.drive_mode = drive_mode
        """
        The pin drive mode. One of:

        - `digitalio.DriveMode.PUSH_PULL`
        - `digitalio.DriveMode.OPEN_DRAIN`
        """

Renders as:

.. py:attribute:: drive_mode
    :noindex:

    The pin drive mode. One of:

    - `digitalio.DriveMode.PUSH_PULL`
    - `digitalio.DriveMode.OPEN_DRAIN`

Property description
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Comment comes from the getter::

    @property
    def datetime(self):
        """The current date and time as a `time.struct_time`."""
        return self.datetime_register

    @datetime.setter
    def datetime(self, value):
        pass

Renders as:

.. py:attribute:: datetime
    :noindex:

    The current date and time as a `time.struct_time`.

Read-only example::

    @property
    def temperature(self):
        """
        The current temperature in degrees Celsius. (read-only)

        The device may require calibration to get accurate readings.
        """
        return self._read(TEMPERATURE)


Renders as:

.. py:attribute:: temperature
    :noindex:

    The current temperature in degrees Celsius. (read-only)

    The device may require calibration to get accurate readings.

Data descriptor description
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Comment is after the definition::

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

Documentation References to other Libraries
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
When you need to make references to documentation in other libraries you should refer the class using single
backticks  ``:class:`~adafruit_motor.servo.Servo```. You must also add the reference in the ``conf.py`` file in the
``intersphinx_mapping section`` by adding a new entry::

    "adafruit_motor": ("https://circuitpython.readthedocs.io/projects/motor/en/latest/", None,),

Use BusDevice
--------------------------------------------------------------------------------

`BusDevice <https://github.com/adafruit/Adafruit_CircuitPython_BusDevice>`_ is an
awesome foundational library that manages talking on a shared I2C or SPI device
for you. The devices manage locking which ensures that a transfer is done as a
single unit despite CircuitPython internals and, in the future, other Python
threads. For I2C, the device also manages the device address. The SPI device,
manages baudrate settings, chip select line and extra post-transaction clock
cycles.

I2C Example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

  from adafruit_bus_device import i2c_device

  DEVICE_DEFAULT_I2C_ADDR = 0x42

  class Widget:
      """A generic widget."""

      def __init__(self, i2c, address=DEVICE_DEFAULT_I2C_ADDR):
          self.i2c_device = i2c_device.I2CDevice(i2c, address)
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
              spi.readinto(self.buf)
          return self.buf[0]



Class documentation example template
--------------------------------------------------------------------------------
When documenting classes, you should use the following template to illustrate basic usage.
It is similar with the simpletest example, however this will display the information in the Read The Docs
documentation.
The advantage of using this template is it makes the documentation consistent across the libraries.

This is an example for a AHT20 temperature sensor. Include the following after the class parameter:


.. code-block:: python

    """

    **Quickstart: Importing and using the AHT10/AHT20 temperature sensor**

        Here is an example of using the :class:`AHTx0` class.
        First you will need to import the libraries to use the sensor

        .. code-block:: python

            import board
            import adafruit_ahtx0

        Once this is done you can define your `board.I2C` object and define your sensor object

        .. code-block:: python

            i2c = board.I2C()  # uses board.SCL and board.SDA
            aht = adafruit_ahtx0.AHTx0(i2c)

        Now you have access to the temperature and humidity using
        the :attr:`temperature` and :attr:`relative_humidity` attributes

        .. code-block:: python

            temperature = aht.temperature
            relative_humidity = aht.relative_humidity

    """


Use composition
--------------------------------------------------------------------------------

When writing a driver, take in objects that provide the functionality you need
rather than taking their arguments and constructing them yourself or subclassing
a parent class with functionality. This technique is known as composition and
leads to code that is more flexible and testable than traditional inheritance.

.. seealso:: `Wikipedia <https://en.wikipedia.org/wiki/Dependency_inversion_principle>`_
  has more information on "dependency inversion".

For example, if you are writing a driver for an I2C device, then take in an I2C
object instead of the pins themselves. This allows the calling code to provide
any object with the appropriate methods such as an I2C expansion board.

Another example is to expect a :py:class:`~digitalio.DigitalInOut` for a pin to
toggle instead of a :py:class:`~microcontroller.Pin` from :py:mod:`board`.
Taking in the :py:class:`~microcontroller.Pin` object alone would limit the
driver to pins on the actual microcontroller instead of pins provided by another
driver such as an IO expander.

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

Although Python doesn't require managing memory, it's still a good practice for
library writers to think about memory allocations. Avoid them in drivers if
you can because you never know how much something will be called. Fewer
allocations means less time spent cleaning up. So, where you can, prefer
bytearray buffers that are created in ``__init__`` and used throughout the
object with methods that read or write into the buffer instead of creating new
objects. Unified hardware API classes such as `busio.SPI` are design to read and
write to subsections of buffers.

It's ok to allocate an object to return to the user. Just beware of causing more
than one allocation per call due to internal logic.

**However**, this is a memory tradeoff so do not do it for large or rarely used
buffers.

Examples
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

struct.pack
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Use `struct.pack_into` instead of `struct.pack`.

Use of MicroPython ``const()``
--------------------------------------------------------------------------------
The MicroPython ``const()`` feature, as discussed in `this forum post
<https://forum.micropython.org/viewtopic.php?t=450>`_, and in `this issue thread
<https://github.com/micropython/micropython/issues/573>`_, provides some
optimizations that can be useful on smaller, memory constrained devices. However,
when using ``const()``, keep in mind these general guide lines:

- Always use via an import, ex: ``from micropython import const``
- Limit use to global (module level) variables only.
- If user will not need access to variable, prefix name with a leading
  underscore, ex: ``_SOME_CONST``.

Libraries Examples
------------------
When adding examples, cookiecutter will add a ``<name>_simpletest.py`` file in the examples directory for you.
Be sure to include code with the library minimal functionalities to work on a device.
You could other examples if needed featuring different
functionalities of the library.
If you add additional examples, be sure to include them in the ``examples.rst``. Naming of the examples
files should use the name of the library followed by a description, using underscore to separate them.
When using print statements you should use the ``" ".format()`` format, as there are particular boards
that are not capable to use f-strings.

.. code-block:: python

  text_to_display = "World!"

  print("Hello {}".format(text_to_display))

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
| ``magnetic``          | (float, float, float) | x, y, z micro-Tesla (uT)                                                |
+-----------------------+-----------------------+-------------------------------------------------------------------------+
| ``orientation``       | (float, float, float) | x, y, z degrees                                                         |
+-----------------------+-----------------------+-------------------------------------------------------------------------+
| ``gyro``              | (float, float, float) | x, y, z radians per second                                              |
+-----------------------+-----------------------+-------------------------------------------------------------------------+
| ``temperature``       | float                 | degrees Celsius                                                         |
+-----------------------+-----------------------+-------------------------------------------------------------------------+
| ``CO2``               | float                 | measured CO2 in ppm                                                     |
+-----------------------+-----------------------+-------------------------------------------------------------------------+
| ``eCO2``              | float                 | equivalent/estimated CO2 in ppm (estimated from some other measurement) |
+-----------------------+-----------------------+-------------------------------------------------------------------------+
| ``TVOC``              | float                 | Total Volatile Organic Compounds in ppb                                 |
+-----------------------+-----------------------+-------------------------------------------------------------------------+
| ``distance``          | float                 | centimeters (cm)                                                        |
+-----------------------+-----------------------+-------------------------------------------------------------------------+
| ``proximity``         | int                   | non-unit-specific proximity values (monotonic but not actual distance)  |
+-----------------------+-----------------------+-------------------------------------------------------------------------+
| ``light``             | float                 | non-unit-specific light levels (should be monotonic but is not lux)     |
+-----------------------+-----------------------+-------------------------------------------------------------------------+
| ``lux``               | float                 | SI lux                                                                  |
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
| ``duty_cycle``        | int                   | 16-bit PWM duty cycle (regardless of output resolution)                 |
+-----------------------+-----------------------+-------------------------------------------------------------------------+
| ``frequency``         | int                   | Hertz (Hz)                                                              |
+-----------------------+-----------------------+-------------------------------------------------------------------------+
| ``value``             | bool                  | Digital logic                                                           |
+-----------------------+-----------------------+-------------------------------------------------------------------------+
| ``value``             | int                   | 16-bit Analog value, unit-less                                          |
+-----------------------+-----------------------+-------------------------------------------------------------------------+
| ``weight``            | float                 | grams (g)                                                               |
+-----------------------+-----------------------+-------------------------------------------------------------------------+
| ``sound_level``       | float                 | non-unit-specific sound level (monotonic but not actual decibels)       |
+-----------------------+-----------------------+-------------------------------------------------------------------------+

Adding native modules
--------------------------------------------------------------------------------

The Python API for a new module should be defined and documented in
``shared-bindings`` and define an underlying C API. If the implementation is
port-agnostic or relies on underlying APIs of another module, the code should
live in ``shared-module``. If it is port specific then it should live in ``common-hal``
within the port's folder. In either case, the file and folder structure should
mimic the structure in ``shared-bindings``.

To test your native modules or core enhancements, follow these Adafruit Learning Guides
for building local firmware to flash onto your device(s):

`Build CircuitPython <https://learn.adafruit.com/building-circuitpython>`_


MicroPython compatibility
--------------------------------------------------------------------------------

Keeping compatibility with MicroPython isn't a high priority. It should be done
when it's not in conflict with any of the above goals.
