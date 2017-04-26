.. currentmodule:: pyb
.. _pyb.ADC:

class ADC -- analog to digital conversion
=========================================

.. only:: port_pyboard

    Usage::

        import pyb
    
        adc = pyb.ADC(pin)              # create an analog object from a pin
        val = adc.read()                # read an analog value
    
        adc = pyb.ADCAll(resolution)    # create an ADCAll object
        val = adc.read_channel(channel) # read the given channel
        val = adc.read_core_temp()      # read MCU temperature
        val = adc.read_core_vbat()      # read MCU VBAT
        val = adc.read_core_vref()      # read MCU VREF

 
Constructors
------------


.. only:: port_pyboard

    .. class:: pyb.ADC(pin)

       Create an ADC object associated with the given pin.
       This allows you to then read analog values on that pin.

Methods
-------

.. only:: port_pyboard

    .. method:: ADC.read()

       Read the value on the analog pin and return it.  The returned value
       will be between 0 and 4095.

    .. method:: ADC.read_timed(buf, timer)
    
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

The ADCAll Object
-----------------

.. only:: port_pyboard

    Instantiating this changes all ADC pins to analog inputs. The raw MCU temperature,
    VREF and VBAT data can be accessed on ADC channels 16, 17 and 18 respectively.
    Appropriate scaling will need to be applied. The temperature sensor on the chip
    has poor absolute accuracy and is suitable only for detecting temperature changes.

    The ``ADCAll`` ``read_core_vbat()`` and ``read_core_vref()`` methods read
    the backup battery voltage and the (1.21V nominal) reference voltage using the
    3.3V supply as a reference. Assuming the ``ADCAll`` object has been Instantiated with
    ``adc = pyb.ADCAll(12)`` the 3.3V supply voltage may be calculated:
    
    ``v33 = 3.3 * 1.21 / adc.read_core_vref()``

    If the 3.3V supply is correct the value of ``adc.read_core_vbat()`` will be
    valid. If the supply voltage can drop below 3.3V, for example in in battery
    powered systems with a discharging battery, the regulator will fail to preserve
    the 3.3V supply resulting in an incorrect reading. To produce a value which will
    remain valid under these circumstances use the following:

    ``vback = adc.read_core_vbat() * 1.21 / adc.read_core_vref()``

    It is possible to access these values without incurring the side effects of ``ADCAll``::
    
        def adcread(chan):                              # 16 temp 17 vbat 18 vref
            assert chan >= 16 and chan <= 18, 'Invalid ADC channel'
            start = pyb.millis()
            timeout = 100
            stm.mem32[stm.RCC + stm.RCC_APB2ENR] |= 0x100 # enable ADC1 clock.0x4100
            stm.mem32[stm.ADC1 + stm.ADC_CR2] = 1       # Turn on ADC
            stm.mem32[stm.ADC1 + stm.ADC_CR1] = 0       # 12 bit
            if chan == 17:
                stm.mem32[stm.ADC1 + stm.ADC_SMPR1] = 0x200000 # 15 cycles
                stm.mem32[stm.ADC + 4] = 1 << 23
            elif chan == 18:
                stm.mem32[stm.ADC1 + stm.ADC_SMPR1] = 0x1000000
                stm.mem32[stm.ADC + 4] = 0xc00000
            else:
                stm.mem32[stm.ADC1 + stm.ADC_SMPR1] = 0x40000
                stm.mem32[stm.ADC + 4] = 1 << 23
            stm.mem32[stm.ADC1 + stm.ADC_SQR3] = chan
            stm.mem32[stm.ADC1 + stm.ADC_CR2] = 1 | (1 << 30) | (1 << 10) # start conversion
            while not stm.mem32[stm.ADC1 + stm.ADC_SR] & 2: # wait for EOC
                if pyb.elapsed_millis(start) > timeout:
                    raise OSError('ADC timout')
            data = stm.mem32[stm.ADC1 + stm.ADC_DR]     # clear down EOC
            stm.mem32[stm.ADC1 + stm.ADC_CR2] = 0       # Turn off ADC
            return data

        def v33():
            return 4096 * 1.21 / adcread(17)

        def vbat():
            return  1.21 * 2 * adcread(18) / adcread(17)  # 2:1 divider on Vbat channel

        def vref():
            return 3.3 * adcread(17) / 4096

        def temperature():
            return 25 + 400 * (3.3 * adcread(16) / 4096 - 0.76)

    