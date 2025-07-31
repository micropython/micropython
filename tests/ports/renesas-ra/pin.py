from machine import Pin

p = Pin("SW1", Pin.IN)
if p.value() == 1:
    print("pass")
else:
    print("FAIL")

p = Pin("LED1", Pin.OUT)
p.high()
if p.value() == 1:
    print("pass")
else:
    print("FAIL")
p.low()
