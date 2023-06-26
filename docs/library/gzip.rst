:mod:`gzip` -- gzip compression & decompression
===============================================

.. module:: gzip
   :synopsis: gzip compression & decompression

|see_cpython_module| :mod:`python:gzip`.

**Availability:**

* Added in MicroPython v1.21.

* Decompression: Enabled via the ``MICROPY_PY_GZIP`` build option, on by default
  on ports with the "extra features" level or higher (most boards).

* Compression: Enabled via the ``MICROPY_PY_GZIP_COMPRESS`` the build
  option. **TODO**

Functions
---------

.. class:: GzipFile(*, fileobj, mode, wbits=25, hist=None)

   This class can be used to wrap a *fileobj* which is any
   :term:`stream-like <stream>` object such as a file, socket, or stream
   (including :class:`io.BytesIO`). It is itself a stream and implements the
   standard read/readinto/write/close methods.

   When the *mode* argument is ``"rb"``, reads from the GzipFile instance will
   decompress the data in the underlying stream and return decompressed data.

   If compression support is enabled then the *mode* argument can be set to
   ``"wb"``, and writes to the GzipFile instance will be compressed and written
   to the underlying stream.

   By default the GzipFile class will read and write data using the gzip file
   format, including a header and footer with checksum and a window size of 512
   bytes.

   As a MicroPython-specific extension, the *wbits* argument may be specified to
   make the GzipFile instance use either the raw DEFLATE or zlib file format.
   See :meth:`zlib.compress` and :meth:`zlib.decompress` for more detail about
   how this parameter operates. For example, by setting ``wbits=9``, GzipFile
   can be used to read and write in zlib format with the minimum window size.

   Another MicroPython-specific extension is the *hist* parameter which allows
   the caller to pre-allocate the window buffer. This should be
   a :class:`python:bytearray` instance large enough for the specified value
   of *wbits*.

   The **file**, **compresslevel**, and **mtime** arguments are not
   supported. **fileobj** and **mode** must always be specified as keyword
   arguments.

.. note:: The :mod:`gzip.compress <python:gzip>` and :mod:`gzip.decompress <python:gzip>`
  functions are not implemented. Prefer to use :class:`gzip.GzipFile` as it
  allows more memory-efficient reading and writing from streams, or the
  :meth:`zlib.compress` and :meth:`zlib.decompress` functions as they allow
  control over the window size.

A typical use case for :class:`gzip.GzipFile` is to read or write a compressed
file from storage:

.. code:: python

   import gzip

   # Reading:
   with open("data.gz", "rb") as f:
       with gzip.GzipFile(fileobj=f, mode="rb") as g:
           # Use g.read(), g.readinto(), etc.

   # Writing:
   with open("data.gz", "wb") as f:
       with gzip.GzipFile(fileobj=f, mode="wb") as g:
           # Use g.write(...) etc

   # MicroPython extension: read zlib format (auto-detect wbits):
   with open("data.z", "rb") as f:
       with gzip.GzipFile(fileobj=f, mode="rb", wbits=0) as g:
           # Use g.read(), g.readinto(), etc.

   # MicroPython extension: write zlib format:
   with open("data.z", "wb") as f:
       with gzip.GzipFile(fileobj=f, mode="wb", wbits=9) as g:
           # Use g.write(...) etc

For guidance on working with gzip sources and choosing the window size see the
note at the :ref:`end of the zlib documentation <zlib_wbits>`.
