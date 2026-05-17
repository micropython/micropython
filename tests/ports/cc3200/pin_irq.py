"""
Pin IRQ test for the CC3200 based boards.
"""

from machine import Pin
import machine
import os
import time

mch = os.uname().machine
if "LaunchPad" in mch:
    pins = ["GP16", "GP13"]
elif "WiPy" in mch:
    pins = ["GP16", "GP13"]
else:
    raise Exception("Board not supported!")

pin0 = Pin(pins[0], mode=Pin.OUT, value=1)
pin1 = Pin(pins[1], mode=Pin.IN, pull=Pin.PULL_UP)


def pin_handler(pin_o):
    global pin_irq_count_trigger
    global pin_irq_count_total
    global _trigger
    if _trigger & pin1_irq.flags():
        pin_irq_count_trigger += 1
    pin_irq_count_total += 1


pin_irq_count_trigger = 0
pin_irq_count_total = 0
_trigger = Pin.IRQ_FALLING
pin1_irq = pin1.irq(trigger=_trigger, handler=pin_handler)
for i in range(0, 10):
    pin0.toggle()
    time.sleep_ms(5)
print(pin_irq_count_trigger == 5)
print(pin_irq_count_total == 5)

pin_irq_count_trigger = 0
pin_irq_count_total = 0
_trigger = Pin.IRQ_RISING
pin1_irq = pin1.irq(trigger=_trigger, handler=pin_handler)
for i in range(0, 200):
    pin0.toggle()
    time.sleep_ms(5)
print(pin_irq_count_trigger == 100)
print(pin_irq_count_total == 100)

pin1_irq.disable()
pin0(1)
pin_irq_count_trigger = 0
pin_irq_count_total = 0
_trigger = Pin.IRQ_FALLING
pin1_irq.init(trigger=_trigger, handler=pin_handler)
pin0(0)
time.sleep_us(50)
print(pin_irq_count_trigger == 1)
print(pin_irq_count_total == 1)
pin0(1)
time.sleep_us(50)
print(pin_irq_count_trigger == 1)
print(pin_irq_count_total == 1)

# check the call method
pin1_irq()
print(pin_irq_count_trigger == 1)  # no flags since the irq was manually triggered
print(pin_irq_count_total == 2)

pin1_irq.disable()
pin_irq_count_trigger = 0
pin_irq_count_total = 0
for i in range(0, 10):
    pin0.toggle()
    time.sleep_ms(5)
print(pin_irq_count_trigger == 0)
print(pin_irq_count_total == 0)

# test waking up from suspended mode on low level
pin0(0)
t0 = time.ticks_ms()
pin1_irq.init(trigger=Pin.IRQ_LOW_LEVEL, wake=machine.SLEEP)
machine.sleep()
print(time.ticks_ms() - t0 < 10)
print("Awake")

# test waking up from suspended mode on high level
pin0(1)
t0 = time.ticks_ms()
pin1_irq.init(trigger=Pin.IRQ_HIGH_LEVEL, wake=machine.SLEEP)
machine.sleep()
print(time.ticks_ms() - t0 < 10)
print("Awake")

# check for memory leaks
for i in range(0, 1000):
    pin0_irq = pin0.irq(trigger=_trigger, handler=pin_handler)
    pin1_irq = pin1.irq(trigger=_trigger, handler=pin_handler)

# next ones must raise
try:
    pin1_irq.init(trigger=123456, handler=pin_handler)
except:
    print("Exception")

try:
    pin1_irq.init(trigger=Pin.IRQ_LOW_LEVEL, wake=1789456)
except:
    print("Exception")

try:
    pin0_irq = pin0.irq(
        trigger=Pin.IRQ_RISING, wake=machine.SLEEP
    )  # GP16 can't wake up from DEEPSLEEP
except:
    print("Exception")

pin0_irq.disable()
pin1_irq.disable()
