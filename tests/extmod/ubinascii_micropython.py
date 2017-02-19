try:
    try:
        import ubinascii as binascii
    except ImportError:
        import binascii
except ImportError:
    import sys
    print("SKIP")
    sys.exit()

# two arguments supported in uPy but not CPython
a = binascii.hexlify(b'123', ':')
print(a)
