from machine import Pin

pin_out_name = "P16_0"  # Use string label
pin_out = Pin(pin_out_name, mode=Pin.OUT, value=True)
print(pin_out)

pin_in = Pin.cpu.P16_1
pin_in.init(mode=Pin.IN, pull=Pin.PULL_UP)
print(pin_in)
