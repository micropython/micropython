try:
    try:
        import ubinascii as binascii
    except ImportError:
        import binascii
except ImportError:
    print("SKIP")
    raise SystemExit

# two arguments supported in uPy but not CPython
a = binascii.hexlify(b"123", ":")
print(a)

# zero length buffer
print(binascii.hexlify(b"", b":"))
