.. _pyb.ADC:

class ADC -- analog to digital conversion
=========================================

.. only:: port_pyboard

    Usage::

        import pyb
    
        adc = pyb.ADC(pin)              # create an analog object from a pin
        val = adc.read()                # read an analog value
    
        adc = pyb.ADCAll(resolution)    # creale an ADCAll object
        val = adc.read_channel(channel) # read the given channel
        val = adc.read_core_temp()      # read MCU temperature
        val = adc.read_core_vbat()      # read MCU VBAT
        val = adc.read_core_vref()      # read MCU VREF

.. only:: port_wipy

    Usage::
    
       import pyb

       adc = pyb.ADC()                 # create an ADC object
       apin = adc.channel(pin='GP3')   # create an analog pin on GP3
       val = apin()                    # read an analog value

Constructors
------------


.. only:: port_pyboard

    .. class:: pyb.ADC(pin)

       Create an ADC object associated with the given pin.
       This allows you to then read analog values on that pin.

.. only:: port_wipy

    .. class:: pyb.ADC(id=0, \*, bits=12)

       Create an ADC object associated with the given pin.
       This allows you to then read analog values on that pin.
       For more info check the `pinout and alternate functions
       table. <https://raw.githubusercontent.com/wipy/wipy/master/docs/PinOUT.png>`_ 

       .. warning:: 

          ADC pin input range is 0-1.4V (being 1.8V the absolute maximum that it 
          can withstand). When GP2, GP3, GP4 or GP5 are remapped to the 
          ADC block, 1.8 V is the maximum. If these pins are used in digital mode, 
          then the maximum allowed input is 3.6V.

Methods
-------

.. only:: port_pyboard

    .. method:: adc.read()

       Read the value on the analog pin and return it.  The returned value
       will be between 0 and 4095.

    .. method:: adc.read_timed(buf, timer)
    
       Read analog values into ``buf`` at a rate set by the ``timer`` object.

       ``buf`` can be bytearray or array.array for example.  The ADC values have
       12-bit resolution and are stored directly into ``buf`` if its element size is
       16 bits or greater.  If ``buf`` has only 8-bit elements (eg a bytearray) then
       the sample resolution will be reduced to 8 bits.

       ``timer`` should be a Timer object, and a sample is read each time the timer
       triggers.  The timer must already be initialised and running at the desired
       sampling frequency.

       To support previous behaviour of this function, ``timer`` can also be an
       integer which specifies the frequency (in Hz) to sample at.  In this case
       Timer(6) will be automatically configured to run at the given frequency.

       Example using a Timer object (preferred way)::

           adc = pyb.ADC(pyb.Pin.board.X19)    # create an ADC on pin X19
           tim = pyb.Timer(6, freq=10)         # create a timer running at 10Hz
           buf = bytearray(100)                # creat a buffer to store the samples
           adc.read_timed(buf, tim)            # sample 100 values, taking 10s

       Example using an integer for the frequency::

           adc = pyb.ADC(pyb.Pin.board.X19)    # create an ADC on pin X19
           buf = bytearray(100)                # create a buffer of 100 bytes
           adc.read_timed(buf, 10)             # read analog values into buf at 10Hz
                                               #   this will take 10 seconds to finish
           for val in buf:                     # loop over all values
               print(val)                      # print the value out

       This function does not allocate any memory.

.. only:: port_wipy

   .. method:: adc.channel(id, *, pin)

      Create an analog pin. If only channel ID is given, the correct pin will be selected. Alternatively,
      only the pin can be passed and the correct channel will be selected. Examples::

        # all of these are equivalent and enable ADC channel 1 on GP3
        apin = adc.channel(1)
        apin = adc.channel(pin='GP3')
        apin = adc.channel(id=1, pin='GP3')

   .. method:: adc.init()

      Enable the ADC block.

   .. method:: adc.deinit()

      Disable the ADC block.

.. only:: port_wipy

    class ADCChannel --- read analog values from internal or external sources
    =========================================================================

    .. only:: port_wipy

        ADC channels can be connected to internal points of the MCU or to GPIO pins.
        ADC channels are created using the ADC.channel method.

       .. method:: adcchannel()

          Fast method to read the channel value.

       .. method:: adcchannel.value()

          Read the channel value.

       .. method:: adcchannel.init()

          Re-init (and effectively enable) the ADC channel.

       .. method:: adcchannel.deinit()

          Disable the ADC channel.
