import psoc_edge

# ---------------------------------------------------------------------------
# Runs after wdt.py triggered a WDT reset.
# On PSOC Edge, Cy_WDT fires via the XRES line; the WDT ISR stamps
# BACKUP_BREG_SET0[0] before XRES fires so psoc_edge.system_reset_cause()
# can distinguish a WDT reset from a physical reset-button press.
# It reads hardware directly, bypassing the SW reset_cause variable that
# machine_deinit() overwrites on every Ctrl-D.
# SYSTEM_RESET_WDT = 1 (system_reset_reason_t)
# ---------------------------------------------------------------------------

print("***** Test 9: reset_cause() after WDT reset *****")
cause = psoc_edge.system_reset_cause()
if cause == 1:
    print("PASS")
else:
    print("FAIL: reset_cause =", cause)
