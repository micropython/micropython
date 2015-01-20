.. _pyb.DAC:

class DAC -- digital to analog conversion
=========================================

The DAC is used to output analog values (a specific voltage) on pin X5 or pin X6.
The voltage will be between 0 and 3.3V.

*This module will undergo changes to the API.*

Example usage::

    from pyb import DAC

    dac = DAC(1)            # create DAC 1 on pin X5
    dac.write(128)          # write a value to the DAC (makes X5 1.65V)

To output a continuous sine-wave::

    import math
    from pyb import DAC

    # create a buffer containing a sine-wave
    buf = bytearray(100)
    for i in range(len(buf)):
        buf[i] = 128 + int(127 \* math.sin(2 \* math.pi \* i / len(buf)))

    # output the sine-wave at 400Hz
    dac = DAC(1)
    dac.write_timed(buf, 400 \* len(buf), mode=DAC.CIRCULAR)


Constructors
------------

.. class:: pyb.DAC(port)

   Construct a new DAC object.
   
   ``port`` can be a pin object, or an integer (1 or 2).
   DAC(1) is on pin X5 and DAC(2) is on pin X6.


Methods
-------

.. method:: dac.noise(freq)

   Generate a pseudo-random noise signal.  A new random sample is written
   to the DAC output at the given frequency.

.. method:: dac.triangle(freq)

   Generate a triangle wave.  The value on the DAC output changes at
   the given frequency, and the frequence of the repeating triangle wave
   itself is 2048 times smaller.

.. method:: dac.write(value)

   Direct access to the DAC output (8 bit only at the moment).

.. method:: dac.write_timed(data, freq, \*, mode=DAC.NORMAL)

   Initiates a burst of RAM to DAC using a DMA transfer.
   The input data is treated as an array of bytes (8 bit data).
   
   ``mode`` can be ``DAC.NORMAL`` or ``DAC.CIRCULAR``.
   
   TIM6 is used to control the frequency of the transfer.
