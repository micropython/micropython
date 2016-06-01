MicroPython libraries
=====================

This chapter described modules (function and class libraries) which are built
into MicroPython. There are few categories of modules:

* Modules which implement subset of standard Python functionality and are not
  intended to be extended by a user.
* Modules which are implement subset of Python functionality, with a provision
  for extension by a user (via Python code).
* Modules which implement MicroPython extensions to Python standard libraries.
* Modules specific to a particular port and thus not portable.

Note about the availability of modules and their contents: This documentation
in general aspires to describe all modules and functions/classes which are
implemented in MicroPython. However, MicroPython is highly configurable, and
each port to a particular board/embedded system makes available only a subset
of MicroPython libraries. For officially supported ports, there is an effort
to either filter out non-applicable items, or mark individual descriptions
with "Availability:" clauses describing which ports provide a given feature.
With that in mind, please still be ready to a fact that some function/class
in a module, or even entire module, described in this documentation, may be
not available in a particular build of MicroPython on a particular board. The
best place to find a general information of the availability/non-availability
of a particular feature is "General Information" secton which contains
information pertaining to a specific port.

Beyond the built-in libraries described in this documentation, many more
modules from the Python standard library, as well as further MicroPython
extensions to it, can be found in the `micropython-lib repository
<https://github.com/micropython/micropython-lib>`_.

Python standard libraries and micro-libraries
---------------------------------------------

The following standard Python libraries have been "micro-ified" to fit in with
the philosophy of MicroPython.  They provide the core functionality of that
module and are intended to be a drop-in replacement for the standard Python
library.

.. only:: not port_unix

    The modules are available by their u-name, and also by their non-u-name.  The
    non-u-name can be overridden by a file of that name in your package path.
    For example, ``import json`` will first search for a file ``json.py`` or
    directory ``json`` and load that package if it is found.  If nothing is found,
    it will fallback to loading the built-in ``ujson`` module.

.. only:: port_unix

    .. toctree::
       :maxdepth: 1

       cmath.rst
       gc.rst
       math.rst
       select.rst
       sys.rst
       ubinascii.rst
       ucollections.rst
       uhashlib.rst
       uheapq.rst
       uio.rst
       ujson.rst
       uos.rst
       ure.rst
       usocket.rst
       ustruct.rst
       utime.rst
       uzlib.rst

.. only:: port_pyboard

    .. toctree::
       :maxdepth: 1

       cmath.rst
       gc.rst
       math.rst
       select.rst
       sys.rst
       ubinascii.rst
       ucollections.rst
       uhashlib.rst
       uheapq.rst
       uio.rst
       ujson.rst
       uos.rst
       ure.rst
       usocket.rst
       ustruct.rst
       utime.rst
       uzlib.rst

.. only:: port_wipy

    .. toctree::
       :maxdepth: 1

       gc.rst
       select.rst
       sys.rst
       ubinascii.rst
       ujson.rst
       uos.rst
       ure.rst
       usocket.rst
       ussl.rst
       utime.rst

.. only:: port_esp8266

    .. toctree::
       :maxdepth: 1

       gc.rst
       math.rst
       sys.rst
       ubinascii.rst
       ucollections.rst
       uhashlib.rst
       uheapq.rst
       uio.rst
       ujson.rst
       uos.rst
       ure.rst
       usocket.rst
       ussl.rst
       ustruct.rst
       utime.rst
       uzlib.rst


MicroPython-specific libraries
------------------------------

Functionality specific to the MicroPython implementation is available in
the following libraries.

.. toctree::
   :maxdepth: 1

   machine.rst
   micropython.rst
   network.rst
   uctypes.rst


.. only:: port_pyboard

   Libraries specific to the pyboard
   ---------------------------------

   The following libraries are specific to the pyboard.

   .. toctree::
      :maxdepth: 2

      pyb.rst

.. only:: port_wipy

   Libraries specific to the WiPy
   ---------------------------------

   The following libraries are specific to the WiPy.

   .. toctree::
      :maxdepth: 2

      wipy.rst


.. only:: port_esp8266

   Libraries specific to the ESP8266
   ---------------------------------

   The following libraries are specific to the ESP8266.

   .. toctree::
      :maxdepth: 2

      esp.rst
