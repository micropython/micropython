# cmdline: -m cmdline.cmd_module_atexit_exc
#
# Test running as a module and using sys.atexit, with script completion via sys.exit.

import sys

if not hasattr(sys, "atexit"):
    print("SKIP")
    raise SystemExit

# Verify we ran as a module.
print(sys.argv)

sys.atexit(lambda: print("done"))

print("start")

# This will raise SystemExit to finish the script, and atexit should still be run.
sys.exit(0)
