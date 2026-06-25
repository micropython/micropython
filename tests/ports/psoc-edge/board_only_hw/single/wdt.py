import machine
import time

# ---------------------------------------------------------------------------
# Invalid parameter tests (before WDT is started)
# ---------------------------------------------------------------------------

print("***** Test 1: Wrong WDT id *****")
try:
    wdt = machine.WDT(1, timeout=5000)
except ValueError:
    print("ValueError raised - PASS")

print("***** Test 2: WDT timeout too short *****")
try:
    wdt = machine.WDT(0, timeout=0)
except ValueError:
    print("ValueError raised - PASS")

print("***** Test 3: WDT timeout too long *****")
# PSoC Edge max is 383999 ms (22-bit counter at 32768 Hz, 3-match reset)
try:
    wdt = machine.WDT(0, timeout=400000)
except ValueError:
    print("ValueError raised - PASS")

# ---------------------------------------------------------------------------
# Valid creation and feed near the timeout boundary.
# If the WDT fires before feed() is called the board resets and the
# test produces no further output (i.e. it fails).
# ---------------------------------------------------------------------------

print("***** Test 4: WDT created with 5000 ms timeout *****")
wdt = machine.WDT(id=0, timeout=5000)
print("WDT created - PASS")

print("***** Test 5: Feed near boundary (cycle 1) *****")
time.sleep_ms(3500)
wdt.feed()
print("Feed OK - PASS")

print("***** Test 6: Feed near boundary (cycle 2) *****")
time.sleep_ms(3500)
wdt.feed()
print("Feed OK - PASS")

# ---------------------------------------------------------------------------
# Reconfigure to a short timeout without feeding so the WDT fires within
# post_test_delay_ms. wdt_reset_cause_check.py verifies reset_cause() == WDT_RESET.
# ---------------------------------------------------------------------------

print("***** Test 7: Reconfigure WDT to trigger reset *****")
wdt = machine.WDT(0, timeout=1500)
print("Reconfigure OK - PASS")
# WDT fires ~1500 ms after this; board resets within post_test_delay_ms.
