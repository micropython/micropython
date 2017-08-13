.. _machine.Pin:

class Pin -- control I/O pins
=============================

A pin is the basic object to control I/O pins.  It has methods to set
the mode of the pin (input, output, etc) and methods to get and set the
digital logic level. For analog control of a pin, see the ADC class.

Usage Model:

.. only:: port_wipy

    Board pins are identified by their string id::

        from machine import Pin
        g = machine.Pin('GP9', mode=Pin.OUT, pull=None, drive=Pin.MED_POWER, alt=-1)

    You can also configure the Pin to generate interrupts. For instance::

        from machine import Pin

        def pincb(pin):
            print(pin.id())

        pin_int = Pin('GP10', mode=Pin.IN, pull=Pin.PULL_DOWN)
        pin_int.irq(trigger=Pin.IRQ_RISING, handler=pincb)
        # the callback can be triggered manually
        pin_int.irq()()
        # to disable the callback
        pin_int.irq().disable()

    Now every time a falling edge is seen on the gpio pin, the callback will be
    executed. Caution: mechanical push buttons have "bounce" and pushing or
    releasing a switch will often generate multiple edges.
    See: http://www.eng.utah.edu/~cs5780/debouncing.pdf for a detailed
    explanation, along with various techniques for debouncing.

    All pin objects go through the pin mapper to come up with one of the
    gpio pins.

.. only:: port_esp8266

   ::

    from machine import Pin

    # create an output pin on GPIO0
    p0 = Pin(0, Pin.OUT)
    p0.value(0)
    p0.value(1)

    # create an input pin on GPIO2
    p2 = Pin(2, Pin.IN, Pin.PULL_UP)
    print(p2.value())

Constructors
------------

.. class:: machine.Pin(id, ...)

   Create a new Pin object associated with the id.  If additional arguments are given,
   they are used to initialise the pin.  See :meth:`pin.init`.

Methods
-------

.. only:: port_wipy

    .. method:: pin.init(mode, pull, \*, drive, alt)
    
       Initialise the pin:

         - ``mode`` can be one of:

            - ``Pin.IN``  - input pin.
            - ``Pin.OUT`` - output pin in push-pull mode.
            - ``Pin.OPEN_DRAIN`` - output pin in open-drain mode.
            - ``Pin.ALT`` - pin mapped to an alternate function.
            - ``Pin.ALT_OPEN_DRAIN`` - pin mapped to an alternate function in open-drain mode.

         - ``pull`` can be one of:

            - ``None`` - no pull up or down resistor.
            - ``Pin.PULL_UP`` - pull up resistor enabled.
            - ``Pin.PULL_DOWN`` - pull down resitor enabled.

         - ``drive`` can be one of:

            - ``Pin.LOW_POWER`` - 2mA drive capability.
            - ``Pin.MED_POWER`` - 4mA drive capability.
            - ``Pin.HIGH_POWER`` - 6mA drive capability.

         - ``alt`` is the number of the alternate function. Please refer to the
           `pinout and alternate functions table. <https://raw.githubusercontent.com/wipy/wipy/master/docs/PinOUT.png>`_
           for the specific alternate functions that each pin supports.

       Returns: ``None``.

    .. method:: pin.id()

       Get the pin id.

.. only:: port_esp8266

    .. method:: pin.init(mode, pull=None, \*, value)

       Initialise the pin:

         - `mode` can be one of:

            - ``Pin.IN``  - input pin.
            - ``Pin.OUT`` - output pin in push-pull mode.

         - `pull` can be one of:

            - ``None`` - no pull up or down resistor.
            - ``Pin.PULL_UP`` - pull up resistor enabled.

         - if `value` is given then it is the output value to set the pin
           if it is in output mode.

.. method:: pin.value([value])

   Get or set the digital logic level of the pin:

     - With no argument, return 0 or 1 depending on the logic level of the pin.
     - With ``value`` given, set the logic level of the pin.  ``value`` can be
       anything that converts to a boolean.  If it converts to ``True``, the pin
       is set high, otherwise it is set low.

