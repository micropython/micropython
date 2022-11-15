.. _renesas-ra_using_peripheral:

Using peripherals
=================

For quick help information, please enter::

    help()

You can access RA MCU's peripherals using MicroPython modules.
To list supported modules, please enter::

    help('modules')

Especially `machine` module and class :ref:`machine.Pin <machine.Pin>` are very important for using
peripherals. Note that prefix 'u' is added to the module for MicroPython,
so you can see "umachine" in the list but you can use it like "import machine".

Using "from machine import Pin", Pin name is available corresponding to
the RA MCU's pin name which are Pin.cpu.P000 and 'P000'.
In addition, you can use 'LED1', 'LED2', 'SW1', and 'SW2' name if the board
has these LEDs and switches.

LED blinking
------------

As simple example, you can enter following program to blink LED1.
Please enter key 4 times after the input of last time.sleep(1). ::

    import time
    from machine import Pin
    led1 = Pin('LED1')
    print(led1)
    while True:
        led1.on()
        time.sleep(1)
        led1.off()
        time.sleep(1)

You can see the LED1 blinking per 1 second.

If you want to stop the program, please enter CTRL-C. ::

    Traceback (most recent call last):
      File "<stdin>", line 5, in <module>
    KeyboardInterrupt:

This message is displayed, and the program stops.
The message means the program was interrupted at line 5 "while" statement.

Using print(led1), you can confirm that LED1 is assigned to Pin.cpu.P106
on the board.::

     Pin(Pin.cpu.P106, mode=Pin.OUT, pull=Pin.PULL_NONE, drive=Pin.LOW_POWER)

So you can get the same result if Pin(Pin.cpu.P106) is specified
instead of Pin('LED1'). ::

    import time
    from machine import Pin
    led1 = Pin(Pin.cpu.P106)
    print(led1)
    while True:
        led1.on()
        time.sleep(1)
        led1.off()
        time.sleep(1)

