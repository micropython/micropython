:mod:`io` --- Core tools for working with streams
=================================================

.. module:: io
   :synopsis: Core tools for working with streams.

The io module provides Python’s main facilities for dealing with
various types of I/O. There are two main types of I/O: text I/O and
binary I/O. These are generic categories, and various backing stores
can be used for each of them.

----------------------------------------------


.. function:: io.open(name, mode='r')

   Open a file named `name`. `mode` is a combination of the characters
   ``'rwbta'``.


.. class:: io.FileIO(...)

   This is type of a file open in binary mode, e.g. using ``open(name,
   "rb")``. You should not instantiate this class directly.


.. class:: io.TextIOWrapper(...)

   This is type of a file open in text mode, e.g. using ``open(name,
   "rt")``. You should not instantiate this class directly.


.. class:: uio.StringIO([string])


.. class:: uio.BytesIO([string])

   In-memory file-like objects for input/output. ``StringIO`` is used
   for text-mode I/O (similar to a normal file opened with ``"t"``
   modifier). ``BytesIO`` is used for binary-mode I/O (similar to a
   normal file opened with ``"b"`` modifier). Initial contents of
   file-like objects can be specified with string parameter (should be
   normal string for ``StringIO`` or bytes object for
   ``BytesIO``).


   .. method:: read(size=-1)

      Read up to `size` bytes fro mthe file. If `size` is negative or
      ``None``, read until end of file.


   .. method:: readall()

      Read until end of file.


   .. method:: readline()

      Read a line.


   .. method:: write(b)

      Write `b` to the file.


   .. method:: seek(offset[, whence])

      Move the file cursor `offset` bytes relative to beginning of the
      file (``0``), current position (``1``) or end of the file
      (``2``). The default value of `whence` is ``0``.


   .. method:: flush()

      Flush all buffers.


   .. method:: close()

      Close the file.


   .. method:: getvalue()

      Get the current contents of the underlying buffer which holds
      data.
