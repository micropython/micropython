.. currentmodule:: machine
.. _machine.UART:

class UART -- duplex serial communication bus
=============================================

UART implements the standard UART/USART duplex serial communications protocol.  At
the physical level it consists of 2 lines: RX and TX.  The unit of communication
is a character (not to be confused with a string character) which can be 8 or 9
bits wide.

UART objects can be created and initialised using::

    from machine import UART

    uart = UART(1, 9600)                         # init with given baudrate
    uart.init(9600, bits=8, parity=None, stop=1) # init with given parameters

Supported parameters on the W60x: Bits can be 8. Stop can be 1. With *parity=None*.

A UART object acts like a `stream` object and reading and writing is done
using the standard stream methods::

    uart.read(10)       # read 10 characters, returns a bytes object
    uart.read()         # read all available characters
    uart.readline()     # read a line
    uart.readinto(buf)  # read and store into the given buffer
    uart.write('abc')   # write the 3 characters

Constructors
------------

.. class:: UART(id, ...)

   Construct a UART object of the given id.

Methods
-------

.. method:: UART.init(baudrate=9600, bits=8, parity=None, stop=1, \*, ...)

   Initialise the UART bus with the given parameters:

     - *baudrate* is the clock rate.
     - *bits* is the number of bits per character, 7, 8 or 9.
     - *parity* is the parity, ``None``, 0 (even) or 1 (odd).
     - *stop* is the number of stop bits, 1 or 2.

.. method:: UART.read([nbytes])

   Read characters.  If ``nbytes`` is specified then read at most that many bytes,
   otherwise read as much data as possible.

   Return value: a bytes object containing the bytes read in.  Returns ``None``
   on timeout.

.. method:: UART.readinto(buf[, nbytes])

   Read bytes into the ``buf``.  If ``nbytes`` is specified then read at most
   that many bytes.  Otherwise, read at most ``len(buf)`` bytes.

   Return value: number of bytes read and stored into ``buf`` or ``None`` on
   timeout.

.. method:: UART.readline()

   Read a line, ending in a newline character.

   Return value: the line read or ``None`` on timeout.

.. method:: UART.write(buf)

   Write the buffer of bytes to the bus.

   Return value: number of bytes written or ``None`` on timeout.
