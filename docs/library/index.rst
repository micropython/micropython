.. _micropython_lib:

CircuitPython libraries
=====================

Python standard libraries and micro-libraries
---------------------------------------------

These libraries are the same or are subsets or slight variants of the standard Python libraries.

MicroPython prefixed many of these libraries with ``u``. In CircuitPython, those
that are subsets or the same as the standard Python libraries have been or will be renamed
to their original names.
Our aspiration is that code written in CircuitPython
that uses Python standard libraries will be runnable on CPython without changes.
But we may fall short of this goal in some cases.

Some of the libraries below are not enabled on CircuitPython builds with
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
   struct.rst
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

A few string operations are not enabled on CircuitPython builds with
limited flash memory, usually on non-Express builds:
``string.center()``, ``string.partition()``, ``string.splitlines()``,
``string.reversed()``.


CircuitPython/MicroPython-specific libraries
------------------------------

Functionality specific to the CircuitPython (MicroPython) implementation is available in
the following libraries.

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
