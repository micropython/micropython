We love CircuitPython and would love to see it come to more microcontroller
platforms. Since 3.0 we've reworked CircuitPython to make it easier than ever to
add support. While there are some major differences between ports, this page
covers the similarities that make CircuitPython what it is and how that core
fits into a variety of microcontrollers.

Architecture
============

There are three core pieces to CircuitPython:

The first is the Python VM that the awesome MicroPython devs have created.
These VMs are written to be portable so there is not much needed when moving to
a different microcontroller, especially if it is ARM based.

The second is the infrastructure around those VMs which provides super basic
operating system functionality such as initializing hardware, running USB,
prepping file systems and automatically running user code on boot. In
CircuitPython we've dubbed this component the supervisor because it monitors
and facilitates the VMs which run user Python code. Porting involves the
supervisor because many of the tasks it does while interfacing with the
hardware. Once complete, the REPL works and debugging can migrate to a
Python based approach rather than C.

The third core piece is the plethora of low level APIs that CircuitPython
provides as the foundation for higher level libraries including device drivers.
These APIs are called from within the running VMs through the Python interfaces
defined in ``shared-bindings``. These bindings rely on the underlying
``common_hal`` C API to implement the functionality needed for the Python API.
By splitting the two, we work to ensure standard functionality across which
means that libraries and examples apply across ports with minimal changes.

Porting
=======

Step 1: Getting building
------------------------
The first step to porting to a new microcontroller is getting a build running.
The primary goal of it should be to get ``main.c`` compiling with the assistance
of the ``supervisor/supervisor.mk`` file. Port specific code should be isolated
to the port's directory (in the top level until the ``ports`` directory is
present). This includes the Makefile and any C library resources. Make sure
these resources are compatible with the MIT License of the rest of the code!

Circuitpython has a number of modules enabled by default in
``py/circuitpy_mpconfig.mk``. Most of these modules will need to be disabled in
``mpconfigboard.mk`` during the early stages of a port in order for it to
compile. As the port progresses in module support, this list can be pruned down
as a natural "TODO" list. An example minimal build list is shown below:

.. code-block:: makefile

    # These modules are implemented in ports/<port>/common-hal:

    # Typically the first module to create
    CIRCUITPY_MICROCONTROLLER = 0
    # Typically the second module to create
    CIRCUITPY_DIGITALIO = 0
    # Other modules:
    CIRCUITPY_ANALOGIO = 0
    CIRCUITPY_BUSIO = 0
    CIRCUITPY_COUNTIO = 0
    CIRCUITPY_NEOPIXEL_WRITE = 0
    CIRCUITPY_PULSEIO = 0
    CIRCUITPY_OS = 0
    CIRCUITPY_NVM = 0
    CIRCUITPY_AUDIOBUSIO = 0
    CIRCUITPY_AUDIOIO = 0
    CIRCUITPY_ROTARYIO = 0
    CIRCUITPY_RTC = 0
    CIRCUITPY_SDCARDIO = 0
    CIRCUITPY_FRAMEBUFFERIO = 0
    CIRCUITPY_FREQUENCYIO = 0
    CIRCUITPY_I2CPERIPHERAL = 0
    # Requires SPI, PulseIO (stub ok):
    CIRCUITPY_DISPLAYIO = 0

    # These modules are implemented in shared-module/ - they can be included in
    # any port once their prerequisites in common-hal are complete.
    # Requires DigitalIO:
    CIRCUITPY_BITBANGIO = 0
    # Requires DigitalIO
    CIRCUITPY_GAMEPAD = 0
    # Requires neopixel_write or SPI (dotstar)
    CIRCUITPY_PIXELBUF = 0
    # Requires OS
    CIRCUITPY_RANDOM = 0
    # Requires OS, filesystem
    CIRCUITPY_STORAGE = 0
    # Requires Microcontroller
    CIRCUITPY_TOUCHIO = 0
    # Requires USB
    CIRCUITPY_USB_HID = 0
    CIRCUITPY_USB_MIDI = 0
    # Does nothing without I2C
    CIRCUITPY_REQUIRE_I2C_PULLUPS = 0
    # No requirements, but takes extra flash
    CIRCUITPY_ULAB = 0

Step 2: Init
--------------
Once your build is setup, the next step should be to get your clocks going as
you expect from the supervisor. The supervisor calls ``port_init`` to allow for
initialization at the beginning of main. This function also has the ability to
request a safe mode state which prevents the supervisor from running user code
while still allowing access to the REPL and other resources.

The core port initialization and reset methods are defined in
``supervisor/port.c`` and should be the first to be implemented. It's required
that they be implemented in the ``supervisor`` directory within the port
directory. That way, they are always in the expected place.

The supervisor also uses three linker variables, ``_ezero``, ``_estack`` and
``_ebss`` to determine memory layout for stack overflow checking.

Step 3: REPL
------------
Getting the REPL going is a huge step. It involves a bunch of initialization to
be done correctly and is a good sign you are well on your porting way. To get
the REPL going you must implement the functions and definitions from
``supervisor/serial.h`` with a corresponding ``supervisor/serial.c`` in the port
directory. This involves sending and receiving characters over some sort of
serial connection. It could be UART or USB for example.
