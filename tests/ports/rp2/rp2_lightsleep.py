# This test is mostly intended for ensuring USB serial stays stable over
# lightsleep, but can double as a general lightsleep test.
#
# In theory this should run on any port, but native USB REPL doesn't currently
# recover automatically on all ports. On pyboard and ESP32-S3 the host needs to
# send something to the port before it responds again. Possibly the same for other
# ports with native USB.
#
# A range of sleep periods (1 to 512ms) are tested. The total nominal sleep time
# is 10.23 seconds, but on most ports this will finish much earlier as interrupts
# happen before each timeout expires.
import sys, time

try:
    from machine import lightsleep, Pin
except ImportError:
    print("SKIP")
    raise SystemExit

# RP2350 currently fails this test, needs further investigation.
if "RP2350" in sys.implementation._machine:
    print("SKIP")
    raise SystemExit

try:
    led = Pin(Pin.board.LED, Pin.OUT)
except AttributeError:
    led = None

t0 = time.ticks_ms()

for n in range(100):
    if led:
        led.toggle()
    sys.stdout.write(chr(ord("a") + (n % 26)))
    lightsleep(2 ** (n % 10))

print()  # end of line

delta = time.ticks_diff(time.ticks_ms(), t0)

if delta < 2500:
    # As per note above, we don't expect the full 10.23 seconds
    # but a very short total runtime may indicate a bug with
    # lightsleep
    print("Unexpected short test time", delta, "ms")
elif delta > 11000:
    # Similarly, the test shouldn't take much longer than the max lightsleep
    # time
    print("Unexpected long test time", delta, "ms")
else:
    print("Test time OK")

print("DONE")
