from machine import Pin, bitstream
import time

pin_tx = Pin("P16_0", mode=Pin.OUT, value=0)
pin_api = Pin("P16_1", mode=Pin.OUT, value=0)

# API validation cases.
api_timing = (500, 1500, 1500, 500)
api_data = bytes([0x10, 0x00, 0x00])

call_ok = True
try:
    bitstream(pin_api, 0, api_timing, api_data)
except Exception:
    call_ok = False
print("bitstream call ok:", call_ok)

try:
    bitstream(pin_api, 0, (299, 850, 800, 450), api_data)
except ValueError as exc:
    print("bitstream rejects short timing:", str(exc) == "timing must be >= 300 ns")
else:
    print("bitstream rejects short timing:", False)

# Timing: (T0H, T0L, T1H, T1L) in nanoseconds
# 500us/1500us pulses so IRQ latency (~100us) is negligible for accurate decoding
timing = (500000, 1500000, 1500000, 500000)  # nanoseconds (500us / 1500us)

# 4-byte test pattern
test_data = bytes([0x12, 0x34, 0x56, 0x78])

# Drive pin LOW immediately as a connectivity beacon.
# RX has a pull-up; if TX is electrically connected, RX will see LOW here.
# This actively overrides the pull-up and proves the wire is present.
pin_tx.value(0)

# In multi_stub mode TX stub starts first; sleep long enough for RX DUT to connect,
# sample the LOW beacon, then arm its capture window before transmission begins.
time.sleep_ms(8000)

try:
    bitstream(pin_tx, 0, timing, test_data)
    print("bitstream tx ok: True")
except Exception as e:
    print("bitstream tx ok: False")
