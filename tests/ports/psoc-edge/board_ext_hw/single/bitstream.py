from machine import Pin, bitstream


pin_out = Pin("P16_0", mode=Pin.OUT, value=0)

# timing = (T0H, T0L, T1H, T1L) in nanoseconds
timing = (500, 1500, 1500, 500)
data = bytes([0x10, 0x00, 0x00])

call_ok = True
try:
    bitstream(pin_out, 0, timing, data)
except Exception:
    call_ok = False
print("bitstream call ok:", call_ok)

try:
    bitstream(pin_out, 0, (299, 850, 800, 450), data)
except ValueError as exc:
    print("bitstream rejects short timing:", str(exc) == "timing must be >= 300 ns")
else:
    print("bitstream rejects short timing:", False)
