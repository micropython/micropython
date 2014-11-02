:mod:`struct` -- pack and unpack primitive data types
=====================================================

.. module:: struct
   :synopsis: pack and unpack primitive data types

See `Python struct <https://docs.python.org/3/library/struct.html>`_ for more
information.

Functions
---------

.. function:: calcsize(fmt)

   Return the number of bytes needed to store the given ``fmt``.

.. function:: pack(fmt, v1, v2, ...)

   Pack the values ``v1``, ``v2``, ... according to the format string ``fmt``.
   The return value is a bytes object encoding the values.

.. function:: unpack(fmt, data)

   Unpack from the ``data`` according to the format string ``fmt``.
   The return value is a tuple of the unpacked values.
