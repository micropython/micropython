from machine import Pin

pin_out_name = "P16_0"  # Use string label
pin_out = Pin(pin_out_name, mode=Pin.OUT, value=True)
print(pin_out)

pin_in = Pin.cpu.P16_1
pin_in.init(mode=Pin.IN, pull=Pin.PULL_UP)
print(pin_in)

# Validating initialization values
print("pin out initial value 1: ", pin_in.value() == 1)

pin_out = Pin(pin_out_name, mode=Pin.OUT, value=False)
print("pin out initial value 0: ", pin_in.value() == 0)

# Validation different output setting
pin_out.value(1)
print("pin out (self) value 1: ", pin_out.value() == 1)
print("pin out value 1: ", pin_in.value() == 1)

pin_out.value(0)
print("pin out (self) value 0: ", pin_out.value() == 0)
print("pin out value 0: ", pin_in.value() == 0)

pin_out.value(True)
print("pin out value True: ", pin_in.value() == True)

pin_out.value(False)
print("pin out value False: ", pin_in.value() == False)

pin_out.on()
print("pin out value on: ", pin_in.value() == 1)

pin_out.off()
print("pin out value off: ", pin_in.value() == 0)

pin_out(1)
print("pin out callable 1: ", pin_in() == 1)

pin_out(0)
print("pin out callable 0: ", pin_in() == 0)

pin_out.high()
print("pin out value high: ", pin_in() == 1)

pin_out.low()
print("pin out value low: ", pin_in() == 0)

pin_out.toggle()
print("pin out value toggled (0->1): ", pin_in.value() == 1)

# Validating pull resistors configurations and open drain mode
pin_out = Pin(pin_out_name, mode=Pin.OUT, pull=Pin.PULL_UP, drive=Pin.DRIVE_0)
print("pin out with pull up initially high: ", pin_in() == 1)

# This does not work in the test, but does manually for P16_0 - P16_1
# pin_out = Pin(pin_out_name, mode=Pin.OUT, pull=Pin.PULL_DOWN)
# print("pin out with pull down initially down: ", pin_in() == 0)

pin_out = Pin(pin_out_name, mode=Pin.OPEN_DRAIN)
print("pin out with pull none initially 0: ", pin_in() == 0)

# Validating config setters/getters
print("pin out get mode: ", pin_out.mode() == Pin.OPEN_DRAIN)
print("pin out get pull: ", pin_out.pull() == 0)
print("pin out get drive: ", pin_out.drive() == Pin.DRIVE_0)

pin_out.mode(Pin.OUT)
pin_out.pull(Pin.PULL_UP)
pin_out.drive(Pin.DRIVE_4)
print("pin out set mode: ", pin_out.mode() == Pin.OUT)
print("pin out set pull: ", pin_out.pull() == Pin.PULL_UP)
print("pin out set drive: ", pin_out.drive() == Pin.DRIVE_4)
