:mod:`mpu6050` --- functionality specific to the nRF52832 Bluefruit52
====================================================

.. module:: mpu6050
    :synopsis: functionality specific to the nRF52832 Bluefruit52

The ``mpu6050`` module contains functions and classes specifically aimed at
controlling nRF52832 Bluefruit52 modules.


Functions
---------

.. function:: MPU6050()

    New a object MPU6050.

.. function:: init()

    mpu6050 init registers.

.. function:: DMPinit()

    mpu6050 init dmp mode.

.. function:: getTemp()

    Return the soc temperature register value(°C). 

.. function:: getAccel()

    Return the Accel X,Y,Z register value. 

.. function:: getAccel_x()

    Return the Accel X-axis angle value(°). 

.. function:: getAccel_y()

    Return the Accel Y-axis angle value(°). 

.. function:: getAccel_z()

    Return the Accel Z-axis angle value(°). 

.. function:: getGyros()

    Return the Gyros X,Y,Z register value. 

.. function:: getGyro_x()

    Return the Gyro X-axis angular velocity value(rad/s).  

.. function:: getGyro_Y()

    Return the Gyro Y-axis angular velocity value(rad/s).

.. function:: getGyro_Z()

    Return the Gyro Z-axis angular velocity value(rad/s).

.. function:: getDMPdata()

    Return the MPU6050 DMP data (pitch,roll,yaw).

.. function:: setAccelOffset(x_offset,y_offset,z_offset)

    First place the MP6050 module horizontally and use the function 'getAccel()' to read the static register values. and set it for offset.

.. function:: setGyrosOffset(x_offset,y_offset,z_offset)

    First place the MP6050 module horizontally and use the function 'getGyros()' to read the static register values. and set it for offset.

Example
---------

Examples for MPU6050::

    from machine import *
    import time

    mpu = MPU6050()
    mpu.init()
    mpu.DMPinit()

    accel_offset = [0,0,0]
    gyros_offset = [0,0,0]

    accel_offset = mpu.getAccel()
    gyros_offset = mpu.getGyros()

    mpu.setAccelOffset(accel_offset)
    mpu.setGyrosOffset(gyros_offset)

    print('Temp:')
    print(mpu.getTemp())

    while True:
        print(mpu.getAccel_x())
        print(mpu.getAccel_y())
        print(mpu.getAccel_z())

        print(mpu.getGyro_x())
        print(mpu.getGyro_y())
        print(mpu.getGyro_z())

        print(mpu.getDMPdata())

