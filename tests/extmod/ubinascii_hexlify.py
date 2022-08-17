try:
    try:
        import ubinascii as binascii
    except ImportError:
        import binascii
except ImportError:
    print("SKIP")
    raise SystemExit

for x in (
    b"\x00\x01\x02\x03\x04\x05\x06\x07",
    b"\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f",
    b"\x7f\x80\xff",
    b"1234ABCDabcd",
):
    print(binascii.hexlify(x))

# Two-argument version (now supported in CPython)
print(binascii.hexlify(b"123", ":"))

# zero length buffer
print(binascii.hexlify(b"", b":"))
