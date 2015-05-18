:mod:`ubinascii` -- binary/ASCII conversions
============================================

.. module:: ubinascii
   :synopsis: binary/ASCII conversions

This module implements conversions between binary data and various
encodings of it in ASCII form (in both directions).

Functions
---------

.. function:: hexlify(data)

   Convert binary data to hexadecimal representation. Return bytes string.

.. function:: unhexlify(data)

   Convert hexadecimal data to binary representation. Return bytes string.
   (i.e. inverse of hexlify)
