.. _pyb.Pin:

class Pin -- control I/O pins
=============================

A pin is the basic object to control I/O pins.  It has methods to set
the mode of the pin (input, output, etc) and methods to get and set the
digital logic level. For analog control of a pin, see the ADC class.

Usage Model:

.. only:: port_pyboard

    All Board Pins are predefined as pyb.Pin.board.Name::
    
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
    
    and can query mappings::
    
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
    
    When a pin has the ``Pin.PULL_UP`` or ``Pin.PULL_DOWN`` pull-mode enabled,
    that pin has an effective 40k Ohm resistor pulling it to 3V3 or GND
    respectively (except pin Y5 which has 11k Ohm resistors).

.. only:: port_wipy

    Board pins are identified by their string name::

        g = pyb.Pin('GPIO9', af=0, mode=pyb.Pin.IN, type=pyb.Pin.STD, strength=pyb.Pin.S2MA)

    You can also configure the Pin to generate interrupts. For instance::

        def pincb(pin):
            print(pin.info().name)

        pin_int = pyb.Pin('GPIO10', af=0, mode=Pin.IN, type=pyb.Pin.STD_PD, strength=pyb.Pin.S2MA)
        pin_int.callback (mode=pyb.Pin.INT_RISING, handler=pincb)
        # the callback can be triggered manually
        pin_int.callback()()
        # to disable the callback
        pin_int.callback().disable()

    Now every time a falling edge is seen on the gpio pin, the callback will be
    executed. Caution: mechanical push buttons have "bounce" and pushing or
    releasing a switch will often generate multiple edges.
    See: http://www.eng.utah.edu/~cs5780/debouncing.pdf for a detailed
    explanation, along with various techniques for debouncing.

    All pin objects go through the pin mapper to come up with one of the
    gpio pins.

Constructors
------------

.. only:: port_pyboard

   .. class:: pyb.Pin(id, ...)

      Create a new Pin object associated with the id.  If additional arguments are given,
      they are used to initialise the pin.  See :meth:`pin.init`.

.. only:: port_wipy

   .. class:: pyb.Pin(name, ...)

      Create a new Pin object associated with the name.  If additional arguments are given,
      they are used to initialise the pin.  See :meth:`pin.init`.

.. only:: port_pyboard

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

.. only:: port_pyboard

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

         - when mode is ``Pin.AF_PP`` or ``Pin.AF_OD``, then af can be the index or name
           of one of the alternate functions associated with a pin.
       
       Returns: ``None``.

.. only:: port_wipy

    .. method:: pin.init(af, mode, type, strength)
    
       Initialise the pin:
       
         - ``af`` is the number of the alternate function. Please refer to the
           `pinout and alternate functions table. <https://raw.githubusercontent.com/wipy/wipy/master/docs/PinOUT.png>`_
           for the specific alternate functions that each pin supports.

         - ``mode`` can be one of:

            - ``Pin.OUT`` - no pull up or down resistors.
            - ``Pin.IN``  - enable the pull-up resistor.

         - ``type`` can be one of:

            - ``Pin.STD``    - push-pull pin.
            - ``Pin.STD_PU`` - push-pull pin with pull-up resistor.
            - ``Pin.STD_PD`` - push-pull pin with pull-down resistor.
            - ``Pin.OD``     - open drain pin.
            - ``Pin.OD_PU``  - open drain pin with pull-up resistor.
            - ``Pin.OD_PD``  - open drain pin with pull-down resistor.

         - ``strength`` can be one of:

            - ``Pin.S2MA`` - 2mA drive capability.
            - ``Pin.S4MA`` - 4mA drive capability.
            - ``Pin.S6MA`` - 6mA drive capability.

       Returns: ``None``.

.. method:: pin.high()

   Set the pin to a high logic level.

.. method:: pin.low()

   Set the pin to a low logic level.

.. method:: pin.value([value])

   Get or set the digital logic level of the pin:

     - With no argument, return 0 or 1 depending on the logic level of the pin.
     - With ``value`` given, set the logic level of the pin.  ``value`` can be
       anything that converts to a boolean.  If it converts to ``True``, the pin
       is set high, otherwise it is set low.

.. only:: port_pyboard

    .. method:: pin.__str__()
    
       Return a string describing the pin object.
    
    .. method:: pin.af()
    
       Returns the currently configured alternate-function of the pin. The
       integer returned will match one of the allowed constants for the af
       argument to the init function.
    
    .. method:: pin.gpio()
    
       Returns the base address of the GPIO block associated with this pin.
    
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

