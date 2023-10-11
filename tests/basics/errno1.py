# test errno's and errno module

try:
    import errno
except ImportError:
    print("SKIP")
    raise SystemExit

# check that constants exist and are integers
print(type(errno.EIO))

# check that errors are rendered in a nice way
msg = str(OSError(errno.EIO))
print(msg[:7], msg[-5:])
msg = str(OSError(errno.EIO, "details"))
print(msg[:7], msg[-14:])
msg = str(OSError(errno.EIO, "details", "more details"))
print(msg[:1], msg[-28:])

# check that unknown errno is still rendered
print(str(OSError(9999)))

# this tests a failed constant lookup in errno
errno = errno
print(errno.__name__)
