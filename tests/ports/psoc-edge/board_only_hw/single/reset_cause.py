import machine

# ---------------------------------------------------------------------------
# Verify reset cause constants are accessible.
# ---------------------------------------------------------------------------

print("***** Test 1: Reset cause constants accessible *****")
_ = machine.PWRON_RESET
_ = machine.HARD_RESET
_ = machine.WDT_RESET
_ = machine.DEEPSLEEP_RESET
_ = machine.SOFT_RESET
print("PASS")

# ---------------------------------------------------------------------------
# Trigger a MicroPython soft reset.
# reset_cause_soft_reset_check.py verifies machine.reset_cause() == SOFT_RESET.
# ---------------------------------------------------------------------------

print("***** Test 2: Trigger soft reset *****")
machine.soft_reset()
