.. currentmodule:: pyb
.. _pyb.ADC:

class ADC -- analog to digital conversion
=========================================

.. only:: port_pyboard

    Usage::

        import pyb
    
        adc = pyb.ADC(pin)                  # create an analog object from a pin
        val = adc.read()                    # read an analog value
    
        adc = pyb.ADCAll(resolution)        # create an ADCAll object
        adc = pyb.ADCAll(resolution, mask)  # create an ADCAll object for selected analog channels
        val = adc.read_channel(channel)     # read the given channel
        val = adc.read_core_temp()          # read MCU temperature
        val = adc.read_core_vbat()          # read MCU VBAT
        val = adc.read_core_vref()          # read MCU VREF
        val = adc.read_vref()               # read MCU supply voltage

 
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

       This function does not allocate any heap memory. It has blocking behaviour:
       it does not return to the calling program until the buffer is full.

    .. method:: ADC.read_timed_multi((adcx, adcy, ...), (bufx, bufy, ...), timer)

       This is a static method. It can be used to extract relative timing or
       phase data from multiple ADC's.

       It reads analog values from multiple ADC's into buffers at a rate set by
       the *timer* object. Each time the timer triggers a sample is rapidly
       read from each ADC in turn.

       ADC and buffer instances are passed in tuples with each ADC having an
       associated buffer. All buffers must be of the same type and length and
       the number of buffers must equal the number of ADC's.

       Buffers can be ``bytearray`` or ``array.array`` for example. The ADC values
       have 12-bit resolution and are stored directly into the buffer if its element
       size is 16 bits or greater.  If buffers have only 8-bit elements (eg a
       ``bytearray``) then the sample resolution will be reduced to 8 bits.

       *timer* must be a Timer object. The timer must already be initialised
       and running at the desired sampling frequency.

       Example reading 3 ADC's::

           adc0 = pyb.ADC(pyb.Pin.board.X1)    # Create ADC's
           adc1 = pyb.ADC(pyb.Pin.board.X2)
           adc2 = pyb.ADC(pyb.Pin.board.X3)
           tim = pyb.Timer(8, freq=100)        # Create timer
           rx0 = array.array('H', (0 for i in range(100))) # ADC buffers of
           rx1 = array.array('H', (0 for i in range(100))) # 100 16-bit words
           rx2 = array.array('H', (0 for i in range(100)))
           # read analog values into buffers at 100Hz (takes one second)
           pyb.ADC.read_timed_multi((adc0, adc1, adc2), (rx0, rx1, rx2), tim)
           for n in range(len(rx0)):
               print(rx0[n], rx1[n], rx2[n])

       This function does not allocate any heap memory. It has blocking behaviour:
       it does not return to the calling program until the buffers are full.

       The function returns ``True`` if all samples were acquired with correct
       timing. At high sample rates the time taken to acquire a set of samples
       can exceed the timer period. In this case the function returns ``False``,
       indicating a loss of precision in the sample interval. In extreme cases
       samples may be missed.

       The maximum rate depends on factors including the data width and the
       number of ADC's being read. In testing two ADC's were sampled at a timer
       rate of 210kHz without overrun. Samples were missed at 215kHz.  For three
       ADC's the limit is around 140kHz, and for four it is around 110kHz.
       At high sample rates disabling interrupts for the duration can reduce the
       risk of sporadic data loss.

The ADCAll Object
-----------------

.. only:: port_pyboard

    Instantiating this changes all masked ADC pins to analog inputs. The preprocessed MCU temperature,
    VREF and VBAT data can be accessed on ADC channels 16, 17 and 18 respectively.
    Appropriate scaling is handled according to reference voltage used (usually 3.3V).
    The temperature sensor on the chip is factory calibrated and allows to read the die temperature
    to +/- 1 degree centigrade. Although this sounds pretty accurate, don't forget that the MCU's internal
    temperature is measured. Depending on processing loads and I/O subsystems active the die temperature
    may easily be tens of degrees above ambient temperature. On the other hand a pyboard woken up after a
    long standby period will show correct ambient temperature within limits mentioned above.

    The ``ADCAll`` ``read_core_vbat()``, ``read_vref()`` and ``read_core_vref()`` methods read
    the backup battery voltage, reference voltage and the (1.21V nominal) reference voltage using the
    actual supply as a reference. All results are floating point numbers giving direct voltage values.

    ``read_core_vbat()`` returns the voltage of the backup battery. This voltage is also adjusted according
    to the actual supply voltage. To avoid analog input overload the battery voltage is measured
    via a voltage divider and scaled according to the divider value. To prevent excessive loads
    to the backup battery, the voltage divider is only active during ADC conversion.

    ``read_vref()`` is evaluated by measuring the internal voltage reference and backscale it using
    factory calibration value of the internal voltage reference. In most cases the reading would be close
    to 3.3V. If the pyboard is operated from a battery, the supply voltage may drop to values below 3.3V.
    The pyboard will still operate fine as long as the operating conditions are met. With proper settings
    of MCU clock, flash access speed and programming mode it is possible to run the pyboard down to
    2 V and still get useful ADC conversion.

    It is very important to make sure analog input voltages never exceed actual supply voltage.

    Other analog input channels (0..15) will return unscaled integer values according to the selected
    precision.

    To avoid unwanted activation of analog inputs (channel 0..15) a second prarmeter can be specified.
    This parameter is a binary pattern where each requested analog input has the corresponding bit set.
    The default value is 0xffffffff which means all analog inputs are active. If just the internal
    channels (16..18) are required, the mask value should be 0x70000.
 
    It is possible to access channle 16..18 values without incurring the side effects of ``ADCAll``::
    
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

    Note that this example is only valid for the F405 MCU and all values are not corrected by Vref and
    factory calibration data.
