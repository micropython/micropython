# Dev by Sonthaya Nongnuch

from machine import Pin

S1 = Pin(16, Pin.IN, Pin.PULL_UP)
S2 = Pin(14, Pin.IN, Pin.PULL_UP)

__s1_press = None
__s1_release = None
__s2_press = None
__s2_release = None

def __onSwitchChangesValue(pin):
    if pin.value():
        callback = None
        if pin == S1:
            callback = __s1_release
        elif pin == S2:
            callback = __s2_release
        if callback:
            callback()
    else:
        callback = None
        if pin == S1:
            callback = __s1_press
        elif pin == S2:
            callback = __s2_press
        if callback:
            callback()


S1.irq(handler=__onSwitchChangesValue, trigger=Pin.IRQ_FALLING | Pin.IRQ_RISING)
S2.irq(handler=__onSwitchChangesValue, trigger=Pin.IRQ_FALLING | Pin.IRQ_RISING)

def value(pin):
    return 0 if pin.value() else 1

def press(pin, callback):
    global __s1_press, __s2_press
    if pin == S1:
        __s1_press = callback
    elif pin == S2:
        __s2_press = callback

def release(pin, callback):
    global __s1_release, __s2_release
    if pin == S1:
        __s1_release = callback
    elif pin == S2:
        __s2_release = callback
