# cmdline: -m cmdline.cmd_module_atexit
#
# Test running as a module and using sys.atexit.

import sys

if not hasattr(sys, "atexit"):
    print("SKIP")
    raise SystemExit

# Verify we ran as a module.
print(sys.argv)

sys.atexit(lambda: print("done"))

print("start")
