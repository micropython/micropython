.. currentmodule:: pyb
.. _pyb.USB_VCP:

class USB_VCP -- USB virtual comm port
======================================

The USB_VCP class allows creation of a `stream`-like object representing the USB
virtual comm port.  It can be used to read and write data over USB to
the connected host.


Constructors
------------

.. class:: pyb.USB_VCP()

   Create a new USB_VCP object.


Methods
-------

.. method:: USB_VCP.setinterrupt(chr)

   Set the character which interrupts running Python code.  This is set
   to 3 (CTRL-C) by default, and when a CTRL-C character is received over
   the USB VCP port, a KeyboardInterrupt exception is raised.

   Set to -1 to disable this interrupt feature.  This is useful when you
   want to send raw bytes over the USB VCP port.

.. method:: USB_VCP.isconnected()

   Return ``True`` if USB is connected as a serial device, else ``False``.

.. method:: USB_VCP.any()

   Return ``True`` if any characters waiting, else ``False``.

.. method:: USB_VCP.close()

   This method does nothing.  It exists so the USB_VCP object can act as
   a file.

.. method:: USB_VCP.read([nbytes])

   Read at most ``nbytes`` from the serial device and return them as a
   bytes object.  If ``nbytes`` is not specified then the method reads
   all available bytes from the serial device.
   USB_VCP `stream` implicitly works in non-blocking mode,
   so if no pending data available, this method will return immediately
   with ``None`` value.

.. method:: USB_VCP.readinto(buf, [maxlen])

   Read bytes from the serial device and store them into ``buf``, which
   should be a buffer-like object.  At most ``len(buf)`` bytes are read.
   If ``maxlen`` is given and then at most ``min(maxlen, len(buf))`` bytes
   are read.

   Returns the number of bytes read and stored into ``buf`` or ``None``
   if no pending data available.

.. method:: USB_VCP.readline()

   Read a whole line from the serial device.

   Returns a bytes object containing the data, including the trailing
   newline character or ``None`` if no pending data available.

.. method:: USB_VCP.readlines()

   Read as much data as possible from the serial device, breaking it into
   lines.

   Returns a list of bytes objects, each object being one of the lines.
   Each line will include the newline character.

.. method:: USB_VCP.write(buf)

   Write the bytes from ``buf`` to the serial device.

   Returns the number of bytes written.

.. method:: USB_VCP.recv(data, \*, timeout=5000)

   Receive data on the bus:
   
     - ``data`` can be an integer, which is the number of bytes to receive,
       or a mutable buffer, which will be filled with received bytes.
     - ``timeout`` is the timeout in milliseconds to wait for the receive.
   
   Return value: if ``data`` is an integer then a new buffer of the bytes received,
   otherwise the number of bytes read into ``data`` is returned.

.. method:: USB_VCP.send(data, \*, timeout=5000)

   Send data over the USB VCP:
   
     - ``data`` is the data to send (an integer to send, or a buffer object).
     - ``timeout`` is the timeout in milliseconds to wait for the send.
   
   Return value: number of bytes sent.
