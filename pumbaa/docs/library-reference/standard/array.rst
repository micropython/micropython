:mod:`array` ---  Array
=======================

.. module:: array
   :synopsis: Array.

This module defines an object type which can compactly represent an
array of basic values: characters, integers, floating point
numbers. Arrays are sequence types and behave very much like lists,
except that the type of objects stored in them is constrained. The
type is specified at object creation time by using a type code, which
is a single character. The following type codes are defined:

+-----------+--------------------+-------------+-----------------------+
| Type code | C Type             | Python Type | Minimum size in bytes |
+===========+====================+=============+=======================+
| ``'b'``   | signed char        | int         | 1                     |
+-----------+--------------------+-------------+-----------------------+
| ``'B'``   | unsigned char      | int         | 1                     |
+-----------+--------------------+-------------+-----------------------+
| ``'h'``   | signed short       | int         | 2                     |
+-----------+--------------------+-------------+-----------------------+
| ``'H'``   | unsigned short     | int         | 2                     |
+-----------+--------------------+-------------+-----------------------+
| ``'i'``   | signed int         | int         | 2                     |
+-----------+--------------------+-------------+-----------------------+
| ``'I'``   | unsigned int       | int         | 2                     |
+-----------+--------------------+-------------+-----------------------+
| ``'l'``   | signed long        | int         | 4                     |
+-----------+--------------------+-------------+-----------------------+
| ``'L'``   | unsigned long      | int         | 4                     |
+-----------+--------------------+-------------+-----------------------+
| ``'q'``   | signed long long   | int         | 8                     |
+-----------+--------------------+-------------+-----------------------+
| ``'Q'``   | unsigned long long | int         | 8                     |
+-----------+--------------------+-------------+-----------------------+
| ``'f'``   | float              | float       | 4                     |
+-----------+--------------------+-------------+-----------------------+
| ``'d'``   | double             | float       | 8                     |
+-----------+--------------------+-------------+-----------------------+

----------------------------------------------


.. class:: array.array(typecode[, initializer])

   A new array whose items are restricted by `typecode`, and
   initialized from the optional `initializer` value, which must be a
   list, a bytes-like object, or iterable over elements of the
   appropriate type.

   If given a list or string, the `initializer` is passed to the new
   array’s `fromlist()`, `frombytes()`, or `fromunicode()` method (see
   below) to add initial items to the array. Otherwise, the iterable
   initializer is passed to the `extend()` method.


   .. method:: array.append(x)

      Append a new item with value `x` to the end of the array.


   .. method:: array.extend(iterable)

      Append items from `iterable` to the end of the array. If
      `iterable` is another array, it must have exactly the same type
      code; if not, `TypeError` will be raised. If `iterable` is not
      an array, it must be iterable and its elements must be the right
      type to be appended to the array.
