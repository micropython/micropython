.. _pyb.ADC:

class ADC -- analog to digital conversion: read analog values on a pin
======================================================================

Usage::

    import pyb

    adc = pyb.ADC(pin)              # create an analog object from a pin
    val = adc.read()                # read an analog value

    adc = pyb.ADCAll(resolution)    # creale an ADCAll object
    val = adc.read_channel(channel) # read the given channel
    val = adc.read_core_temp()      # read MCU temperature
    val = adc.read_core_vbat()      # read MCU VBAT
    val = adc.read_core_vref()      # read MCU VREF


Constructors
------------

.. class:: pyb.ADC(pin)

   Create an ADC object associated with the given pin.
   This allows you to then read analog values on that pin.


Methods
-------

.. method:: adc.read()

   Read the value on the analog pin and return it.  The returned value
   will be between 0 and 4095.

.. method:: adc.read_timed(buf, freq)

   Read analog values into the given buffer at the given frequency. Buffer
   can be bytearray or array.array for example. If a buffer with 8-bit elements
   is used, sample resolution will be reduced to 8 bits.
   
   Example::
   
       adc = pyb.ADC(pyb.Pin.board.X19)    # create an ADC on pin X19
       buf = bytearray(100)                # create a buffer of 100 bytes
       adc.read_timed(buf, 10)             # read analog values into buf at 10Hz
                                           #   this will take 10 seconds to finish
       for val in buf:                     # loop over all values
           print(val)                      # print the value out
   
   This function does not allocate any memory.
