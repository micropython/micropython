class USB_VCP -- USB virtual comm port
======================================

The USB_VCP class allows creation of an object representing the USB
virtual comm port.  It can be used to read and write data over USB to
the connected host.


Constructors
------------

.. class:: pyb.USB_VCP()

   Create a new USB_VCP object.


Methods
-------

.. method:: usb_vcp.setinterrupt(chr)

   Set the character which interrupts running Python code.  This is set
   to 3 (CTRL-C) by default, and when a CTRL-C character is received over
   the USB VCP port, a KeyboardInterrupt exception is raised.

   Set to -1 to disable this interrupt feature.  This is useful when you
   want to send raw bytes over the USB VCP port.

.. method:: usb_vcp.isconnected()

   Return ``True`` if USB is connected as a serial device, else ``False``.

.. method:: usb_vcp.any()

   Return ``True`` if any characters waiting, else ``False``.

.. method:: usb_vcp.close()


.. method:: usb_vcp.read([nbytes])


.. method:: usb_vcp.readall()


.. method:: usb_vcp.readline()


.. method:: usb_vcp.recv(data, \*, timeout=5000)

   Receive data on the bus:
   
     - ``data`` can be an integer, which is the number of bytes to receive,
       or a mutable buffer, which will be filled with received bytes.
     - ``timeout`` is the timeout in milliseconds to wait for the receive.
   
   Return value: if ``data`` is an integer then a new buffer of the bytes received,
   otherwise the number of bytes read into ``data`` is returned.

.. method:: usb_vcp.send(data, \*, timeout=5000)

   Send data over the USB VCP:
   
     - ``data`` is the data to send (an integer to send, or a buffer object).
     - ``timeout`` is the timeout in milliseconds to wait for the send.
   
   Return value: number of bytes sent.

.. method:: usb_vcp.write(buf)

