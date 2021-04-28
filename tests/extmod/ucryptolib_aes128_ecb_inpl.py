# Inplace operations (input and output buffer is the same)
try:
    from ucryptolib import aes
except ImportError:
    print("SKIP")
    raise SystemExit

crypto = aes(b"1234" * 4, 1)
buf = bytearray(bytes(range(32)))
crypto.encrypt(buf, buf)
print(buf)

crypto = aes(b"1234" * 4, 1)
crypto.decrypt(buf, buf)
print(buf)
