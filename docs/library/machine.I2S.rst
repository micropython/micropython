.. currentmodule:: machine
.. _machine.I2S:

class I2S -- Inter-IC Sound bus protocol
========================================

I2S is a synchronous serial protocol used to connect digital audio devices.
At the physical level, a bus consists of 3 lines: SCK, WS, SD.
The I2S class supports controller operation.  Peripheral operation is not supported.

The I2S class is currently available as a Technical Preview.  During the preview period, feedback from
users is encouraged.  Based on this feedback, the I2S class API and implementation may be changed.

I2S objects can be created and initialized using::

    from machine import I2S
    from machine import Pin

    # ESP32
    sck_pin = Pin(14)   # Serial clock output
    ws_pin = Pin(13)    # Word clock output
    sd_pin = Pin(12)    # Serial data output

    or

    # PyBoards
    sck_pin = Pin("Y6")   # Serial clock output
    ws_pin = Pin("Y5")    # Word clock output
    sd_pin = Pin("Y8")    # Serial data output

    audio_out = I2S(2,
                    sck=sck_pin, ws=ws_pin, sd=sd_pin,
                    mode=I2S.TX,
                    bits=16,
                    format=I2S.MONO,
                    rate=44100,
                    ibuf=20000)

    audio_in = I2S(2,
                   sck=sck_pin, ws=ws_pin, sd=sd_pin,
                   mode=I2S.RX,
                   bits=32,
                   format=I2S.STEREO,
                   rate=22050,
                   ibuf=20000)

3 modes of operation are supported:
 - blocking
 - non-blocking
 - uasyncio

blocking::

   num_written = audio_out.write(buf) # blocks until buf emptied

   num_read = audio_in.readinto(buf) # blocks until buf filled

non-blocking::

   audio_out.irq(i2s_callback)         # i2s_callback is called when buf is emptied
   num_written = audio_out.write(buf)  # returns immediately

   audio_in.irq(i2s_callback)          # i2s_callback is called when buf is filled
   num_read = audio_in.readinto(buf)   # returns immediately

uasyncio::

   swriter = uasyncio.StreamWriter(audio_out)
   swriter.write(buf)
   await swriter.drain()

   sreader = uasyncio.StreamReader(audio_in)
   num_read = await sreader.readinto(buf)

Some codec devices like the WM8960 or SGTL5000 require separate initialization
before they can operate with the I2S class.  For these, separate drivers are
supplied, which also offer methods for controlling volume, audio processing and
other things.  For these drivers see:

- :ref:`wm8960`

Constructor
-----------

.. class:: I2S(id, *, sck, ws, sd, mck=None, mode, bits, format, rate, ibuf)

   Construct an I2S object of the given id:

   - ``id`` identifies a particular I2S bus; it is board and port specific

   Keyword-only parameters that are supported on all ports:

     - ``sck`` is a pin object for the serial clock line
     - ``ws`` is a pin object for the word select line
     - ``sd`` is a pin object for the serial data line
     - ``mck`` is a pin object for the master clock line;
       master clock frequency is sampling rate * 256
     - ``mode`` specifies receive or transmit
     - ``bits`` specifies sample size (bits), 16 or 32
     - ``format`` specifies channel format, STEREO or MONO
     - ``rate`` specifies audio sampling rate (Hz);
       this is the frequency of the ``ws`` signal
     - ``ibuf`` specifies internal buffer length (bytes)

   For all ports, DMA runs continuously in the background and allows user applications to perform other operations while
   sample data is transferred between the internal buffer and the I2S peripheral unit.
   Increasing the size of the internal buffer has the potential to increase the time that user applications can perform non-I2S operations
   before underflow (e.g. ``write`` method) or overflow (e.g. ``readinto`` method).

Methods
-------

.. method:: I2S.init(sck, ...)

  see Constructor for argument descriptions

.. method:: I2S.deinit()

  Deinitialize the I2S bus

.. method::  I2S.readinto(buf)

  Read audio samples into the buffer specified by ``buf``.  ``buf`` must support the buffer protocol, such as bytearray or array.
  "buf" byte ordering is little-endian.  For Stereo format, left channel sample precedes right channel sample. For Mono format,
  the left channel sample data is used.
  Returns number of bytes read

.. method::  I2S.write(buf)

  Write audio samples contained in ``buf``. ``buf`` must support the buffer protocol, such as bytearray or array.
  "buf" byte ordering is little-endian.  For Stereo format, left channel sample precedes right channel sample. For Mono format,
  the sample data is written to both the right and left channels.
  Returns number of bytes written

.. method::  I2S.irq(handler)

  Set a callback. ``handler`` is called when ``buf`` is emptied (``write`` method) or becomes full (``readinto`` method).
  Setting a callback changes the ``write`` and ``readinto`` methods to non-blocking operation.
  ``handler`` is called in the context of the MicroPython scheduler.

.. staticmethod::  I2S.shift(*, buf, bits, shift)

  bitwise shift of all samples contained in ``buf``. ``bits`` specifies sample size in bits. ``shift`` specifies the number of bits to shift each sample.
  Positive for left shift, negative for right shift.
  Typically used for volume control.  Each bit shift changes sample volume by 6dB.

Constants
---------

.. data:: I2S.RX

   for initialising the I2S bus ``mode`` to receive

.. data:: I2S.TX

   for initialising the I2S bus ``mode`` to transmit

.. data:: I2S.STEREO

   for initialising the I2S bus ``format`` to stereo

.. data:: I2S.MONO

   for initialising the I2S bus ``format`` to mono