.. only:: port_wipy

    .. method:: pin.toggle()

        Toggle the value of the pin.

    .. method:: pin.info()

        Return a 5-tuple with the configuration of the pin:
        ``(name, alternate-function, mode, type, strength)``

    .. method:: pin.callback(mode, priority=1, handler=None, wakes=pyb.Sleep.ACTIVE)

        Create a callback to be triggered when data is received on the UART.

            - ``mode`` configures the pin level which can generate an interrupt. Possible values are:

                - ``Pin.INT_FALLING`` interrupt on falling edge.
                - ``Pin.INT_RISING`` interrupt on rising edge.
                - ``Pin.INT_RISING_FALLING`` interrupt on rising and falling edge.
                - ``Pin.INT_LOW_LEVEL`` interrupt on low level.
                - ``Pin.INT_HIGH_LEVEL`` interrupt on high level.

            - ``priority`` level of the interrupt. Can take values in the range 1-7.
              Higher values represent higher priorities.
            - ``handler`` is an optional function to be called when new characters arrive.
            - ``wakes`` selects the power mode in which this interrupt can wake up the
              board. Please note:

              - If ``wakes=pyb.Sleep.ACTIVE`` any pin can wake the board.
              - If ``wakes=pyb.Sleep.SUSPENDED`` pins ``GPIO2``, ``GPIO4``, ``GPIO10``, 
                ``GPIO11``, GPIO17`` or ``GPIO24`` can wake the board. Note that only 1
                of this pins can be enabled as a wake source at the same time, so, only
                the last enabled pin as a ``pyb.Sleep.SUSPENDED`` wake source will have effect.
              - If ``wakes=pyb.Sleep.SUSPENDED`` pins ``GPIO2``, ``GPIO4``, ``GPIO10``, 
                ``GPIO11``, GPIO17`` and ``GPIO24`` can wake the board. In this case all this 6
                pins can be enabled as a ``pyb.Sleep.HIBERNATE`` wake source at the same time.
              - Values can be ORed to make a pin generate interrupts in more than one power
                mode.

            Returns a callback object.


Constants
---------

.. only:: port_pyboard

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

.. only:: port_wipy

    .. data:: Pin.IN
    
       input pin mode
     
    .. data:: Pin.OUT
    
       output pin mode
    
    .. data:: Pin.STD
    
       push-pull pin type
    
    .. data:: Pin.STD_PU
    
       push-pull pin with internall pull-up resistor
    
    .. data:: Pin.STD_PD
    
       push-pull pin with internall pull-down resistor
    
    .. data:: Pin.OD
    
       open-drain pin
    
    .. data:: Pin.OD_PU
    
       open-drain pin with pull-up resistor
    
    .. data:: Pin.OD_PD
    
       open-drain pin with pull-down resistor
    
    .. data:: Pin.INT_FALLING
    
       interrupt on falling edge
    
    .. data:: Pin.INT_RISING
    
       interrupt on rising edge
    
    .. data:: Pin.INT_RISING_FALLING
    
       interrupt on rising and falling edge
    
    .. data:: Pin.INT_LOW_LEVEL
    
       interrupt on low level
    
    .. data:: Pin.INT_HIGH_LEVEL
    
       interrupt on high level
    
    .. data:: Pin.S2MA
    
       2mA drive strength
    
    .. data:: Pin.S4MA
    
       4mA drive strength
    
    .. data:: Pin.S6MA 
    
       6mA drive strength

.. only:: port_pyboard

    class PinAF -- Pin Alternate Functions
    ======================================
    
    A Pin represents a physical pin on the microcprocessor. Each pin
    can have a variety of functions (GPIO, I2C SDA, etc). Each PinAF
    object represents a particular function for a pin.
    
    Usage Model::
    
        x3 = pyb.Pin.board.X3
        x3_af = x3.af_list()
    
    x3_af will now contain an array of PinAF objects which are availble on
    pin X3.
    
    For the pyboard, x3_af would contain:
        [Pin.AF1_TIM2, Pin.AF2_TIM5, Pin.AF3_TIM9, Pin.AF7_USART2]
    
    Normally, each peripheral would configure the af automatically, but sometimes
    the same function is available on multiple pins, and having more control
    is desired.
    
    To configure X3 to expose TIM2_CH3, you could use::
    
       pin = pyb.Pin(pyb.Pin.board.X3, mode=pyb.Pin.AF_PP, af=pyb.Pin.AF1_TIM2)
    
    or::
    
       pin = pyb.Pin(pyb.Pin.board.X3, mode=pyb.Pin.AF_PP, af=1)

    Methods
    -------
    
    .. method:: pinaf.__str__()
    
       Return a string describing the alternate function.
    
    .. method:: pinaf.index()
    
       Return the alternate function index.
    
    .. method:: pinaf.name()
    
       Return the name of the alternate function.
    
    .. method:: pinaf.reg()
    
       Return the base register associated with the peripheral assigned to this
       alternate function. For example, if the alternate function were TIM2_CH3
       this would return stm.TIM2
