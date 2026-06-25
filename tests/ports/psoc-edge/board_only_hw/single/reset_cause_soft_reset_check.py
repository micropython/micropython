import machine

# ---------------------------------------------------------------------------
# Runs after reset_cause.py triggered a soft reset.
# ---------------------------------------------------------------------------

print("***** Test 3: reset_cause() after soft reset *****")
if machine.reset_cause() == machine.SOFT_RESET:
    print("PASS")
else:
    print("FAIL: reset_cause =", machine.reset_cause())
