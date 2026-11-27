# MIT license; Copyright (c) 2026 OpenMV LLC.
#
# Test GPIO and pin configuration (pinconf) using a loopback.
#
# IMPORTANT: This test requires hardware connections: the two pins in
# `pwm_loopback_pins` must be wired together.
#
# requires: target-wiring

import time
from machine import Pin
from target_wiring import pwm_loopback_pins

out_pin, in_pin = pwm_loopback_pins[0]

# Digital output to input.
pin_out = Pin(out_pin, Pin.OUT, value=0)
pin_in = Pin(in_pin, Pin.IN)
values = []
for v in (0, 1, 0, 1):
    pin_out.value(v)
    time.sleep_us(10)
    values.append(pin_in.value())
print("loopback", values)

# Pull-up, with the driving pin as an input.  Pull-down is not tested because
# on boards like the OpenMV AE3 these pins go through a level shifter with
# pull-ups.
pin_out.init(Pin.IN)
pin_in.init(Pin.IN, Pin.PULL_UP)
time.sleep_ms(1)
print("pull_up", pin_in.value())

# Open-drain output: pulled up by the input pin when released.
pin_in.init(Pin.IN, Pin.PULL_UP)
pin_out.init(Pin.OPEN_DRAIN, value=0)
time.sleep_ms(1)
low = pin_in.value()
pin_out.value(1)
time.sleep_ms(1)
high = pin_in.value()
print("open_drain", low, high)

# Edge interrupts.
pin_out.init(Pin.OUT, value=0)
pin_in.init(Pin.IN)
events = 0


def irq_handler(p):
    global events
    events += 1


pin_in.irq(irq_handler, Pin.IRQ_RISING | Pin.IRQ_FALLING)
for _ in range(10):
    pin_out.value(1)
    time.sleep_ms(2)
    pin_out.value(0)
    time.sleep_ms(2)
pin_in.irq(None)
print("irq", events)

# Rising edge only.
events = 0
pin_in.irq(irq_handler, Pin.IRQ_RISING)
for _ in range(5):
    pin_out.value(1)
    time.sleep_ms(2)
    pin_out.value(0)
    time.sleep_ms(2)
pin_in.irq(None)
print("irq_rising", events)

pin_out.init(Pin.IN)
