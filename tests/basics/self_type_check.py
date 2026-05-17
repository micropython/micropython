# make sure type of first arg (self) to a builtin method is checked

import sys

# Minimal builds usually don't enable MICROPY_BUILTIN_METHOD_CHECK_SELF_ARG,
# which is required for this test.
if getattr(sys.implementation, "_build", None) == "minimal":
    print("SKIP")
    raise SystemExit

list.append

try:
    list.append()
except TypeError as e:
    print("TypeError")

try:
    list.append(1)
except TypeError as e:
    print("TypeError")

try:
    list.append(1, 2)
except TypeError as e:
    print("TypeError")

l = []
list.append(l, 2)
print(l)

try:
    getattr(list, "append")(1, 2)
except TypeError as e:
    print("TypeError")

l = []
getattr(list, "append")(l, 2)
print(l)
