# test __dict__ attribute of a built-in module
# see import/module_dict.py for the equivalent test on user modules

import sys

if not hasattr(sys, "__dict__"):
    print("SKIP")
    raise SystemExit


# dict of a built-in module (read-only)
print(type(sys.__dict__))
print(sys.__dict__["__name__"])
