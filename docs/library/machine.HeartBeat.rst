.. _machine.HeartBeat:

class HeartBeat -- heart beat LED
=================================

The HeartBeat class controls the heart beat led which by default
flashes once every 5s. The user can disable the HeartBeat and then
is free to control this LED manually through GP25 using the Pin
class. The GP25 can also be remapped as a PWM output, an this
can be used to control the light intesity of the heart beat LED.

Example usage::

    hb = machine.HeartBeat()
    hb.disable()    # disable the heart beat
    hb.enable()     # enable the heart beat

Constructors
------------

.. class:: machine.HeartBeat()

   Create a HeartBeat object.

Methods
-------

.. method:: heartbeat.enable()

   Enable the heart beat. The LED will flash once every 5 seconds.

.. method:: heartbeat.disable()

   Disable the heart beat. The LED can then be controlled manually.

   Example::
   
      from machine import HeartBeat
      from machine import Pin
   
      # disable the heart beat
      HeartBeat().disable()
      # init GP25 as output
      led = Pin('GP25', mode=Pin.OUT)
      # toggle the led
      led.toggle()
      ...
