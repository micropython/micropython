# Check that super rejects invalid arguments.
try:
    super(str, 0)
except TypeError:
    print("TypeError")

try:
    super(str, int)
except TypeError:
    print("TypeError")

try:
    super(0, int)
except TypeError:
    print("TypeError")
