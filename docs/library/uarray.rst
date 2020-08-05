:mod:`uarray` -- arrays of numeric data
=======================================

.. module:: uarray
   :synopsis: efficient arrays of numeric data

|see_cpython_module| :mod:`python:array`.

This module, ``uarray``, is a subset of ``python:array`` and only the methods documented below are available, 
with the exception of ``decode``.
``decode`` is unique to MicroPython and behaves like ``array.tobytes().decode(...)`` does in Python
(but  is more efficient because it does not creat an intermediates ``bytes`` object).

Supported type codes: ``b``, ``B``, ``h``, ``H``, ``i``, ``I``, ``l``,
``L``, ``q``, ``Q``, ``f``, ``d`` (the latter 2 depending on the
floating-point support).

Classes
-------

.. class:: array(typecode, [iterable])

    Create array with elements of given type. Initial contents of the
    array are given by *iterable*. If it is not provided, an empty
    array is created.

    .. method:: append(val)

        Append new element *val* to the end of array, growing it.

    .. method:: extend(iterable)

        Append new elements as contained in *iterable* to the end of
        array, growing it.

    .. method:: decode(encoding="utf-8", errors="strict")

        Return a string decoded from *self*. 
        Default *encoding* is 'utf-8'. 
        *errors* may be given to set a different error handling scheme. 
        The default for errors is 'strict', meaning that encoding errors raise a ``UnicodeError``. 
        Other possible values are 'ignore' and 'replace'.

    .. method:: __getitem__(index)

        Indexed read of *self*, called as ``a[index]`` (where ``a`` is an ``array``).
        Returns a value if *index* is an ``int`` and an ``array`` if *index* is a slice. 
        Negative indices count from end and ``IndexError``is thrown if the index out of range.
	     **Note**: This method, ``__getitem__``, cannot be called directly (``a.__getitem__(index)`` fails) and the 
	     method is not present in ``__dict__``, however ``a[index]`` does work.

    .. method:: __setitem__(index, value)

        Indexed write into *self*, called as ``a[index] = value`` (where ``a`` is an ``array``).
        ``value`` is a single value if *index* is an ``int`` and an ``array`` if *index* is a slice. 
        Negative indices count from end and ``IndexError``is thrown if the index out of range.
	     **Note**: This method, ``__setitem__``, cannot be called directly (``a.__setitem__(index, value)`` fails) and the 
	     method is not present in ``__dict__``, however ``a[index] = value`` does work.

    .. method:: __len__()

        Returns the number of items in *self*, called as ``len(a)`` (where ``a`` is an ``array``).
	     **Note**: This method, ``__len__``, cannot be called directly (``a.__len__()`` fails) and the 
	     method is not present in ``__dict__``, however ``len(a)`` does work.

    .. method:: __add__(other)

        Return a new ``array`` that is the concatonation of *self* with *other*, called as ``a + other`` (where ``a`` and *other* are both ``array``s).
	     **Note**: This method, ``__add__``, cannot be called directly (``a.__add__(other)`` fails) and the 
	     method is not present in ``__dict__``, however ``a + other`` does work.

    .. method:: __iadd__(other)

        Concatonates *self* with *other* in-place, called as ``a += other`` (where ``a`` and *other* are both ``array``s).
        Equivalent to ``extend(other)``.
	     **Note**: This method, ``__iadd__``, cannot be called directly (``a.__iadd__(other)`` fails) and the 
	     method is not present in ``__dict__``, however ``a += other`` does work.

    .. method:: __repr__()

        Returns the string representation of *self*, called as ``str(a)`` or ``repr(a)``` (where ``a`` is an ``array``).
        Returns the string 'array(<type>, [<elements>])', 
        where ``<type>`` is the type code letter for *slef* and ``<elements>`` is a comma seperated list of the elements of *self*.
	     **Note**: This method, ``__repr__``, cannot be called directly (``a.__repr__()`` fails) and the 
	     method is not present in ``__dict__``, however ``str(a)`` and ``repr(a)`` both work.
