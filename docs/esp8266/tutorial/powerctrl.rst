Power control
=============

The ESP8266 provides the ability to change the CPU frequency on the fly, and
enter a deep-sleep state.  Both can be used to manage power consumption.

Changing the CPU frequency
--------------------------

The machine module has a function to get and set the CPU frequency.  To get the
current frequency use::

    >>> import machine
    >>> machine.freq()
    80000000

By default the CPU runs at 80MHz.  It can be change to 160MHz if you need more
processing power, at the expense of current consumption::

    >>> machine.freq(160000000)
    >>> machine.freq()
    160000000

You can change to the higher frequency just while your code does the heavy
processing and then change back when its finished.

Deep-sleep mode
---------------

The deep-sleep mode will shut down the ESP8266 and all its peripherals,
including the WiFi (but not including the real-time-clock, which is used to wake
the chip).  This drastically reduces current consumption and is a good way to
make devices that can run for a while on a battery.

To be able to use the deep-sleep feature you must connect GPIO16 to the reset
pin (RST on the Adafruit Feather HUZZAH board).  Then the following code can be
used to sleep and wake the device::

    import machine

    # configure RTC.ALARM0 to be able to wake the device
    rtc = machine.RTC()
    rtc.irq(trigger=rtc.ALARM0, wake=machine.DEEPSLEEP)

    # set RTC.ALARM0 to fire after 10 seconds (waking the device)
    rtc.alarm(rtc.ALARM0, 10000)

    # put the device to sleep
    machine.deepsleep()

Note that when the chip wakes from a deep-sleep it is completely reset,
including all of the memory.  The boot scripts will run as usual and you can
put code in them to check the reset cause to perhaps do something different if
the device just woke from a deep-sleep.  For example, to print the reset cause
you can use::

    if machine.reset_cause() == machine.DEEPSLEEP_RESET:
        print('woke from a deep sleep')
    else:
        print('power on or hard reset')

