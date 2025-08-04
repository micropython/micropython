import machine
import time

# invalid test cases
print("\n***** Test 1: Wrong WDT id *****\n")
try:
    wdt = machine.WDT(1, timeout=6000)
except Exception:
    print("FAIL")

print("\n***** Test 2: WDT timeout too low *****\n")
try:
    wdt = machine.WDT(0, 0)
except Exception:
    print("FAIL")

print("\n***** Test 3: WDT timeout too high *****\n")
try:
    wdt = machine.WDT(id=0, timeout=6001)
except Exception:
    print("FAIL")

# valid test cases

print("\n***** Test 4: WDT created successfully 1.5s *****\n")
wdt = machine.WDT(id=0, timeout=1500)
print(wdt)
print("PASS")

print("\n***** Test 5: WDT feed after 100ms *****\n")
time.sleep_ms(100)
wdt.feed()
print(wdt)
print("PASS")

print("\n***** Test 6: WDT feed after 200ms *****\n")
time.sleep_ms(200)
wdt.feed()
print(wdt)
print("PASS")

# reinitializing again fails

print("\n***** Test 7: trying to create WDT 2nd instance *****\n")
try:
    wdt = machine.WDT(0, timeout=1000)
except Exception:
    print("FAIL")
