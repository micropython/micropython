.. currentmodule:: pyb
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

    dac = DAC(1, bits=12)   # use 12 bit resolution
    dac.write(4095)         # output maximum value, 3.3V

To output a continuous sine-wave::

    import math
    from pyb import DAC

    # create a buffer containing a sine-wave
    buf = bytearray(100)
    for i in range(len(buf)):
        buf[i] = 128 + int(127 * math.sin(2 * math.pi * i / len(buf)))

    # output the sine-wave at 400Hz
    dac = DAC(1)
    dac.write_timed(buf, 400 * len(buf), mode=DAC.CIRCULAR)

To output a continuous sine-wave at 12-bit resolution::

    import math
    from array import array
    from pyb import DAC

    # create a buffer containing a sine-wave, using half-word samples
    buf = array('H', 2048 + int(2047 * math.sin(2 * math.pi * i / 128)) for i in range(128))

    # output the sine-wave at 400Hz
    dac = DAC(1, bits=12)
    dac.write_timed(buf, 400 * len(buf), mode=DAC.CIRCULAR)

Constructors
------------

.. class:: pyb.DAC(port, bits=8, *, buffering=None)

   Construct a new DAC object.

   ``port`` can be a pin object, or an integer (1 or 2).
   DAC(1) is on pin X5 and DAC(2) is on pin X6.

   ``bits`` is an integer specifying the resolution, and can be 8 or 12.
   The maximum value for the write and write_timed methods will be
   2\*\*``bits``-1.

   The *buffering* parameter selects the behaviour of the DAC op-amp output
   buffer, whose purpose is to reduce the output impedance.  It can be
   ``None`` to select the default (buffering enabled for :meth:`DAC.noise`,
   :meth:`DAC.triangle` and :meth:`DAC.write_timed`, and disabled for
   :meth:`DAC.write`), ``False`` to disable buffering completely, or ``True``
   to enable output buffering.

   When buffering is enabled the DAC pin can drive loads down to 5KΩ.
   Otherwise it has an output impedance of 15KΩ maximum: consequently
   to achieve a 1% accuracy without buffering requires the applied load
   to be less than 1.5MΩ.  Using the buffer incurs a penalty in accuracy,
   especially near the extremes of range.

Methods
-------

.. method:: DAC.init(bits=8, *, buffering=None)

   Reinitialise the DAC.  *bits* can be 8 or 12.  *buffering* can be
   ``None``, ``False`` or ``True``; see above constructor for the meaning
   of this parameter.

.. method:: DAC.deinit()

   De-initialise the DAC making its pin available for other uses.

.. method:: DAC.noise(freq)

   Generate a pseudo-random noise signal.  A new random sample is written
   to the DAC output at the given frequency.

.. method:: DAC.triangle(freq)

   Generate a triangle wave.  The value on the DAC output changes at the given
   frequency and ramps through the full 12-bit range (up and down). Therefore
   the frequency of the repeating triangle wave itself is 8192 times smaller.

.. method:: DAC.write(value)

   Direct access to the DAC output.  The minimum value is 0.  The maximum
   value is 2\*\*``bits``-1, where ``bits`` is set when creating the DAC
   object or by using the ``init`` method.

.. method:: DAC.write_timed(data, freq, *, mode=DAC.NORMAL)

   Initiates a burst of RAM to DAC using a DMA transfer.
   The input data is treated as an array of bytes in 8-bit mode, and
   an array of unsigned half-words (array typecode 'H') in 12-bit mode.

   ``freq`` can be an integer specifying the frequency to write the DAC
   samples at, using Timer(6).  Or it can be an already-initialised
   Timer object which is used to trigger the DAC sample.  Valid timers
   are 2, 4, 5, 6, 7 and 8.

   ``mode`` can be ``DAC.NORMAL`` or ``DAC.CIRCULAR``.

   Example using both DACs at the same time::

     dac1 = DAC(1)
     dac2 = DAC(2)
     dac1.write_timed(buf1, pyb.Timer(6, freq=100), mode=DAC.CIRCULAR)
     dac2.write_timed(buf2, pyb.Timer(7, freq=200), mode=DAC.CIRCULAR)

Constants
---------

.. data:: DAC.NORMAL

   NORMAL mode does a single transmission of the waveform in the data buffer,

.. data:: DAC.CIRCULAR

   CIRCULAR mode does a transmission of the waveform in the data buffer, and wraps around
   to the start of the data buffer every time it reaches the end of the table.
