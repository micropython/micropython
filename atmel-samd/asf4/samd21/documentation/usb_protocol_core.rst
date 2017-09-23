=================
USB Protocol Core
=================

USB Protocol Core is a part of the USB Device Stack library. It provides basic
macro definitions, data structures and API functions which are compliant with
USB specification version 2.0.0/2.0.1 for user.

In addition, it provides interfaces of USB descriptor searching for the upper
layer to use, which is especially helpful in the USB enumeration process.

Features
--------
* Packaging USB device, configuration, interface, endpoint descriptors.
* Getting the descriptor length or type from an assigned USB descriptor.
* Getting VID, PID value from an assigned USB descriptor.
* Getting the total length of an USB configuration descriptor.
* Finding the next valid descriptor in a given USB descriptor.
* Finding an endpoint descriptor in a given USB descriptor.
* Finding a specific descriptor by an assigned descriptor type.
* Finding a configure descriptor by an assigned configure value.
* Finding a string descriptor by an assigned string index.


Applications
------------

N/A

Dependencies
------------

N/A


Limitations
-----------

N/A

