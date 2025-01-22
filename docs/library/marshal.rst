:mod:`marshal` -- Python object serialization
=============================================

.. module:: marshal
   :synopsis: Convert Python objects to and from a binary format

|see_cpython_module| :mod:`python:marshal`.

This module implements conversion between Python objects and a binary format.
The format is specific to MicroPython but does not depend on the machine
architecture, so the data can be transferred and used on a different MicroPython
instance, as long as the version of the binary data matches (it's currently
versioned as the mpy file version, see :ref:`mpy_files`).

Functions
---------

.. function:: dumps(value, /)

   Convert the given *value* to binary format and return a corresponding ``bytes``
   object.

   Currently, code objects are the only supported values that can be converted.

.. function:: loads(data, /)

   Convert the given bytes-like *data* to its corresponding Python object, and
   return it.
