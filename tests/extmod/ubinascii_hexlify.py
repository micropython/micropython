try:
    try:
        import ubinascii as binascii
    except ImportError:
        import binascii
except ImportError:
    print("SKIP")
    raise SystemExit

print(binascii.hexlify(b"\x00\x01\x02\x03\x04\x05\x06\x07"))
print(binascii.hexlify(b"\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"))
print(binascii.hexlify(b"\x7f\x80\xff"))
print(binascii.hexlify(b"1234ABCDabcd"))
try:
    binascii.hexlify("")
except TypeError:
    print("TypeError")