.. method:: pin([value])

   Pin objects are callable. The call method provides a (fast) shortcut to set and get the value of the pin.
   See **pin.value** for more details.

.. method:: pin.alt_list()

    Returns a list of the alternate functions supported by the pin. List items are
    a tuple of the form: ``('ALT_FUN_NAME', ALT_FUN_INDEX)``

    Availability: WiPy.

.. only:: port_wipy

    .. method:: pin.toggle()

        Toggle the value of the pin.

    .. method:: pin.mode([mode])

        Get or set the pin mode.

    .. method:: pin.pull([pull])

        Get or set the pin pull.

    .. method:: pin.drive([drive])

        Get or set the pin drive strength.

    .. method:: pin.irq(\*, trigger, priority=1, handler=None, wake=None)

        Create a callback to be triggered when the input level at the pin changes.

            - ``trigger`` configures the pin level which can generate an interrupt. Possible values are:

                - ``Pin.IRQ_FALLING`` interrupt on falling edge.
                - ``Pin.IRQ_RISING`` interrupt on rising edge.
                - ``Pin.IRQ_LOW_LEVEL`` interrupt on low level.
                - ``Pin.IRQ_HIGH_LEVEL`` interrupt on high level.
              
              The values can be *ORed* together, for instance mode=Pin.IRQ_FALLING | Pin.IRQ_RISING

            - ``priority`` level of the interrupt. Can take values in the range 1-7.
              Higher values represent higher priorities.
            - ``handler`` is an optional function to be called when new characters arrive.
            - ``wakes`` selects the power mode in which this interrupt can wake up the
              board. Please note:

              - If ``wake_from=machine.Sleep.ACTIVE`` any pin can wake the board.
              - If ``wake_from=machine.Sleep.SUSPENDED`` pins ``GP2``, ``GP4``, ``GP10``,
                ``GP11``, GP17`` or ``GP24`` can wake the board. Note that only 1
                of this pins can be enabled as a wake source at the same time, so, only
                the last enabled pin as a ``machine.Sleep.SUSPENDED`` wake source will have effect.
              - If ``wake_from=machine.Sleep.SUSPENDED`` pins ``GP2``, ``GP4``, ``GP10``,
                ``GP11``, ``GP17`` and ``GP24`` can wake the board. In this case all of the
                6 pins can be enabled as a ``machine.Sleep.HIBERNATE`` wake source at the same time.
              - Values can be ORed to make a pin generate interrupts in more than one power
                mode.

            Returns a callback object.

.. only:: port_esp8266

    .. method:: pin.irq(\*, trigger, handler=None)

        Create a callback to be triggered when the input level at the pin changes.

            - ``trigger`` configures the pin level which can generate an interrupt. Possible values are:

                - ``Pin.IRQ_FALLING`` interrupt on falling edge.
                - ``Pin.IRQ_RISING`` interrupt on rising edge.

              The values can be OR'ed together to trigger on multiple events.

            - ``handler`` is an optional function to be called when the interrupt triggers.

            Returns a callback object.

Attributes
----------

.. class:: Pin.board

    Contains all ``Pin`` objects supported by the board. Examples::

        Pin.board.GP25
        led = Pin(Pin.board.GP25, mode=Pin.OUT)
        Pin.board.GP2.alt_list()

    Availability: WiPy.

Constants
---------

The following constants are used to configure the pin objects.  Note that
not all constants are available on all ports.

.. data:: IN
          OUT
          OPEN_DRAIN
          ALT
          ALT_OPEN_DRAIN

   Selects the pin mode.

.. data:: PULL_UP
          PULL_DOWN

   Selects the whether there is a pull up/down resistor.

.. data:: LOW_POWER
          MED_POWER
          HIGH_POWER

   Selects the pin drive strength.

.. data:: IRQ_FALLING
          IRQ_RISING
          IRQ_LOW_LEVEL
          IRQ_HIGH_LEVEL

   Selects the IRQ trigger type.
