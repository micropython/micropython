import machine

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
# Valid creation and feed
# ---------------------------------------------------------------------------

print("***** Test 4: WDT created with 5000 ms timeout *****")
wdt = machine.WDT(id=0, timeout=5000)
print("WDT created - PASS")

print("***** Test 5: Feed *****")
wdt.feed()
print("Feed OK - PASS")

print("***** Test 6: Feed again *****")
wdt.feed()
print("Feed OK - PASS")

# ---------------------------------------------------------------------------
# Singleton: second instance must be rejected
# ---------------------------------------------------------------------------

print("***** Test 7: Second WDT instance rejected *****")
try:
    wdt = machine.WDT(0, timeout=5000)
except ValueError:
    print("ValueError raised - PASS")
