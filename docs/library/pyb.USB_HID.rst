.. currentmodule:: pyb

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

.. method:: USB_HID.send(data)

   Send data over the USB HID interface:

     - ``data`` is the data to send (a tuple/list of integers, or a
       bytearray).
