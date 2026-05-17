:mod:`struct` -- pack and unpack primitive data types
=====================================================

.. module:: struct
   :synopsis: pack and unpack primitive data types

|see_cpython_module| :mod:`python:struct`.

The following byte orders are supported:

+-----------+------------------------+----------+-----------+
| Character | Byte order             | Size     | Alignment |
+===========+========================+==========+===========+
| @         | native                 | native   | native    |
+-----------+------------------------+----------+-----------+
| <         | little-endian          | standard | none      |
+-----------+------------------------+----------+-----------+
| >         | big-endian             | standard | none      |
+-----------+------------------------+----------+-----------+
| !         | network (= big-endian) | standard | none      |
+-----------+------------------------+----------+-----------+

The following data types are supported:

+--------+--------------------+-------------------+---------------+
| Format | C Type             | Python type       | Standard size |
+========+====================+===================+===============+
| b      | signed char        | integer           | 1             |
+--------+--------------------+-------------------+---------------+
| B      | unsigned char      | integer           | 1             |
+--------+--------------------+-------------------+---------------+
| h      | short              | integer           | 2             |
+--------+--------------------+-------------------+---------------+
| H      | unsigned short     | integer           | 2             |
+--------+--------------------+-------------------+---------------+
| i      | int                | integer (`1<fn>`) | 4             |
+--------+--------------------+-------------------+---------------+
| I      | unsigned int       | integer (`1<fn>`) | 4             |
+--------+--------------------+-------------------+---------------+
| l      | long               | integer (`1<fn>`) | 4             |
+--------+--------------------+-------------------+---------------+
| L      | unsigned long      | integer (`1<fn>`) | 4             |
+--------+--------------------+-------------------+---------------+
| q      | long long          | integer (`1<fn>`) | 8             |
+--------+--------------------+-------------------+---------------+
| Q      | unsigned long long | integer (`1<fn>`) | 8             |
+--------+--------------------+-------------------+---------------+
| e      | n/a (half-float)   | float (`2<fn>`)   | 2             |
+--------+--------------------+-------------------+---------------+
| f      | float              | float (`2<fn>`)   | 4             |
+--------+--------------------+-------------------+---------------+
| d      | double             | float (`2<fn>`)   | 8             |
+--------+--------------------+-------------------+---------------+
| s      | char[]             | bytes             |               |
+--------+--------------------+-------------------+---------------+
| P      | void *             | integer           |               |
+--------+--------------------+-------------------+---------------+

.. _fn:

(1) Requires long support when used with values larger than 30 bits.
(2) Requires floating point support.

.. admonition:: Difference to CPython
   :class: attention

   Whitespace is not supported in format strings.

Functions
---------

.. function:: calcsize(fmt)

   Return the number of bytes needed to store the given *fmt*.

.. function:: pack(fmt, v1, v2, ...)

   Pack the values *v1*, *v2*, ... according to the format string *fmt*.
   The return value is a bytes object encoding the values.

.. function:: pack_into(fmt, buffer, offset, v1, v2, ...)

   Pack the values *v1*, *v2*, ... according to the format string *fmt*
   into a *buffer* starting at *offset*. *offset* may be negative to count
   from the end of *buffer*.

.. function:: unpack(fmt, data)

   Unpack from the *data* according to the format string *fmt*.
   The return value is a tuple of the unpacked values.

.. function:: unpack_from(fmt, data, offset=0, /)

   Unpack from the *data* starting at *offset* according to the format string
   *fmt*. *offset* may be negative to count from the end of *data*. The return
   value is a tuple of the unpacked values.
