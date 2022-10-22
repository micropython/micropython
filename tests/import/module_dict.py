# test __dict__ attribute of a user module

import sys

if not hasattr(sys, "__dict__"):
    print("SKIP")
    raise SystemExit


import import1b

# dict of a user module (read/write)
print(import1b.var)
print(import1b.__dict__["var"])
import1b.__dict__["var"] = "hello"
print(import1b.var)
print(import1b.__dict__["var"])
