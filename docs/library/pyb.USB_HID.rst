.. currentmodule:: pyb
.. _pyb.USB_HID:

class USB_HID -- USB Human Interface Device (HID)
=================================================

The USB_HID class allows creation of an object representing the USB
Human Interface Device (HID) interface.  It can be used to emulate
a peripheral such as a mouse or keyboard.

Before you can use this class, you need to use :meth:`pyb.usb_mode()` to set the USB mode to include the HID interface.

Constructors
------------

.. class:: pyb.USB_HID()

   Create a new USB_HID object.


Methods
-------

.. method:: USB_HID.recv(data, \*, timeout=5000)

   Receive data on the bus:

     - ``data`` can be an integer, which is the number of bytes to receive,
       or a mutable buffer, which will be filled with received bytes.
     - ``timeout`` is the timeout in milliseconds to wait for the receive.

   Return value: if ``data`` is an integer then a new buffer of the bytes received,
   otherwise the number of bytes read into ``data`` is returned.

.. method:: USB_HID.send(data)

   Send data over the USB HID interface:

     - ``data`` is the data to send (a tuple/list of integers, or a
       bytearray).
