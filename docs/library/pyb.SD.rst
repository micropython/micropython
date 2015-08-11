.. _pyb.SD:

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

    # data, clk and cmd pins must be passed along with
    # their respective alternate functions
    sd = pyb.SD(('GP15', 8, 'GP10', 6, 'GP11', 6))
    sd.mount()
    # do normal file operations

Constructors
------------

.. class:: pyb.SD([pins_tuple])

   Create a SD card object. In order to initalize the card, give it a 6-tuple
   ``(dat_pin, dat_af, clk_pin, clk_af, cmd_pin, cmd_af)`` with the data, clock
   and cmd pins together their respective alternate functions.

Methods
-------

.. method:: sd.init([pins_tuple])

   Enable the SD card.

.. method:: sd.deinit()

   Disable the SD card (also unmounts it to avoid file system crashes).

.. method:: sd.mount()

   Mount the SD card on the file system. Accesible as ``/sd``.

.. method:: sd.unmount()

   Unmount the SD card from the file system.
