.. _micropython_lib:

MicroPython libraries
=====================

.. warning::

    These modules are inherited from MicroPython and may not work in CircuitPython
    as documented or at all! If they do work, they may change at any time.


Python standard libraries and micro-libraries
---------------------------------------------


.. toctree::
   :maxdepth: 1

   builtins.rst
   array.rst
   gc.rst
   sys.rst
   ubinascii.rst
   ucollections.rst
   uerrno.rst
   uhashlib.rst
   uheapq.rst
   uio.rst
   ujson.rst
   ure.rst
   uselect.rst
   usocket.rst
   ussl.rst
   ustruct.rst
   uzlib.rst


MicroPython-specific libraries
------------------------------

Functionality specific to the MicroPython implementation is available in
the following libraries.

.. toctree::
   :maxdepth: 1

   btree.rst
   framebuf.rst
   micropython.rst
   network.rst
   uctypes.rst

Libraries specific to the ESP8266
---------------------------------

The following libraries are specific to the ESP8266.

.. toctree::
  :maxdepth: 2

  esp.rst
