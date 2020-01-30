try:
    try:
        import ubinascii as binascii
    except ImportError:
        import binascii
except ImportError:
    print("SKIP")
    raise SystemExit

# two arguments supported in uPy but not CPython
a = binascii.hexlify(b'123', ':')
print(a)

# zero length buffer
print(binascii.hexlify(b'', b':'))

# base64 extensions
print(binascii.b2a_base64(b"foo",binascii.ADD_NEWLINE))
print(binascii.b2a_base64(b"foo",0))
print(binascii.b2a_base64(b"foox",binascii.URL))
print(binascii.b2a_base64(b"foo",binascii.URL | binascii.NOPADDING))

print(binascii.a2b_base64(b"_-a",binascii.URL | binascii.NOPADDING))
