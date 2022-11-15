.. _pins_zephyr:

GPIO Pins
=========

Use :ref:`machine.Pin <machine.Pin>` to control I/O pins.

For Zephyr, pins are initialized using a tuple of port and pin number ``(\"GPIO_x\", pin#)``
for the ``id`` value. For example to initialize a pin for the red LED on a FRDM-k64 board::

        LED = Pin(("GPIO_1", 22), Pin.OUT)

Reference your board's datasheet or Zephyr documentation for pin numbers, see below for more examples.

.. list-table:: Pin Formatting
   :header-rows: 1

   * - Board
     - Pin
     - Format
   * - frdm_k64f
     - Red LED = PTB22
     - ("GPIO_1", 22)
   * - 96b_carbon
     - LED1 = PD2
     - ("GPIOD", 2)
   * - mimxrt685_evk_cm33
     - Green LED = PIO0_14
     - ("GPIO0", 14)

Interrupts
----------

The Zephyr port also supports interrupt handling for Pins using `machine.Pin.irq() <machine.Pin.irq>`.
To respond to Pin change IRQs run::

    from machine import Pin

    SW2 = Pin(("GPIO_2", 6), Pin.IN)            # create Pin object for switch 2
    SW3 = Pin(("GPIO_0", 4), Pin.IN)            # create Pin object for switch 3

    SW2.irq(lambda t: print("SW2 changed"))     # print message when SW2 state is changed (triggers change IRQ)
    SW3.irq(lambda t: print("SW3 changed"))     # print message when SW3 state is changed (triggers change IRQ)

    while True:                                 # wait
        pass
