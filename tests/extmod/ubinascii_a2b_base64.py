try:
    try:
        import ubinascii as binascii
    except ImportError:
        import binascii
except ImportError:
    print("SKIP")
    raise SystemExit

print(binascii.a2b_base64(b''))
print(binascii.a2b_base64(b'Zg=='))
print(binascii.a2b_base64(b'Zm8='))
print(binascii.a2b_base64(b'Zm9v'))
print(binascii.a2b_base64(b'Zm9vYg=='))
print(binascii.a2b_base64(b'Zm9vYmE='))
print(binascii.a2b_base64(b'Zm9vYmFy'))

print(binascii.a2b_base64(b'AAECAwQFBgc='))
print(binascii.a2b_base64(b'CAkKCwwNDg8='))
print(binascii.a2b_base64(b'f4D/'))
print(binascii.a2b_base64(b'f4D+')) # convert '+'
print(binascii.a2b_base64(b'MTIzNEFCQ0RhYmNk'))

# Ignore invalid characters and pad sequences
print(binascii.a2b_base64(b'Zm9v\n'))
print(binascii.a2b_base64(b'Zm\x009v\n'))
print(binascii.a2b_base64(b'Zm9v=='))
print(binascii.a2b_base64(b'Zm9v==='))
print(binascii.a2b_base64(b'Zm9v===YmFy'))

try:
    print(binascii.a2b_base64(b'abc'))
except ValueError:
    print("ValueError")
try:
    print(binascii.a2b_base64(b'abcde='))
except ValueError:
    print("ValueError")
try:
    print(binascii.a2b_base64(b'ab*d'))
except ValueError:
    print("ValueError")
try:
    print(binascii.a2b_base64(b'ab=cdef='))
except ValueError:
    print("ValueError")
