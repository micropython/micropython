try:
    import ubinascii as binascii
except ImportError:
    import binascii

# two arguments supported in uPy but not CPython
a = binascii.hexlify(b'123', ':')
print(a)
