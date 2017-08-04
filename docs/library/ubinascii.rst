:mod:`ubinascii` -- binary/ASCII conversions
============================================

.. module:: ubinascii
   :synopsis: binary/ASCII conversions

|see_cpython_module| :mod:`python:binascii`.

This module implements conversions between binary data and various
encodings of it in ASCII form (in both directions).

Functions
---------

.. function:: hexlify(data, [sep])

   Convert binary data to hexadecimal representation. Returns bytes string.

   .. admonition:: Difference to CPython
      :class: attention

      If additional argument, *sep* is supplied, it is used as a separator
      between hexadecimal values.

.. function:: unhexlify(data)

   Convert hexadecimal data to binary representation. Returns bytes string.
   (i.e. inverse of hexlify)

.. function:: a2b_base64(data)

   Decode base64-encoded data, ignoring invalid characters in the input.
   Conforms to `RFC 2045 s.6.8 <https://tools.ietf.org/html/rfc2045#section-6.8>`_.
   Returns a bytes object.

.. function:: b2a_base64(data)

   Encode binary data in base64 format, as in `RFC 3548
   <https://tools.ietf.org/html/rfc3548.html>`_. Returns the encoded data
   followed by a newline character, as a bytes object.
