import sys
try:
    sys.exc_info
except:
    print("SKIP")
    sys.exit()

def f():
    print(sys.exc_info()[0:2])

try:
    1/0
except:
    print(sys.exc_info()[0:2])
    f()

# MicroPython currently doesn't reset sys.exc_info() value
# on exit from "except" block.
#f()

# Recursive except blocks are not handled either - just don't
# use exc_info() at all!
