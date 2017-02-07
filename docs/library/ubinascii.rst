:mod:`ubinascii` -- binary/ASCII conversions
============================================

.. module:: ubinascii
   :synopsis: binary/ASCII conversions

This module implements conversions between binary data and various
encodings of it in ASCII form (in both directions).

Functions
---------

.. function:: hexlify(data, [sep])

   Convert binary data to hexadecimal representation. Returns bytes string.

   .. admonition:: Difference to CPython
      :class: attention

      If additional argument, `sep` is supplied, it is used as a separator
      between hexadecimal values.

.. function:: unhexlify(data)

   Convert hexadecimal data to binary representation. Returns bytes string.
   (i.e. inverse of hexlify)

.. function:: a2b_base64(data)

   Convert Base64-encoded data to binary representation. Returns bytes string.

.. function:: b2a_base64(data)

   Encode binary data in Base64 format. Returns string.
