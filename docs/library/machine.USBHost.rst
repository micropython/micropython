.. currentmodule:: machine
.. _machine.USBHost:

class USBHost -- USB Host driver
================================

USBHost allows MicroPython to act as a USB host, enumerating devices plugged into
the board's USB host port.  It uses the TinyUSB host stack, on a USB controller that
isn't used by the USB device port that provides the REPL, so both can be used at the
same time.

Availability: boards that enable it, which have a USB host port.  On stm32 these
include the STM32N6570-DK, and the board configuration chooses the controller (see
``MICROPY_HW_USB_HOST`` in ``ports/stm32/mpconfigboard_common.h``).

Lifecycle
---------

- The host port starts off inactive.  Call ``USBHost().active(True)`` to start it,
  after which attached devices are enumerated in the background.

- A MicroPython :ref:`soft reset <soft_reset>` deactivates the host port, disconnecting
  all devices.  The host must be activated again after a soft reset.

Constructors
------------

.. class:: USBHost()

   Construct a USBHost object.

   .. note:: This object is a singleton, each call to this constructor
             returns the same object reference.

Methods
-------

.. method:: USBHost.active([value])

   Returns the current active state of the USB host as a boolean.

   If the optional ``value`` argument is set to a truthy value, the host port is
   started and attached devices will be enumerated.  If set to a falsey value, devices
   are disconnected and the port is shut down.

.. method:: USBHost.devices()

   Returns a list of ``(address, vid, pid)`` tuples, one for each enumerated device.
