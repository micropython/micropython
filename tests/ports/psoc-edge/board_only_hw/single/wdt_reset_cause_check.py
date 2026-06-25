import machine

# ---------------------------------------------------------------------------
# Runs after wdt.py triggered a WDT reset.
# ---------------------------------------------------------------------------

print("***** Test 9: reset_cause() after WDT reset *****")
if machine.reset_cause() == machine.WDT_RESET:
    print("PASS")
else:
    print("FAIL: reset_cause =", machine.reset_cause())
