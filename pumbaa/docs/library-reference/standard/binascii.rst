:mod:`binascii` --- Convert between binary and ASCII
====================================================

.. module:: binascii
   :synopsis: Convert between binary and ASCII.

The binascii module contains a number of methods to convert between
binary and various ASCII-encoded binary representations.

----------------------------------------------


.. function:: binascii.a2b_base64(string)

   Convert a block of base64 data back to binary and return the binary
   data. More than one line may be passed at a time.


.. function:: binascii.b2a_base64(data)

   Convert binary `data` to a line of ASCII characters in base64
   coding. The return value is the converted line, including a newline
   char. The newline is added because the original use case for this
   function was to feed it a series of 57 byte input lines to get
   output lines that conform to the MIME-base64 standard. Otherwise
   the output conforms to RFC 3548.


.. function:: binascii.crc32(data[, value])

   Compute CRC-32, the 32-bit checksum of `data`, starting with an
   initial CRC of `value`. The default initial CRC is zero. The
   algorithm is consistent with the ZIP file checksum. Since the
   algorithm is designed for use as a checksum algorithm, it is not
   suitable for use as a general hash algorithm. Use as follows:

   .. code-block:: python

      >>> binascii.crc32(b"hello world")
      222957957
      >>> crc = binascii.crc32(b"hello")
      >>> binascii.crc32(b" world", crc)
      222957957


.. function:: binascii.hexlify(data)

   Return the hexadecimal representation of the binary `data`. Every
   byte of data is converted into the corresponding 2-digit hex
   representation. The returned bytes object is therefore twice as
   long as the length of data.


.. function:: binascii.unhexlify(hexstr)

   Return the binary data represented by the hexadecimal string
   `hexstr`. This function is the inverse of ``hexlify()``. `hexstr`
   must contain an even number of hexadecimal digits (which can be
   upper or lower case), otherwise an ``Error`` exception is raised.
