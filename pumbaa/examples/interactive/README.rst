Interactive Interpreter
=======================

The Python interactive interpreter example. 

Build the application and upload it to the board.

.. code-block:: text

   make -s BOARD=arduino_due upload
   make -s BOARD=arduino_due console

Execute Python code in the interpreter.
   
.. code-block:: python

   MicroPython v1.8.3-88-gf98bb2d on 2016-09-17; Arduino Due with SAM3X8E
   Type "help()" for more information.
   >>> 1+2
   3
   >>> for i in range(10):
   ...     print(i)
   ...
   0
   1
   2
   3
   4
   5
   6
   7
   8
   9
   >>>
