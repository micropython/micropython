from machine import Pin, Signal

# P16_0 (out) and P16_1 (in) must be connected together on the board

pin_out = Pin("P16_0", mode=Pin.OUT, value=0)
pin_in = Pin("P16_1", mode=Pin.IN)

# --- Non-inverted signal wrapping an existing Pin object ---
print("non-inverted")

signal = Signal(pin_out, invert=False)

signal.on()
print("on() drives high:  ", pin_in.value() == 1)

signal.off()
print("off() drives low:  ", pin_in.value() == 0)

signal.value(1)
print("value(1) drives high: ", pin_in.value() == 1)

signal.value(0)
print("value(0) drives low:  ", pin_in.value() == 0)

# value() getter returns the logical signal level
signal.on()
print("value() getter high:  ", signal.value() == 1)
signal.off()
print("value() getter low:   ", signal.value() == 0)

# --- Inverted signal wrapping an existing Pin object ---
print("inverted")

signal = Signal(pin_out, invert=True)

signal.on()
print("on() drives low:   ", pin_in.value() == 0)

signal.off()
print("off() drives high: ", pin_in.value() == 1)

signal.value(1)
print("value(1) drives low:  ", pin_in.value() == 0)

signal.value(0)
print("value(0) drives high: ", pin_in.value() == 1)

# value() getter returns the logical (inverted) signal level
signal.on()
print("value() getter high:  ", signal.value() == 1)
signal.off()
print("value() getter low:   ", signal.value() == 0)

# --- Signal constructed directly from pin ID string ---
print("construct from id")

signal = Signal("P16_0", Pin.OUT, invert=False)
signal.on()
print("id construct on():  ", pin_in.value() == 1)
signal.off()
print("id construct off(): ", pin_in.value() == 0)
