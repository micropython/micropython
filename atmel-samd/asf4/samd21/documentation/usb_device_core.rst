===============
USB Device Core
===============

USB Device Core is a part of the USB Device Stack library. It handles all the requests
during USB Enumeration process and helps upper class driver to be installed successfully
if only the descriptor configuration is correct. For more detailed description about Bus
Enumeration, user can refer to Chapter 9 (USB Device Framework) in Universal Serial Bus
Specification.

Features
--------
* Initialization/de-initialization
* Enabling/disabling USB device class components
* Data Transfer
* Attaching/detaching control
* Waking up by USB host
* Callbacks management on:

  * SOF event
  * USB Device state change
  * USB Device request handler


Applications
------------

N/A

Dependencies
------------

* USB Device Driver
* USB Protocol Core


Limitations
-----------

* It is recommended that at least one device class should run above the USB
  device core.
