.. _micropython_lib:

MicroPython libraries
=====================

.. warning::

   Important summary of this section

   * MicroPython provides built-in modules that mirror the functionality of the
     Python standard library (e.g. :mod:`os`, :mod:`time`), as well as
     MicroPython-specific modules (e.g. :mod:`bluetooth`, :mod:`machine`).
   * Most standard library modules implement a subset of the functionality of
     the equivalent Python module, and in a few cases provide some
     MicroPython-specific extensions (e.g. :mod:`array`, :mod:`os`)
   * Due to resource constraints or other limitations, some ports or firmware
     versions may not include all the functionality documented here.
   * To allow for extensibility, the built-in modules can be extended from
     Python code loaded onto the device.

This chapter describes modules (function and class libraries) which are built
into MicroPython. This documentation in general aspires to describe all modules
and functions/classes which are implemented in the MicroPython project.
However, MicroPython is highly configurable, and each port to a particular
board/embedded system may include only a subset of the available MicroPython
libraries.

With that in mind, please be warned that some functions/classes in a module (or
even the entire module) described in this documentation **may be unavailable**
in a particular build of MicroPython on a particular system. The best place to
find general information of the availability/non-availability of a particular
feature is the "General Information" section which contains information
pertaining to a specific :term:`MicroPython port`.

On some ports you are able to discover the available, built-in libraries that
can be imported by entering the following at the :term:`REPL`::

    help('modules')

Beyond the built-in libraries described in this documentation, many more
modules from the Python standard library, as well as further MicroPython
extensions to it, can be found in :term:`micropython-lib`.

Python standard libraries and micro-libraries
---------------------------------------------

The following standard Python libraries have been "micro-ified" to fit in with
the philosophy of MicroPython.  They provide the core functionality of that
module and are intended to be a drop-in replacement for the standard Python
library.

.. toctree::
   :maxdepth: 1

   array.rst
   binascii.rst
   builtins.rst
   cmath.rst
   collections.rst
   errno.rst
   gc.rst
   hashlib.rst
   heapq.rst
   io.rst
   json.rst
   math.rst
   os.rst
   re.rst
   select.rst
   socket.rst
   ssl.rst
   struct.rst
   sys.rst
   time.rst
   uasyncio.rst
   zlib.rst
   _thread.rst


MicroPython-specific libraries
------------------------------

Functionality specific to the MicroPython implementation is available in
the following libraries.

.. toctree::
   :maxdepth: 1

   bluetooth.rst
   btree.rst
   cryptolib.rst
   framebuf.rst
   machine.rst
   micropython.rst
   neopixel.rst
   network.rst
   uctypes.rst


Port-specific libraries
-----------------------

In some cases the following port/board-specific libraries have functions or
classes similar to those in the :mod:`machine` library.  Where this occurs, the
entry in the port specific library exposes hardware functionality unique to
that platform.

To write portable code use functions and classes from the :mod:`machine` module.
To access platform-specific hardware use the appropriate library, e.g.
:mod:`pyb` in the case of the Pyboard.


Libraries specific to the pyboard
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The following libraries are specific to the pyboard.

.. toctree::
  :maxdepth: 2

  pyb.rst
  lcd160cr.rst


Libraries specific to the WiPy
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The following libraries and classes are specific to the WiPy.

.. toctree::
  :maxdepth: 2

  wipy.rst
  machine.ADCWiPy.rst
  machine.TimerWiPy.rst


Libraries specific to the ESP8266 and ESP32
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The following libraries are specific to the ESP8266 and ESP32.

.. toctree::
  :maxdepth: 2

  esp.rst
  esp32.rst


Libraries specific to the RP2040
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The following libraries are specific to the RP2040, as used in the Raspberry Pi Pico.

.. toctree::
  :maxdepth: 2

  rp2.rst

Libraries specific to Zephyr
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The following libraries are specific to the Zephyr port.

.. toctree::
  :maxdepth: 2

  zephyr.rst

Extending built-in libraries from Python
----------------------------------------

In most cases, the above modules are actually named ``umodule`` rather than
``module``, but MicroPython will alias any module prefixed with a ``u`` to the
non-``u`` version. However a file (or :term:`frozen module`) named
``module.py`` will take precedence over this alias.

This allows the user to provide an extended implementation of a built-in library
(perhaps to provide additional CPython compatibility). The user-provided module
(in ``module.py``) can still use the built-in functionality by importing
``umodule`` directly. This is used extensively in :term:`micropython-lib`. See
:ref:`packages` for more information.

This applies to both the Python standard libraries (e.g. ``os``, ``time``, etc),
but also the MicroPython libraries too (e.g. ``machine``, ``bluetooth``, etc).
The main exception is the port-specific libraries (``pyb``, ``esp``, etc).

*Other than when you specifically want to force the use of the built-in module,
we recommend always using ``import module`` rather than ``import umodule``.*
