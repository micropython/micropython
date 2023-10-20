# test that fixed dictionaries cannot be modified

try:
    import errno
except ImportError:
    print("SKIP")
    raise SystemExit

# Save a copy of errno.errorcode, so we can check later
# that it hasn't been modified.
errorcode_copy = errno.errorcode.copy()

try:
    errno.errorcode.popitem()
except TypeError:
    print("TypeError")

try:
    errno.errorcode.pop(0)
except TypeError:
    print("TypeError")

try:
    errno.errorcode.setdefault(0, 0)
except TypeError:
    print("TypeError")

try:
    errno.errorcode.update([(1, 2)])
except TypeError:
    print("TypeError")

try:
    del errno.errorcode[1]
except TypeError:
    print("TypeError")

try:
    errno.errorcode[1] = 'foo'
except TypeError:
    print("TypeError")

try:
    errno.errorcode.clear()
except TypeError:
    print("TypeError")

assert errno.errorcode == errorcode_copy
