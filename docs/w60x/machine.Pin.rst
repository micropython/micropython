.. currentmodule:: machine
.. _machine.Pin:

class Pin -- control I/O pins
=============================

A pin object is used to control I/O pins (also known as GPIO - general-purpose
input/output).  Pin objects are commonly associated with a physical pin that can
drive an output voltage and read input voltages.  The pin class has methods to set the mode of
the pin (IN, OUT, etc) and methods to get and set the digital logic level.
For analog control of a pin, see the :class:`ADC` class.

A pin object is constructed by using an identifier which unambiguously
specifies a certain I/O pin.  The allowed forms of the identifier and the
physical pin that the identifier maps to are port-specific.  Possibilities
for the identifier are an integer, a string or a tuple with port and pin
number.

Usage Model::

    from machine import Pin

    # create an output pin on pin #PB16
    pb16 = Pin(Pin.PB_16, Pin.OUT, Pin.PULL_DOWM)

    # set the value low then high
    pb16.value(0)
    pb16.value(1)

    # create an input pin on pin #PB17, with a pull up resistor
    pb17 = Pin(Pin.PB_17, Pin.IN, Pin.PULL_UP)

    # read and print the pin value
    print(pb17.value())

    # configure an irq callback
    pb17.irq(lambda p:print(p))
	

Constructors
------------

.. class:: Pin(id, mode=Pin.OUT, pull=Pin.PULL_FLOATING, \*, value)

   Access the pin peripheral (GPIO pin) associated with the given ``id``.  If
   additional arguments are given in the constructor then they are used to initialise
   the pin.  Any settings that are not specified will remain in their previous state.

   The arguments are:

     - ``id`` is mandatory and can be an arbitrary object.  Among possible value
       types are: int (an internal Pin identifier), str (a Pin name), and tuple
       (pair of [port, pin]).

     - ``mode`` specifies the pin mode, which can be one of:

       - ``Pin.IN`` - Pin is configured for input.  If viewed as an output the pin
         is in high-impedance state.

       - ``Pin.OUT`` - Pin is configured for (normal) output.

     - ``pull`` specifies if the pin has a (weak) pull resistor attached, and can be
       one of:

       - ``Pin.PULL_FLOATING `` - No pull up or down resistor.
       - ``Pin.PULL_UP`` - Pull up resistor enabled.
       - ``Pin.PULL_DOWN`` - Pull down resistor enabled.

     - ``value`` is valid only for Pin.OUT modes and specifies initial
       output pin value if given, otherwise the state of the pin peripheral remains
       unchanged.

   As specified above, the Pin class allows to set an alternate function for a particular
   pin, but it does not specify any further operations on such a pin.  Pins configured in
   alternate-function mode are usually not used as GPIO but are instead driven by other
   hardware peripherals.  The only operation supported on such a pin is re-initialising,
   by calling the constructor or :meth:`Pin.init` method.  If a pin that is configured in
   alternate-function mode is re-initialised with ``Pin.IN``, ``Pin.OUT``,
   the alternate function will be removed from the pin.

Methods
-------

.. method:: Pin.init(mode=Pin.OUT, pull=Pin.PULL_FLOATING)

   Re-initialise the pin using the given parameters.  Only those arguments that
   are specified will be set.  The rest of the pin peripheral state will remain
   unchanged.  See the constructor documentation for details of the arguments.
   
   Returns ``None``.

.. method:: Pin.value([x])

   This method allows to set and get the value of the pin, depending on whether
   the argument ``x`` is supplied or not.

   If the argument is omitted then this method gets the digital logic level of
   the pin, returning 0 or 1 corresponding to low and high voltage signals
   respectively.  The behaviour of this method depends on the mode of the pin:

     - ``Pin.IN`` - The method returns the actual input value currently present
       on the pin.
     - ``Pin.OUT`` - The behaviour and return value of the method is undefined.

   If the argument is supplied then this method sets the digital logic level of
   the pin.  The argument ``x`` can be anything that converts to a boolean.
   If it converts to ``True``, the pin is set to state '1', otherwise it is set
   to state '0'.  The behaviour of this method depends on the mode of the pin:

     - ``Pin.IN`` - The value is stored in the output buffer for the pin.  The
       pin state does not change, it remains in the high-impedance state.  The
       stored value will become active on the pin as soon as it is changed to
       ``Pin.OUT`` mode.
     - ``Pin.OUT`` - The output buffer is set to the given value immediately.

   When setting the value this method returns ``None``.

.. method:: Pin.irq(handler,trigger=(Pin.IRQ_DOUBLE_EDGE | Pin.IRQ_FALLING | Pin.IRQ_RISING | Pin.IRQ_LOW_LEVEL | Pin.IRQ_HIGH_LEVEL))

   Configure an interrupt handler to be called when the trigger source of the
   pin is active.  If the pin mode is ``Pin.IN`` then the trigger source is
   the external value on the pin.  If the pin mode is ``Pin.OUT`` then the
   trigger source is the output buffer of the pin.

   The arguments are:

     - ``trigger`` configures the event which can generate an interrupt.
       Possible values are:

	   - ``Pin.IRQ_DOUBLE_EDGE`` interrupt on falling and rising edge.
       - ``Pin.IRQ_FALLING`` interrupt on falling edge.
       - ``Pin.IRQ_RISING`` interrupt on rising edge.
       - ``Pin.IRQ_LOW_LEVEL`` interrupt on low level.
       - ``Pin.IRQ_HIGH_LEVEL`` interrupt on high level.

   This method returns a callback object.

Constants
---------

The following constants are used to configure the pin objects.  Note that
not all constants are available on all ports.

.. data:: Pin.IN
          Pin.OUT

   Selects the pin mode.

.. data:: Pin.PULL_FLOATING
          Pin.PULL_UP
          Pin.PULL_DOWN

   Selects whether there is a pull up/down resistor.  Use the value
   ``PULL_FLOATING`` for no pull.

.. data:: Pin.IRQ_DOUBLE_EDGE 
		  Pin.IRQ_FALLING
          Pin.IRQ_RISING
          Pin.IRQ_LOW_LEVEL
          Pin.IRQ_HIGH_LEVEL
		  
	Selects the IRQ trigger type.
		  
.. data:: Pin.PA_00 
		  Pin.PA_01
          Pin.PA_02
          Pin.PA_03
          Pin.PA_04
		  Pin.PA_05
		  Pin.PA_06
		  Pin.PA_07
		  Pin.PA_08
		  Pin.PA_09
		  Pin.PA_10
		  Pin.PA_11
		  Pin.PA_12
		  Pin.PA_13
		  Pin.PA_14
		  Pin.PA_15
		  Pin.PB_00
		  Pin.PB_01
		  Pin.PB_03
		  Pin.PB_04
		  Pin.PB_05
		  Pin.PB_06
		  Pin.PB_07
		  Pin.PB_08
		  Pin.PB_09
		  Pin.PB_10
		  Pin.PB_11
		  Pin.PB_12
		  Pin.PB_13
		  Pin.PB_14
		  Pin.PB_15
		  Pin.PB_16
		  Pin.PB_17
		  Pin.PB_18
		  Pin.PB_19
		  Pin.PB_20
		  Pin.PB_21
		  Pin.PB_22
		  Pin.PB_23
		  Pin.PB_24
		  Pin.PB_25
		  Pin.PB_26
		  Pin.PB_27
		  Pin.PB_28
		  Pin.PB_29
		  Pin.PB_30
		  Pin.PB_31
		  
	Selects the Pin number.
	
