Core Modules
========================================

These core modules are intended on being consistent across ports. Currently
they are only implemented in the SAMD21 and ESP8266 ports. A module may not exist
in a port if no underlying hardware support is present or if flash space is
limited. For example, a microcontroller without analog features will not have
`analogio`.

Modules
---------

.. toctree::
    :glob:
    :maxdepth: 3

    support_matrix
    */__init__
    help
.. _module-support-matrix:
