Interactive
===========

About
-----

This application is a Python interpreter!

When the application starts it tries to run the script ``main.py``
from the file system. After the script ends the `Python` interactive
interpreter is started.

The serial port baudrate is 38400.

Example script
--------------

Here is an example of how to write a script ``main.py`` using the
interpreter.

1. Start the serial monitor.
   
2. Create ``main.py`` and write ``print('Hello World!\n')`` to
   it. This file will be executed everytime the board starts.

   .. code-block:: text
   
      MicroPython v1.8.3-88-gf98bb2d on 2016-09-17; Arduino Due with SAM3X8E
      Type "help()" for more information.
      >>> with open("main.py", "w") as f:
      ...     f.write("print('Hello World!\n')")
      >>>

3. Restart the board and you'll see ``Hello World!`` on the screen!

   .. code-block:: text

      Hello World!

      MicroPython v1.8.3-88-gf98bb2d on 2016-09-17; Arduino Due with SAM3X8E
      Type "help()" for more information.
      >>>

4. Done!

The example can be found on Github in the
:github-tree:`examples/interactive` folder.

Build and run
-------------

Build and run the application.

.. code-block:: text

   $ cd examples/interactive
   $ make -s BOARD=arduino_due run
   ...
   MicroPython v1.8.3-88-gf98bb2d on 2016-09-17; Arduino Due with SAM3X8E
   Type "help()" for more information.
   >>>
