try:
    import uos, utime
except ImportError:
    print("SKIP")
    raise SystemExit

# Verify that sys is a builtin.
import sys

print(sys, hasattr(sys, "__file__"))

sys.path.clear()
sys.path.append("ext")

# All three should only get builtins, despite sys.py, usys.py, and
# micropython.py being in the path.
# usys isn't extensible, but has a special-cased alias for backwards
# compatibility.
import micropython

print(micropython, hasattr(micropython, "__file__"))
import sys

print(sys, hasattr(sys, "__file__"))
import usys

print(usys, hasattr(usys, "__file__"))

# This should get os.py, which uses uos to get the builtin.
import os

print(os, hasattr(os, "__file__"), os.sep, os.extra)

# This should get time.py, which uses empty sys.path to get the builtin.
import time

print(time, hasattr(time, "__file__"), time.sleep, time.extra)

# These should get the builtins.
import uos

print(uos, hasattr(uos, "__file__"), hasattr(uos, "extra"))

import utime

print(utime, hasattr(utime, "__file__"), hasattr(utime, "extra"))
