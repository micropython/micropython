The AMP audio skin
==================

Soldering and using the AMP audio skin.

.. image:: img/skin_amp_1.jpg
    :alt: AMP skin
    :width: 250px

.. image:: img/skin_amp_2.jpg
    :alt: AMP skin
    :width: 250px

The following video shows how to solder the headers, microphone and speaker onto the AMP skin.

.. raw:: html

    <iframe style="margin-left:3em;" width="560" height="315" src="http://www.youtube.com/embed/fjB1DuZRveo?rel=0" frameborder="0" allowfullscreen></iframe>

For circuit schematics and datasheets for the components on the skin see :ref:`hardware_index`.

Example code
------------

The AMP skin has a speaker which is connected to ``DAC(1)`` via a small
power amplifier.  The volume of the amplifier is controlled by a digital
potentiometer, which is an I2C device with address 46 on the ``IC2(1)`` bus.

To set the volume, define the following function::

    import pyb
    def volume(val):
        pyb.I2C(1, pyb.I2C.CONTROLLER).mem_write(val, 46, 0)

Then you can do::

    >>> volume(0)   # minimum volume
    >>> volume(127) # maximum volume

To play a sound, use the ``write_timed`` method of the ``DAC`` object.
For example::

    import math
    from pyb import DAC

    # create a buffer containing a sine-wave
    buf = bytearray(100)
    for i in range(len(buf)):
        buf[i] = 128 + int(127 * math.sin(2 * math.pi * i / len(buf)))

    # output the sine-wave at 400Hz
    dac = DAC(1)
    dac.write_timed(buf, 400 * len(buf), mode=DAC.CIRCULAR)

You can also play WAV files using the Python ``wave`` module.  You can get
the wave module `here <http://micropython.org/resources/examples/wave.py>`__ and you will also need
the chunk module available `here <http://micropython.org/resources/examples/chunk.py>`__.  Put these
on your pyboard (either on the flash or the SD card in the top-level directory).  You will need an
8-bit WAV file to play, such as `this one <http://micropython.org/resources/examples/test.wav>`_,
or to convert any file you have with the command::

    avconv -i original.wav -ar 22050 -codec pcm_u8 test.wav

Then you can do::

    >>> import wave
    >>> from pyb import DAC
    >>> dac = DAC(1)
    >>> f = wave.open('test.wav')
    >>> dac.write_timed(f.readframes(f.getnframes()), f.getframerate())

This should play the WAV file. Note that this will read the whole file into RAM
so it has to be small enough to fit in it.

To play larger wave files you will have to use the micro-SD card to store it.
Also the file must be read and sent to the DAC in small chunks that will fit
the RAM limit of the microcontroller.  Here is an example function that can
play 8-bit wave files with up to 16kHz sampling::

    import wave
    from pyb import DAC
    from pyb import delay
    dac = DAC(1)

    def play(filename):
        f = wave.open(filename, 'r')
        total_frames = f.getnframes()
        framerate = f.getframerate()

        for position in range(0, total_frames, framerate):
            f.setpos(position)
            dac.write_timed(f.readframes(framerate), framerate)
            delay(1000)

This function reads one second worth of data and sends it to DAC.  It then waits
one second and moves the file cursor to the new position to read the next second
of data in the next iteration of the for-loop.  It plays one second of audio at
a time every one second.
