.. _micropython_lib:

MicroPython libraries
=====================

Python standard libraries and micro-libraries
---------------------------------------------

The libraries below are inherited from MicroPython.
They are similar to the standard Python libraries with the same name.
They implement a subset of or a variant of the corresponding
standard Python library.

CircuitPython's long-term goal is that code written in CircuitPython
using Python standard libraries will be runnable on CPython without changes.

These libraries are not enabled on CircuitPython builds with
limited flash memory, usually on non-Express builds:
``binascii``, ``errno``, ``json``, ``re``.

These libraries are not currently enabled in any CircuitPython build, but may be in the future,
with the ``u`` prefix dropped:
``uctypes``, ``uhashlib``, ``uzlib``.

.. toctree::
   :maxdepth: 1

   builtins.rst
   uheapq.rst
   array.rst
   binascii.rst
   collections.rst
   errno.rst
   gc.rst
   hashlib.rst
   io.rst
   json.rst
   re.rst
   sys.rst
   uasyncio.rst
   uctypes.rst
   uselect.rst
   uzlib.rst

Omitted functions in the ``string`` library
-------------------------------------------

A few string operations are not enabled on small builds
(usually non-Express), due to limited flash memory:
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
