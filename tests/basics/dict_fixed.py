# test that fixed dictionaries cannot be modified

try:
    import uerrno
except ImportError:
    print("SKIP")
    raise SystemExit

# Save a copy of uerrno.errorcode, so we can check later
# that it hasn't been modified.
errorcode_copy = uerrno.errorcode.copy()

try:
    uerrno.errorcode.popitem()
except TypeError:
    print("TypeError")

try:
    uerrno.errorcode.pop(0)
except TypeError:
    print("TypeError")

try:
    uerrno.errorcode.setdefault(0, 0)
except TypeError:
    print("TypeError")

try:
    uerrno.errorcode.update([(1, 2)])
except TypeError:
    print("TypeError")

try:
    del uerrno.errorcode[1]
except TypeError:
    print("TypeError")

try:
    uerrno.errorcode[1] = 'foo'
except TypeError:
    print("TypeError")

try:
    uerrno.errorcode.clear()
except TypeError:
    print("TypeError")

assert uerrno.errorcode == errorcode_copy
