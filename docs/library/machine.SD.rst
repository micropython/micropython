.. currentmodule:: machine
.. _machine.SD:

class SD -- secure digital memory card
======================================

The SD card class allows to configure and enable the memory card
module of the WiPy and automatically mount it as ``/sd`` as part
of the file system. There are several pin combinations that can be
used to wire the SD card socket to the WiPy and the pins used can
be specified in the constructor. Please check the `pinout and alternate functions
table. <https://raw.githubusercontent.com/wipy/wipy/master/docs/PinOUT.png>`_ for
more info regarding the pins which can be remapped to be used with a SD card.

Example usage::

    from machine import SD
    import os
    # clk cmd and dat0 pins must be passed along with
    # their respective alternate functions
    sd = machine.SD(pins=('GP10', 'GP11', 'GP15'))
    os.mount(sd, '/sd')
    # do normal file operations

Constructors
------------

.. class:: SD(id,... )

   Create a SD card object. See ``init()`` for parameters if initialization. 

Methods
-------

.. method:: SD.init(id=0, pins=('GP10', 'GP11', 'GP15'))

   Enable the SD card. In order to initialize the card, give it a 3-tuple:
   ``(clk_pin, cmd_pin, dat0_pin)``.

.. method:: SD.deinit()

   Disable the SD card.
