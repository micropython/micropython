.. _micropython_lib:

Standard Libraries
==================

Python standard libraries
-------------------------

The libraries below implement a subset of the corresponding
standard Python (CPython) library. They are implemented in C, not Python.

CircuitPython's long-term goal is that code written in CircuitPython
using Python standard libraries will be runnable on CPython without changes.

These libraries are not enabled on CircuitPython builds with
limited flash memory:
``binascii``, ``errno``, ``json``, ``re``.

These libraries are not currently enabled in any CircuitPython build, but may be in the future:
``ctypes``

.. toctree::
   :maxdepth: 1

   builtins.rst
   heapq.rst
   array.rst
   binascii.rst
   collections.rst
   errno.rst
   gc.rst
   io.rst
   json.rst
   re.rst
   sys.rst
   ctypes.rst
   select.rst

Omitted ``string`` functions
----------------------------

A few string operations are not enabled on small builds
due to limited flash memory:
``string.center()``, ``string.partition()``, ``string.splitlines()``,
``string.reversed()``.


CircuitPython/MicroPython-specific libraries
--------------------------------------------

Functionality specific to the CircuitPython/MicroPython implementations is available in
the following libraries.

.. toctree::
   :maxdepth: 1

   micropython.rst
