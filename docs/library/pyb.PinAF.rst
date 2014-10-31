class PinAF --- Pin Alternate Functions
=======================================

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
