:mod:`deflate` -- deflate compression & decompression
=====================================================

.. module:: deflate
   :synopsis: deflate compression & decompression

This module allows compression and decompression of binary data with the
`DEFLATE algorithm <https://en.wikipedia.org/wiki/DEFLATE>`_
(commonly used in the zlib library and gzip archiver).

**Availability:**

* Added in MicroPython v1.21.

* Decompression: Enabled via the ``MICROPY_PY_DEFLATE`` build option, on by default
  on ports with the "extra features" level or higher (which is most boards).

* Compression: Enabled via the ``MICROPY_PY_DEFLATE_COMPRESS`` build option, on
  by default on ports with the "full features" level or higher (generally this means
  you need to build your own firmware to enable this).

Classes
-------

.. class:: DeflateIO(stream, format=AUTO, wbits=0, close=False, /)

   This class can be used to wrap a *stream* which is any
   :term:`stream-like <stream>` object such as a file, socket, or stream
   (including :class:`io.BytesIO`). It is itself a stream and implements the
   standard read/readinto/write/close methods.

   The *stream* must be a blocking stream. Non-blocking streams are currently
   not supported.

   The *format* can be set to any of the constants defined below, and defaults
   to ``AUTO`` which for decompressing will auto-detect gzip or zlib streams,
   and for compressing it will generate a raw stream.

   The *wbits* parameter sets the base-2 logarithm of the DEFLATE dictionary
   window size. So for example, setting *wbits* to ``10`` sets the window size
   to 1024 bytes. Valid values are ``5`` to ``15`` inclusive (corresponding to
   window sizes of 32 to 32k bytes).

   If *wbits* is set to ``0`` (the default), then a window size of 256 bytes
   will be used (corresponding to *wbits* set to ``8``), except when
   :ref:`decompressing a zlib stream <deflate_wbits_zlib>`.

   See the :ref:`window size <deflate_wbits>` notes below for more information
   about the window size, zlib, and gzip streams.

   If *close* is set to ``True`` then the underlying stream will be closed
   automatically when the :class:`deflate.DeflateIO` stream is closed. This is
   useful if you want to return a :class:`deflate.DeflateIO` stream that wraps
   another stream and not have the caller need to know about managing the
   underlying stream.

   If compression is enabled, a given :class:`deflate.DeflateIO` instance
   supports both reading and writing. For example, a bidirectional stream like
   a socket can be wrapped, which allows for compression/decompression in both
   directions.

Constants
---------

.. data:: deflate.AUTO
          deflate.RAW
          deflate.ZLIB
          deflate.GZIP

    Supported values for the *format* parameter.

Examples
--------

A typical use case for :class:`deflate.DeflateIO` is to read or write a compressed
file from storage:

.. code:: python

   import deflate

   # Writing a zlib-compressed stream (uses the default window size of 256 bytes).
   with open("data.gz", "wb") as f:
       with deflate.DeflateIO(f, deflate.ZLIB) as d:
           # Use d.write(...) etc

   # Reading a zlib-compressed stream (auto-detect window size).
   with open("data.z", "rb") as f:
       with deflate.DeflateIO(f, deflate.ZLIB) as d:
           # Use d.read(), d.readinto(), etc.

Because :class:`deflate.DeflateIO` is a stream, it can be used for example
with :meth:`json.dump` and :meth:`json.load` (and any other places streams can
be used):

.. code:: python

   import deflate, json

   # Write a dictionary as JSON in gzip format, with a
   # small (64 byte) window size.
   config = { ... }
   with open("config.gz", "wb") as f:
       with deflate.DeflateIO(f, deflate.GZIP, 6) as f:
           json.dump(config, f)

   # Read back that dictionary.
   with open("config.gz", "rb") as f:
       with deflate.DeflateIO(f, deflate.GZIP, 6) as f:
           config = json.load(f)

If your source data is not in a stream format, you can use :class:`io.BytesIO`
to turn it into a stream suitable for use with :class:`deflate.DeflateIO`:

.. code:: python

   import deflate, io

   # Decompress a bytes/bytearray value.
   compressed_data = get_data_z()
   with deflate.DeflateIO(io.BytesIO(compressed_data), deflate.ZLIB) as d:
       decompressed_data = d.read()

   # Compress a bytes/bytearray value.
   uncompressed_data = get_data()
   stream = io.BytesIO()
   with deflate.DeflateIO(stream, deflate.ZLIB) as d:
       d.write(uncompressed_data)
   compressed_data = stream.getvalue()

.. _deflate_wbits:

Deflate window size
-------------------

The window size limits how far back in the stream the (de)compressor can
reference. Increasing the window size will improve compression, but will
require more memory.

However, just because a given window size is used for compression, this does not
mean that the stream will require the same size window for decompression, as
the stream may not reference data as far back as the window allows (for example,
if the length of the input is smaller than the window size).

If the decompressor uses a smaller window size than necessary for the input data
stream, it will fail mid-way through decompression with :exc:`OSError`.

.. _deflate_wbits_zlib:

The zlib format includes a header which specifies the window size used to
compress the data (which due to the above, may be larger than the size required
for the decompressor).

If this header value is lower than the specified *wbits* value, then the header
value will be used instead in order to reduce the memory allocation size. If
the *wbits* parameter is zero (the default), then the header value will only be
used if it is less than the maximum value of ``15`` (which is default value
used by most compressors [#f1]_).

In other words, if the source zlib stream has been compressed with a custom window
size (i.e. less than ``15``), then using the default *wbits* parameter of zero
will decompress any such stream.

The gzip file format does not include the window size in the header.
Additionally, most compressor libraries (including CPython's implementation
of :class:`gzip.GzipFile`) will default to the maximum possible window size.
This makes it difficult to decompress most gzip streams on MicroPython unless
your board has a lot of free RAM.

If you control the source of the compressed data, then prefer to use the zlib
format, with a window size that is suitable for your target device.

.. rubric:: Footnotes

.. [#f1] The assumption here is that if the header value is the default used by
   most compressors, then nothing is known about the likely required window
   size and we should ignore it.
