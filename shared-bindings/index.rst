Core Modules
========================================

These core modules are intended on being consistent across ports. Currently
they are only implemented in the SAMD21 and ESP8266 ports. A module may not exist
in a port if no underlying hardware support is present or if flash space is
limited. For example, a microcontroller without analog features will not have
`analogio`.

.. _module-support-matrix:

Support Matrix
---------------

=================  =======  ==============  =======
Module / Port      SAMD21   SAMD21 Express  ESP8266
=================  =======  ==============  =======
`analogio`         **Yes**  **Yes**         **Yes**
`audioio`          No       **Yes**         No
`bitbangio`        No       **Yes**         **Yes**
`board`            **Yes**  **Yes**         **Yes**
`busio`            **Yes**  **Yes**         **Yes**
`digitalio`        **Yes**  **Yes**         **Yes**
`microcontroller`  **Yes**  **Yes**         **Yes**  
`multiterminal`    No       No              **Yes**
`neopixel_write`   **Yes**  **Yes**         **Yes**  
`nvm`              No       **Yes**         No
`os`               **Yes**  **Yes**         **Yes**
`pulseio`          No       **Yes**         No
`random`           **Yes**  **Yes**         **Yes**
`storage`          **Yes**  **Yes**         **Yes**
`time`             **Yes**  **Yes**         **Yes**
`touchio`          **Yes**  **Yes**         No
`uheap`            Debug    Debug           Debug
`usb_hid`          **Yes**  **Yes**         No
=================  =======  ==============  =======

Modules
---------

.. toctree::
    :glob:
    :maxdepth: 3

    */__init__
    help
