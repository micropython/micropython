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
   
   Note: if you read accelerometer values immediately after creating this object
   you will get 0.  It takes around 20ms for the first sample to be ready, so,
   unless you have some other code between creating this object and reading its
   values, you should put a ``pyb.delay(20)`` after creating it.  For example::
   
       accel = pyb.Accel()
       pyb.delay(20)
       print(accel.x())


Methods
-------

.. method:: accel.filtered_xyz()

   Get a 3-tuple of filtered x, y and z values.

.. method:: accel.tilt()

   Get the tilt register.

.. method:: accel.x()

   Get the x-axis value.

.. method:: accel.y()

   Get the y-axis value.

.. method:: accel.z()

   Get the z-axis value.
