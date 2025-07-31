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

   If *wbits* is set to ``0`` (the default), then for compression a window size
   of 256 bytes will be used (as if *wbits* was set to 8). For decompression, it
   depends on the format:

   * ``RAW`` will use 256 bytes (corresponding to *wbits* set to 8).
   * ``ZLIB`` (or ``AUTO`` with zlib detected) will use the value from the zlib
     header.
   * ``GZIP`` (or ``AUTO`` with gzip detected) will use 32 kilobytes
     (corresponding to *wbits* set to 15).

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
reference. Increasing the window size will improve compression, but will require
more memory and make the compressor slower.

If an input stream was compressed a given window size, then `DeflateIO`
using a smaller window size will fail mid-way during decompression with
:exc:`OSError`, but only if a back-reference actually refers back further
than the decompressor's window size. This means it may be possible to decompress
with a smaller window size. For example, this would trivially be the case if the
original uncompressed data is shorter than the window size.

Decompression
~~~~~~~~~~~~~

The zlib format includes a header which specifies the window size that was used
to compress the data. This indicates the maximum window size required to
decompress this stream. If this header value is less than the specified *wbits*
value (or if *wbits* is unset), then the header value will be used.

The gzip format does not include the window size in the header, and assumes that
all gzip compressors (e.g. the ``gzip`` utility, or CPython's implementation of
:class:`gzip.GzipFile`) use the maximum window size of 32kiB. For this reason,
if the *wbits* parameter is not set, the decompressor will use a 32 kiB window
size (corresponding to *wbits* set to 15). This means that to be able to
decompress an arbitrary gzip stream, you must have at least this much RAM
available. If you control the source data, consider instead using the zlib
format with a smaller window size.

The raw format has no header and therefore does not include any information
about the window size. If *wbits* is not set, then it will default to a window
size of 256 bytes, which may not be large enough for a given stream. Therefore
it is recommended that you should always explicitly set *wbits* if using the raw
format.

Compression
~~~~~~~~~~~

For compression, MicroPython will default to a window size of 256 bytes for all
formats. This provides a reasonable amount of compression with minimal memory
usage and fast compression time, and will generate output that will work with
any decompressor.
