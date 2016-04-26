MicroPython libraries
=====================

MicroPython-specific libraries
------------------------------

Functionality specific to the MicroPython implementation is available in
the following libraries.

.. toctree::
   :maxdepth: 1

   machine.rst
   micropython.rst
   network.rst
   uctypes.rst

Python standard libraries
-------------------------

The following standard Python libraries are built in to MicroPython.

For additional libraries, please download them from the `micropython-lib repository
<https://github.com/micropython/micropython-lib>`_.

.. only:: port_unix

    .. toctree::
       :maxdepth: 1
    
       cmath.rst
       gc.rst
       math.rst
       os.rst
       sys.rst
       time.rst

.. only:: port_pyboard

    .. toctree::
       :maxdepth: 1
    
       cmath.rst
       gc.rst
       math.rst
       os.rst
       select.rst
       sys.rst
       time.rst

.. only:: port_wipy

    .. toctree::
       :maxdepth: 1
    
       gc.rst
       os.rst
       select.rst
       sys.rst
       time.rst

.. only:: port_esp8266

    .. toctree::
       :maxdepth: 1

       gc.rst
       math.rst
       sys.rst
       time.rst

Python micro-libraries
----------------------

The following standard Python libraries have been "micro-ified" to fit in with
the philosophy of MicroPython.  They provide the core functionality of that
module and are intended to be a drop-in replacement for the standard Python
library.

.. only:: not port_unix

    The modules are available by their u-name, and also by their non-u-name.  The
    non-u-name can be overridden by a file of that name in your package path.
    For example, ``import json`` will first search for a file ``json.py`` or
    directory ``json`` and load that package if it is found.  If nothing is found,
    it will fallback to loading the built-in ``ujson`` module.

.. only:: port_pyboard or port_unix

   .. toctree::
      :maxdepth: 1
    
      ubinascii.rst
      uhashlib.rst
      uheapq.rst
      ujson.rst
      ure.rst
      usocket.rst
      ustruct.rst
      uzlib.rst

.. only:: port_wipy

   .. toctree::
      :maxdepth: 1

      ubinascii.rst
      ujson.rst
      ure.rst
      usocket.rst
      ussl.rst

.. only:: port_esp8266

   .. toctree::
      :maxdepth: 1

      ubinascii.rst
      uhashlib.rst
      uheapq.rst
      ujson.rst
      ure.rst
      usocket.rst
      ustruct.rst
      uzlib.rst

.. only:: port_pyboard

   Libraries specific to the pyboard
   ---------------------------------

   The following libraries are specific to the pyboard.

   .. toctree::
      :maxdepth: 2

      pyb.rst

.. only:: port_wipy

   Libraries specific to the WiPy
   ---------------------------------

   The following libraries are specific to the WiPy.

   .. toctree::
      :maxdepth: 2

      wipy.rst


.. only:: port_esp8266

   Libraries specific to the ESP8266
   ---------------------------------

   The following libraries are specific to the ESP8266.

   .. toctree::
      :maxdepth: 2

      esp.rst
