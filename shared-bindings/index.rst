Core Modules
========================================

These core modules are intended on being consistent across ports. Currently
they are only implemented in the SAMD21 and ESP8266 ports. A module may not exist
in a port if no underlying hardware support is present or if flash space is
limited. For example, a microcontroller without analog features will not have
`analogio`.

Support Matrix
---------------

===============  ==========  =========  ===========  =======  =======  ===========  =================  ================  =========  ========  =========  =======  =========
Port             `analogio`  `audioio`  `bitbangio`  `board`  `busio`  `digitalio`  `microcontroller`  `neopixel_write`  `pulseio`  `time`    `touchio`  `uheap`  `usb_hid`
===============  ==========  =========  ===========  =======  =======  ===========  =================  ================  =========  ========  =========  =======  =========
SAMD21           **Yes**     No         No           **Yes**  **Yes**  **Yes**      **Yes**            **Yes**           No         **Yes**   **Yes**    Debug    **Yes**
SAMD21 Express   **Yes**     **Yes**    **Yes**      **Yes**  **Yes**  **Yes**      **Yes**            **Yes**           **Yes**    **Yes**   **Yes**    Debug    **Yes**
ESP8266          **Yes**     No         **Yes**      **Yes**  **Yes**  **Yes**      **Yes**            **Yes**           No         **Yes**   No         Debug    No
===============  ==========  =========  ===========  =======  =======  ===========  =================  ================  =========  ========  =========  =======  =========

Modules
---------

.. toctree::
    :glob:
    :maxdepth: 3

    */__init__
    help
