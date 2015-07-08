class Accel -- accelerometer control
====================================

Accel is an object that controls the accelerometer.  Example usage::

    accel = pyb.Accel()
    for i in range(10):
        print(accel.x(), accel.y(), accel.z())

Raw values are between -32 and 31.


Constructors
------------

.. class:: pyb.Accel()

   Create and return an accelerometer object.
   
Methods
-------

.. method:: accel.filtered_xyz()

   Get a 3-tuple of filtered x, y and z values.

   Implementation note: this method is currently implemented as taking the
   sum of 4 samples, sampled from the 3 previous calls to this function along
   with the sample from the current call.  Returned values are therefore 4
   times the size of what they would be from the raw x(), y() and z() calls.

.. method:: accel.tilt()

   Get the tilt register.

.. method:: accel.x()

   Get the x-axis value.

.. method:: accel.y()

   Get the y-axis value.

.. method:: accel.z()

   Get the z-axis value.
