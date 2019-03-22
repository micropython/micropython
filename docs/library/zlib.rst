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
   that value). Additionally: 
   * if the value is negative, *data* is assumed to be raw DEFLATE stream. 
   * if the value has 16 added to it (24-31, 16 + 8..15) *data* will be 
   * handed as gzip
   * Otherwise *data* is assumed to be zlib stream (with zlib header). 
   *bufsize* parameter is for compatibility with CPython and is ignored.

.. function:: compress(data, wbits=15 /)

   Return gzip DEFLATE compressed *data* as bytearray. *wbits* is DEFLATE dictionary window
   size used during compression (8-15, the dictionary size is power of 2 of
   that value). *data* can be decompressed with *DecompIO()* or *decompess()* 
   in gzip mode (*wbits=31*)

.. class:: DecompIO(stream, wbits=0, /)

   Create a `stream` wrapper which allows transparent decompression of
   compressed data in another *stream*. This allows to process compressed
   streams with data larger than available heap size.

   .. admonition:: Difference to CPython
      :class: attention

      This class is MicroPython extension. It's included on provisional
      basis and may be changed considerably or removed in later versions.
