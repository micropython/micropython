# Test that subnormal float32s are correctly converted to int (esp. on LONGLONG)
# requires: float
import sys

if "rp2" in getattr(sys, "platform", ""):
    # due to an upstream bug rp2 does not handle subnomal numbers:
    # https://github.com/raspberrypi/pico-sdk/issues/1582
    print("SKIP")
    raise SystemExit

e = 1e-40
print(e != 0)
print(int(e))

e = -1e-40
print(e != 0)
print(int(e))
