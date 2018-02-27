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
NOTE 1: **All Supported** means the following ports are supported: SAMD21, SAMD21 Express,
SAMD51, SAMD51 Express, and ESP8266.

NOTE 2: **SAMD** and/or **SAMD Express** without additional numbers, means both SAMD21 & SAMD51 versions
are supported.

=================  ==============================
Module             Supported Ports
=================  ==============================
`analogio`         **All Supported**
`audiobusio`       **SAMD/SAMD Express**
`audioio`          **SAMD Express**
`bitbangio`        **SAMD Express, ESP8266**
`board`            **All Supported**
`busio`            **All Supported**
`digitalio`        **All Supported**
`gamepad`          **SAMD Express, nRF**
`math`             **All Supported**
`microcontroller`  **All Supported**
`multiterminal`    **ESP8266**
`neopixel_write`   **All Supported**
`nvm`              **SAMD Express**
`os`               **All Supported**
`pulseio`          **SAMD/SAMD Express**
`random`           **All Supported**
`storage`          **All Supported**
`struct`           **All Supported**
`supervisor`       **SAMD/SAMD Express**
`time`             **All Supported**
`touchio`          **SAMD/SAMD Express**
`uheap`            **Debug (All)**
`usb_hid`          **SAMD/SAMD Express**
=================  ==============================

Modules
---------

.. toctree::
    :glob:
    :maxdepth: 3

    */__init__
    help
