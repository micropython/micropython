from machine import Pin
import time
import machine

# Tests for Output pin
# Instantiate output pin for led operation
# Constr
p1 = Pin("P13_7")
p1.init(Pin.OUT)
print(p1)  # Pin:111 or P13_7, Mode=OUT, Pull=None, Value=1

p1.value(0)  # Led glows
print(p1.value())  # None

p1.off()  # Switches off led
print(p1.off())  # None

p1.value(False)  # Led glows
print(p1.value())  # None

# Tests for Input pin
# Instantiate input pin for button operation
p2 = Pin("P0_4", Pin.IN)
print(p2)  # Pin:4 or P0_4, Mode=IN, Pull=None, Value=1
p2.value()
print(p2.value())  # 1 - button is released
