# Test machine.USBDevice BUILTIN class-flag constants and invalid-mask handling.

try:
    import machine

    usb = machine.USBDevice()
except:
    print("SKIP")
    raise SystemExit

# BUILTIN_NONE and BUILTIN_DEFAULT must always be present.
assert machine.USBDevice.BUILTIN_NONE is not None
assert machine.USBDevice.BUILTIN_DEFAULT is not None

# Each BUILTIN class-flag constant, if present, must have the expected bit value.
for name, expected in (("BUILTIN_CDC", 1), ("BUILTIN_MSC", 2), ("BUILTIN_NCM", 4)):
    val = getattr(machine.USBDevice, name, None)
    if val is not None:
        assert isinstance(val, int), "{} must be int".format(name)
        assert val == expected, "{} expected {} got {}".format(name, expected, val)

# Any two BUILTIN class-flag constants that are both present must have distinct bits.
flags = [
    getattr(machine.USBDevice, n, None) for n in ("BUILTIN_CDC", "BUILTIN_MSC", "BUILTIN_NCM")
]
present = [f for f in flags if f is not None]
if len(present) > 1:
    combined = 0
    for f in present:
        assert not (combined & f), "BUILTIN class-flag constants overlap"
        combined |= f

print("constants ok")

# Invalid mask tests only run when USB is inactive to avoid OSError(EINVAL).
if not usb.active():
    try:
        usb.builtin_driver = 0
        print("no raise for zero mask")
    except ValueError:
        print("zero mask raises ValueError")

    try:
        usb.builtin_driver = 0x100
        print("no raise for out-of-range")
    except ValueError:
        print("out-of-range raises ValueError")
