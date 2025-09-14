# test the micropython.kbd_intr() function

try:
    import micropython

    micropython.kbd_intr
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

# just check we can actually call it
micropython.kbd_intr(3)
