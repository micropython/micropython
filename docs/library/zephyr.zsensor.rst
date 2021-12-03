.. currentmodule:: zsensor

:mod:`zsensor` --- Zephyr sensor bindings
=========================================

.. module:: zsensor
    :synopsis: zephyr sensor bindings

The ``zsensor`` module contains a class for using sensors with Zephyr.

.. _zsensor.Sensor:

class Sensor --- sensor control for the Zephyr port
---------------------------------------------------

Use this class to access data from sensors on your board.
See Zephyr documentation for sensor usage here: `Sensors
<https://docs.zephyrproject.org/latest/reference/peripherals/sensor.html?highlight=sensor#sensors>`_.

Sensors are defined in the Zephyr devicetree for each board. The quantities that a given sensor can
measure are called a sensor channels. Sensors can have multiple channels to represent different axes
of one property or different properties a sensor can measure. See `Channels`_ below for defined sensor
channels.

Constructor
~~~~~~~~~~~

.. class:: Sensor(device_name)

    Device names are defined in the devicetree for your board.
    For example, the device name for the accelerometer in the FRDM-k64f board is "FXOS8700".

Methods
~~~~~~~

.. method:: Sensor.measure()

    Obtains a measurement sample from the sensor device using Zephyr sensor_sample_fetch and
    stores it in an internal driver buffer as a useful value, a pair of (integer part of value,
    fractional part of value in 1-millionths).
    Returns none if successful or OSError value if failure.

.. method:: Sensor.get_float(sensor_channel)

    Returns the value of the sensor measurement sample as a float.

.. method:: Sensor.get_micros(sensor_channel)

    Returns the value of the sensor measurement sample in millionths.
    (Ex. value of ``(1, 500000)`` returns as ``1500000``)

.. method:: Sensor.get_millis(sensor_channel)

    Returns the value of sensor measurement sample in thousandths.
    (Ex. value of ``(1, 500000)`` returns as ``1500``)

.. method:: Sensor.get_int(sensor_channel)

    Returns only the integer value of the measurement sample.
    (Ex. value of ``(1, 500000)`` returns as ``1``)

Channels
~~~~~~~~

.. data:: ACCEL_X

    Acceleration on the X axis, in m/s^2.

.. data:: ACCEL_Y

    Acceleration on the Y axis, in m/s^2.

.. data:: ACCEL_Z

    Acceleration on the Z axis, in m/s^2.

.. data:: GYRO_X

    Angular velocity around the X axis, in radians/s.

.. data:: GYRO_Y

    Angular velocity around the Y axis, in radians/s.

.. data:: GYRO_Z

    Angular velocity around the Z axis, in radians/s.

.. data:: MAGN_X

    Magnetic field on the X axis, in Gauss.

.. data:: MAGN_Y

    Magnetic field on the Y axis, in Gauss.

.. data:: MAGN_Z

    Magnetic field on the Z axis, in Gauss.

.. data:: DIE_TEMP

    Device die temperature in degrees Celsius.

.. data:: PRESS

    Pressure in kilopascal.

.. data:: PROX

    Proximity. Dimensionless. A value of 1 indicates that an object is close.

.. data:: HUMIDITY

    Humidity, in percent.

.. data:: LIGHT

    Illuminance in visible spectrum, in lux.

.. data:: ALTITUDE

    Altitude, in meters.
