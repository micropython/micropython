# Test that micropython.stdio_mode_raw exists and is callable.
try:
    import micropython

    micropython.stdio_mode_raw
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

# Should not raise even if stdin is not a tty (HAL ignores errors).
micropython.stdio_mode_raw(True)
micropython.stdio_mode_raw(False)

# Wrong number of arguments.
try:
    micropython.stdio_mode_raw()
except TypeError:
    print("TypeError")

try:
    micropython.stdio_mode_raw(True, True)
except TypeError:
    print("TypeError")

print("ok")
