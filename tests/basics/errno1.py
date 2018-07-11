# test errno's and uerrno module

try:
    import uerrno
except ImportError:
    print("SKIP")
    raise SystemExit

# check that constants exist and are integers
print(type(uerrno.EIO))

# check that errors are rendered in a nice way
msg = str(OSError(uerrno.EIO))
print(msg[:7], msg[msg.find(']'):])

msg = str(OSError(uerrno.ENOBUFS))
print(msg[:7], msg[msg.find(']'):])

# check that unknown errno is still rendered
print(str(OSError(9999)))

# this tests a failed constant lookup in errno
errno = uerrno
print(errno.__name__)
