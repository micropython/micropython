Micro Python libraries
======================

Python standard libraries
-------------------------

The following standard Python libraries are built in to Micro Python.

For additional libraries, please download them from the `micropython-lib repository
<https://github.com/micropython/micropython-lib>`_.

.. toctree::
   :maxdepth: 1

   cmath.rst
   gc.rst
   math.rst
   os.rst
   select.rst
   struct.rst
   sys.rst
   time.rst

Python micro-libraries
----------------------

The following standard Python libraries have been "micro-ified" to fit in with
the philosophy of Micro Python.  They provide the core functionality of that
module and are intended to be a drop-in replacement for the standard Python
library.

The modules are available by their u-name, and also by their non-u-name.  The
non-u-name can be overridden by a file of that name in your package path.
For example, ``import json`` will first search for a file ``json.py`` or
directory ``json`` and load that package if it is found.  If nothing is found,
it will fallback to loading the built-in ``ujson`` module.

.. toctree::
   :maxdepth: 1

   ubinascii.rst
   uctypes.rst
   uhashlib.rst
   uheapq.rst
   ujson.rst
   ure.rst
   usocket.rst
   uzlib.rst

Libraries specific to the pyboard
---------------------------------

The following libraries are specific to the pyboard.

.. toctree::
   :maxdepth: 2

   pyb.rst
   network.rst
