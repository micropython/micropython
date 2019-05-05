:mod:`pca9685` --- functionality specific to the nRF52832 Bluefruit52
====================================================

.. module:: pca9685
    :synopsis: functionality specific to the nRF52832 Bluefruit52

The ``pca9685`` module contains functions and classes specifically aimed at
controlling nRF52832 Bluefruit52 modules.


Functions
---------

.. function:: PCA9685()

    New a object PCA9685 driver chip.

.. function:: reset()

    reset PCA9685 register.

.. function:: freq(freq = 50)

    set PCA9685 out pwm frequency.

.. function:: pwm(inedx, on, off)

    set pwm out level.

.. function:: duty(index, value, invert)

    set pwm out duty. 

.. module:: servo
    :synopsis: functionality specific to the nRF52832 Bluefruit52

:mod:`servo` --- functionality specific to the nRF52832 Bluefruit52
====================================================

The ``servo`` module contains functions and classes specifically aimed at
controlling nRF52832 Bluefruit52 modules.


Functions
---------

.. function:: Servos()

    New a object Servo by PCA9685.

.. function:: position(index, degrees)

    set servo go to position(default:0~180).

.. function:: release(index)

    release servo.
