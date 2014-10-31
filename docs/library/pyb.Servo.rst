class Servo -- 3-wire hobby servo driver
========================================

Servo controls standard hobby servos with 3-wires (ground, power, signal).


Constructors
------------

.. class:: pyb.Servo(id)

   Create a servo object.  ``id`` is 1-4.


Methods
-------

.. method:: servo.angle([angle, time=0])

   Get or set the angle of the servo.
   
     - ``angle`` is the angle to move to in degrees.
     - ``time`` is the number of milliseconds to take to get to the specified angle.

.. method:: servo.calibration([pulse_min, pulse_max, pulse_centre, [pulse_angle_90, pulse_speed_100]])

   Get or set the calibration of the servo timing.

.. method:: servo.pulse_width([value])

   Get or set the pulse width in milliseconds.

.. method:: servo.speed([speed, time=0])

   Get or set the speed of a continuous rotation servo.
   
     - ``speed`` is the speed to move to change to, between -100 and 100.
     - ``time`` is the number of milliseconds to take to get to the specified speed.
