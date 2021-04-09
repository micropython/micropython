# test the micropython.kbd_intr() function

import micropython

try:
    micropython.kbd_intr
except AttributeError:
    print("SKIP")
    raise SystemExit

# just check we can actually call it
micropython.kbd_intr(3)
