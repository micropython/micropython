class Pin --- control I/O pins
==============================

A pin is the basic object to control I/O pins.  It has methods to set
the mode of the pin (input, output, etc) and methods to get and set the
digital logic level.  For analog control of a pin, see the ADC class.

Usage Model:

All Board Pins are predefined as pyb.Pin.board.Name ::

    x1_pin = pyb.Pin.board.X1

    g = pyb.Pin(pyb.Pin.board.X1, pyb.Pin.IN)

CPU pins which correspond to the board pins are available
as ``pyb.cpu.Name``. For the CPU pins, the names are the port letter
followed by the pin number. On the PYBv1.0, ``pyb.Pin.board.X1`` and
``pyb.Pin.cpu.B6`` are the same pin.

You can also use strings::

    g = pyb.Pin('X1', pyb.Pin.OUT_PP)

Users can add their own names::

    MyMapperDict = { 'LeftMotorDir' : pyb.Pin.cpu.C12 }
    pyb.Pin.dict(MyMapperDict)
    g = pyb.Pin("LeftMotorDir", pyb.Pin.OUT_OD)

and can query mappings ::

    pin = pyb.Pin("LeftMotorDir")

Users can also add their own mapping function::

    def MyMapper(pin_name):
       if pin_name == "LeftMotorDir":
           return pyb.Pin.cpu.A0

    pyb.Pin.mapper(MyMapper)

So, if you were to call: ``pyb.Pin("LeftMotorDir", pyb.Pin.OUT_PP)``
then ``"LeftMotorDir"`` is passed directly to the mapper function.

To summarise, the following order determines how things get mapped into
an ordinal pin number:

1. Directly specify a pin object
2. User supplied mapping function
3. User supplied mapping (object must be usable as a dictionary key)
4. Supply a string which matches a board pin
5. Supply a string which matches a CPU port/pin

You can set ``pyb.Pin.debug(True)`` to get some debug information about
how a particular object gets mapped to a pin.


Constructors
------------

.. class:: pyb.Pin(id, ...)

   Create a new Pin object associated with the id.  If additional arguments are given,
   they are used to initialise the pin.  See ``init``.


Class methods
-------------

.. method:: Pin.af_list()

   Returns an array of alternate functions available for this pin.

.. method:: Pin.debug([state])

   Get or set the debugging state (``True`` or ``False`` for on or off).

.. method:: Pin.dict([dict])

   Get or set the pin mapper dictionary.

.. method:: Pin.mapper([fun])

   Get or set the pin mapper function.


Methods
-------

.. method:: pin.__str__()

   Return a string describing the pin object.

.. method:: pin.af()

   Returns the currently configured alternate-function of the pin. The
   integer returned will match one of the allowed constants for the af
   argument to the init function.

.. method:: pin.gpio()

   Returns the base address of the GPIO block associated with this pin.

.. method:: pin.high()

   Set the pin to a high logic level.

.. method:: pin.init(mode, pull=Pin.PULL_NONE, af=-1)

   Initialise the pin:
   
     - ``mode`` can be one of:
       - ``Pin.IN`` - configure the pin for input;
       - ``Pin.OUT_PP`` - configure the pin for output, with push-pull control;
       - ``Pin.OUT_OD`` - configure the pin for output, with open-drain control;
       - ``Pin.AF_PP`` - configure the pin for alternate function, pull-pull;
       - ``Pin.AF_OD`` - configure the pin for alternate function, open-drain;
       - ``Pin.ANALOG`` - configure the pin for analog.
     - ``pull`` can be one of:
       - ``Pin.PULL_NONE`` - no pull up or down resistors;
       - ``Pin.PULL_UP`` - enable the pull-up resistor;
       - ``Pin.PULL_DOWN`` - enable the pull-down resistor.
     - when mode is Pin.AF_PP or Pin.AF_OD, then af can be the index or name
       of one of the alternate functions associated with a pin.
   
   Returns: ``None``.

.. method:: pin.low()

   Set the pin to a low logic level.

.. method:: pin.mode()

   Returns the currently configured mode of the pin. The integer returned
   will match one of the allowed constants for the mode argument to the init
   function.

.. method:: pin.name()

   Get the pin name.

.. method:: pin.names()

   Returns the cpu and board names for this pin.

.. method:: pin.pin()

   Get the pin number.

.. method:: pin.port()

   Get the pin port.

.. method:: pin.pull()

   Returns the currently configured pull of the pin. The integer returned
   will match one of the allowed constants for the pull argument to the init
   function.

.. method:: pin.value([value])

   Get or set the digital logic level of the pin:
   
     - With no argument, return 0 or 1 depending on the logic level of the pin.
     - With ``value`` given, set the logic level of the pin.  ``value`` can be
       anything that converts to a boolean.  If it converts to ``True``, the pin
       is set high, otherwise it is set low.


Constants
---------

.. data:: Pin.AF_OD

   initialise the pin to alternate-function mode with an open-drain drive

.. data:: Pin.AF_PP

   initialise the pin to alternate-function mode with a push-pull drive

.. data:: Pin.ANALOG

   initialise the pin to analog mode

.. data:: Pin.IN

   initialise the pin to input mode

.. data:: Pin.OUT_OD

   initialise the pin to output mode with an open-drain drive

.. data:: Pin.OUT_PP

   initialise the pin to output mode with a push-pull drive

.. data:: Pin.PULL_DOWN

   enable the pull-down resistor on the pin

.. data:: Pin.PULL_NONE

   don't enable any pull up or down resistors on the pin

.. data:: Pin.PULL_UP

   enable the pull-up resistor on the pin
