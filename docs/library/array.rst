:mod:`array` -- arrays of numeric data
======================================

.. module:: array
   :synopsis: efficient arrays of numeric data

See `Python array <https://docs.python.org/3/library/array.html>`_ for more
information.

Supported format codes: ``b``, ``B``, ``h``, ``H``, ``i``, ``I``, ``l``,
``L``, ``q``, ``Q``, ``f``, ``d`` (the latter 2 depending on the
floating-point support).

Classes
-------

.. class:: array.array(typecode, [iterable])

    Create array with elements of given type. Initial contents of the
    array are given by an `iterable`. If it is not provided, an empty
    array is created.

    .. method:: append(val)

        Append new element to the end of array, growing it.

    .. method:: extend(iterable)

        Append new elements as contained in an iterable to the end of
        array, growing it.
