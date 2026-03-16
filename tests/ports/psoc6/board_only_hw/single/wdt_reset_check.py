import psoc6
import sys

reset_cause = psoc6.system_reset_cause()
if reset_cause != 1:
    print("Failed as reset reason is: " + str(reset_cause))
