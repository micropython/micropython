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

.. function:: dmpInit()

    mpu6050 init dmp mode.

.. function:: setDMPEnabled(True)

    set DMP Mode enable.

.. function:: dmpGetFIFOPacketSize()

    Return FIFO packet sizes.

.. function:: getIntStatus()

    Return interrupt status. 

.. function:: getFIFOCount()

    Return FIFO counts. 

.. function:: getFIFOBytes()

    Return FIFO bytes to buffers. 

.. function:: dmpGetQuaternion(fifoBuffer)

    Calculation quaternion value and return to buffers. 

.. function:: dmpGetEuler(*fifoBuffer)

    Calculation euler angle value and return to buffers(yaw,roll,pitch). 

.. function:: getTemp()

    Return the soc temperature register value(°C). 

.. function:: getAccelX()

    Return the Accel X register value. 

.. function:: getAccelY()

    Return the Accel Y register value. 

.. function:: getAccelZ()

    Return the Accel Z register value. 

.. function:: getGyroX()

    Return the Gyro X register value. 

.. function:: getGyroY()

    Return the Gyro Y register value. 

.. function:: getGyroZ()

    Return the Gyro Z register value. 

Constants
---------

.. data:: MPU6050.MPU6050_DEFAULT_ADDRESS
          MPU6050.MPU6050_RA_TEMP_OUT_H

   Selects the register address.
