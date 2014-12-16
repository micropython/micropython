.. _pyb.LED:

class LED -- LED object
=======================

The LED object controls an individual LED (Light Emitting Diode).


Constructors
------------

.. class:: pyb.LED(id)

   Create an LED object associated with the given LED:
   
     - ``id`` is the LED number, 1-4.


Methods
-------

.. method:: led.intensity([value])

   Get or set the LED intensity.  Intensity ranges between 0 (off) and 255 (full on).
   If no argument is given, return the LED intensity.
   If an argument is given, set the LED intensity and return ``None``.

.. method:: led.off()

   Turn the LED off.

.. method:: led.on()

   Turn the LED on, to maximum intensity.

.. method:: led.toggle()

   Toggle the LED between on (maximum intensity) and off.  If the LED is at
   non-zero intensity then it is considered "on" and toggle will turn it off.
