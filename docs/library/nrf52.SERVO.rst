:mod:`pca9685` --- functionality specific to the nRF52832 Bluefruit52
====================================================

.. module:: pca9685
    :synopsis: functionality specific to the nRF52832 Bluefruit52

The ``pca9685`` module contains functions and classes specifically aimed at
controlling nRF52832 Bluefruit52 modules.


Functions
---------

.. function:: PCA9685(i2c,address)

    New a object PCA9685 driver chip.

.. function:: reset()

    reset PCA9685 register.

.. function:: freq(freq = 50)

    Get and set the pwm frequency.

.. function:: pwm(inedx, [on], [off])

    Get or set the PWM signal's on and off timings for the channel `index`.

.. function:: duty(index, [value], [invert])

    Get or set the PWM duty cycle in range 0-4095 (4095 corresponds to 100%
    duty cycle). If `invert` is `True`, 4095 corresponds to 0 and 0
    corresponds to 100%. 

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
    Allows controlling up to 16 hobby servos.
    The freq argument sets the PWM signal frequency in Hz. Analog servos usually expect this to be 50, but digital servos can often handle higher frequencies, resulting in smoother movements.

    The min_us and max_us arguments set the range of the singnal's duty that the servo accepts. This is different between different servo models, but usually they are centerd at 1500µs.

    The degrees argument specifies the physical range of the servo corresponding to the signal's duty range specified before. It is used to calculate signal's duty when the angle is specified in degrees or radians.

.. function:: position(index, [degrees])

    set servo go to position(default:0~180).
    Get or set the servo position. The position can be specified in
    degrees (the default), radians, microseconds or directly as a number
    between 0 and 4095 signifying the duty cycle. It will be automatically
    clamped to the minimum and maximum range allowed.

    When getting the position, it will always be returned in duty cycle.

.. function:: release(index)

    Stop sending a signal to the given servomechanism, releasing it.
