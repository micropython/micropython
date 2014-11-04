Debouncing a pin input
======================

A pin used as input from a switch or other mechanical device can have a lot
of noise on it, rapidly changing from low to high when the switch is first
pressed or released.  This noise can be eliminated using a capacitor (a
debouncing circuit).  It can also be eliminated using a simple function that
makes sure the value on the pin is stable.

The following function does just this.  It gets the current value of the given
pin, and then waits for the value to change.  The new pin value must be stable
for a continuous 20ms for it to register the change.  You can adjust this time
(to say 50ms) if you still have noise. ::

    import pyb

    def wait_pin_change(pin):
        # wait for pin to change value
        # it needs to be stable for a continuous 20ms
        cur_value = pin.value()
        active = 0
        while active < 20:
            if pin.value() != cur_value:
                active += 1
            else:
                active = 0
            pyb.delay(1)


Use it something like this::

    import pyb

    pin_x1 = pyb.Pin('X1', pyb.Pin.IN, pyb.Pin.PULL_DOWN)
    while True:
        wait_pin_change(pin_x1)
        pyb.LED(4).toggle()
