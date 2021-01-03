.. _micropython_lib:

MicroPython libraries
=====================

Python standard libraries and micro-libraries
---------------------------------------------

These libraries are inherited from MicroPython.
They are similar to the standard Python libraries with the same name
or with the "u" prefix dropped.
They implement a subset of or a variant of the corresponding
standard Python library.

.. warning::

   Though these MicroPython-based libraries are available in CircuitPython,
   their functionality may change in the future, perhaps significantly.
   As CircuitPython continues to develop, new versions of these libraries will
   be created that are more compliant with the standard Python libraries.
   You may need to change your code later if you rely
   on any non-standard functionality they currently provide.

CircuitPython's long-term goal is that code written in CircuitPython
using Python standard libraries will be runnable on CPython without changes.

Some libraries below are not enabled on CircuitPython builds with
limited flash memory, usually on non-Express builds:
``uerrno``, ``ure``.

Some libraries are not currently enabled in any CircuitPython build, but may be in the future:
``uio``, ``ujson``, ``uzlib``.

Some libraries are only enabled only WiFi-capable ports (ESP8266, nRF)
because they are typically used for network software:
``binascii``, ``hashlib``, ``uheapq``, ``uselect``, ``ussl``.
Not all of these are enabled on all WiFi-capable ports.

.. toctree::
   :maxdepth: 1

   builtins.rst
   uheapq.rst
   array.rst
   binascii.rst
   collections.rst
   gc.rst
   hashlib.rst
   sys.rst
   uerrno.rst
   uio.rst
   ujson.rst
   ure.rst
   uselect.rst
   usocket.rst
   ussl.rst
   uzlib.rst

Omitted functions in the ``string`` library
-------------------------------------------

A few string operations are not enabled on CircuitPython
M0 non-Express builds, due to limited flash memory:
``string.center()``, ``string.partition()``, ``string.splitlines()``,
``string.reversed()``.


CircuitPython/MicroPython-specific libraries
--------------------------------------------

Functionality specific to the CircuitPython/MicroPython implementation is available in
the following libraries. These libraries may change significantly or be removed in future
versions of CircuitPython.

.. toctree::
   :maxdepth: 1

   btree.rst
   framebuf.rst
   micropython.rst
   network.rst
   uctypes.rst

Libraries specific to the ESP8266
---------------------------------

The following libraries are specific to the ESP8266.

.. toctree::
  :maxdepth: 2

  esp.rst
