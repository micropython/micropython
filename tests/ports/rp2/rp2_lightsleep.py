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
import sys

try:
    from machine import lightsleep, Pin
except ImportError:
    print("SKIP")
    raise SystemExit
try:
    led = Pin(Pin.board.LED, Pin.OUT)
except AttributeError:
    led = None

for n in range(100):
    if led:
        led.toggle()
    sys.stdout.write(chr(ord("a") + (n % 26)))
    lightsleep(2 ** (n % 10))

print("\nDONE")
