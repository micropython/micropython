:mod:`zlib` -- zlib decompression
=================================

.. module:: zlib
   :synopsis: zlib decompression

|see_cpython_module| :mod:`python:zlib`.

This module allows to decompress binary data compressed with
`DEFLATE algorithm <https://en.wikipedia.org/wiki/DEFLATE>`_
(commonly used in zlib library and gzip archiver). Compression
is not yet implemented.

Functions
---------

.. function:: decompress(data, wbits=0, bufsize=0, /)

   Return decompressed *data* as bytes. *wbits* is DEFLATE dictionary window
   size used during compression (8-15, the dictionary size is power of 2 of
   that value). Additionally, if value is positive, *data* is assumed to be
   zlib stream (with zlib header). Otherwise, if it's negative, it's assumed
   to be raw DEFLATE stream. *bufsize* parameter is for compatibility with
   CPython and is ignored.

.. class:: DecompIO(stream, wbits=0, /)

   Create a `stream` wrapper which allows transparent decompression of
   compressed data in another *stream*. This allows to process compressed
   streams with data larger than available heap size. In addition to
   values described in :func:`decompress`, *wbits* may take values
   24..31 (16 + 8..15), meaning that input stream has gzip header.

   .. admonition:: Difference to CPython
      :class: attention

      This class is MicroPython extension. It's included on provisional
      basis and may be changed considerably or removed in later versions.

.. function:: DecompIO.set_stream(obj, stream, wbits=0, /)

   Update a `DecompIO` to use a new stream object.  This is exactly like
   creating a new `DecompIO` object, except the buffer (usually 32k) is
   reused and therefore doesn't have to be reallocated.  This can 
   prevent a random `MemoryError` due to memory fragmentation when
   processing large numbers of streams.
   
   The buffer sizes of the two streams must match, or this will throw
   a `ValueError`.

