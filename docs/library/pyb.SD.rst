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
    sd = pyb.SD('GPIO15', 8, 'GPIO16', 8, 'GPIO17', 8)
    sd.enable()      # enable and mount the SD card
    sd.disable()     # disable and unmount it

Constructors
------------

.. class:: pyb.SD(dat_pin, dat_pin_af, clk_pin, clk_pin_af, cmd_pin, cmd_pin_af)

   Create a SD card object. Data, clock and cmd pins must be passed along with 
   their respective alternate functions.

Methods
-------

.. method:: sd.enable()

   Enable the SD card and mount it on the file system. Accesible as ``/sd``.

.. method:: sd.disable()

   Disable the SD card and remove it from the file system.
