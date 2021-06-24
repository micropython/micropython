.. _production:

Using MicroPython in Production
===============================

MicroPython offers a rich set of features and compatibility that make is an attractive option 
for use in production code. This section intends to make it easier to find simple well-accepted
answers to questions that may appear.

Generally the best solutions leverage existing features of MicroPython or only require minimal
modification to the repository.


Adding Board Definitions
------------------------

MicroPython supports many platforms including various microcontrollers, operating systems, and
languages. Microcontrollers in particular exhibit a wide variety of capabilities depending on
the architecture, CPU, included peripherals, and even what hardware they are connected to in a
product.

To accomodate this variety MicroPython includes board definitions. Each microcontroller port 
comes with a generic board and may include other board definitions. Adding a custom board definition
is typically easy and is independent of the main MicroPython internals.

The exact requirements for adding a custom board vary between ports. Generally it is best to begin
by copying a similar board definition and changing values to match your board.


Main Program Execution
----------------------
A standard build of MicroPython is intended for development boards. Any interested individual can
fire up the REPL and begin learning more about microcontrollers. To transition to a production
build one may wish to:

* provide a main application written in MicroPython
* protect the main application from being overwritten
* prevent users from interfering with bootup

*dictating a main program execution*

MicroPython allows users to determine their own main application flow by modifying **boot.py** and 
**main.py**. In order to provide a particular main program execution one can freeze **main.py** and
**boot.py** during the build process. These files are baked into the firmware and visible to the 
interpreter just like files added to the filesystem during runtime. Note: bytecode frozen into the
firmware image cannot be overwritten.

*protecting a main application*

Guaranteeing proper functionality on a production product is crucial. It can be concerning that users
have the potential to access the filesystem and create **main.py** / **boot.py**, thereby influencing
runtime execution. There are two main concerns: removal / destruction of the intended main program and 
overriding the main program.

* modules frozen into the firmware are write-protected (barring extreme cases such as self-modifying code)
* when a module exists in both frozen bytecode and on the filesystem the frozen module is preferred

*preventing hijacking of main program execution*

There are three ways that end users can dictate program execution built into MicroPython.

* providing **boot.py** module
* providing **main.py** module
* accessing the REPL

Freezing the **boot.py** and **main.py** modules into firmware close the first two opportunities. When
the **main.py** module stops running, either successfully or not, program flow continues to the REPL.

An infinite loop is a simple method to try to keep the main program from exiting. This would work until
an exception went uncaught. To combat this possibility one can add a generic exception handling method
within the loop as demonstrated in the following code:

**main.py**

.. code-block:: python

   while True:
      try:
         raise Exception('simulate an uncaught exception')
      except:
         print('exception caught in main program')

.. note::
   note that the **KeyboardInterrupt** is not caught in the above exception handling example and so it is
   still possible for the user to terminate the main program execution and access the REPL given access to
   stdin (most likely a UART on microcontrollers)

Knowing these facts lets one rest assured that freezing their main program flow offers sufficient 
protection.
