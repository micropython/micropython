:mod:`uio` -- input/output streams
==================================

.. module:: uio
   :synopsis: input/output streams

This module contains additional types of stream (file-like) objects
and helper functions.

Functions
---------

.. function:: open(name, mode='r', **kwargs)

    Open a file. Builtin ``open()`` function is aliased to this function.
    All ports (which provide access to file system) are required to support
    `mode` parameter, but support for other arguments vary by port.

Classes
-------

.. class:: FileIO(...)

    This is type of a file open in binary mode, e.g. using ``open(name, "rb")``.
    You should not instantiate this class directly.

.. class:: TextIOWrapper(...)

    This is type of a file open in text mode, e.g. using ``open(name, "rt")``.
    You should not instantiate this class directly.

.. class:: StringIO([string])
.. class:: BytesIO([string])

    In-memory file-like objects for input/output. `StringIO` is used for
    text-mode I/O (similar to a normal file opened with "t" modifier).
    `BytesIO` is used for binary-mode I/O (similar to a normal file
    opened with "b" modifier). Initial contents of file-like objects
    can be specified with `string` parameter (should be normal string
    for `StringIO` or bytes object for `BytesIO`). All the usual file
    methods like ``read()``, ``write()``, ``seek()``, ``flush()``,
    ``close()`` are available on these objects, and additionally, a
    following method:

    .. method:: getvalue()

        Get the current contents of the underlying buffer which holds data.
