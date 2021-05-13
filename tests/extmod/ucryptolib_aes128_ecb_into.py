# Operations with pre-allocated output buffer
try:
    from ucryptolib import aes
except ImportError:
    print("SKIP")
    raise SystemExit

crypto = aes(b"1234" * 4, 1)
enc = bytearray(32)
crypto.encrypt(bytes(range(32)), enc)
print(enc)

crypto = aes(b"1234" * 4, 1)
dec = bytearray(32)
crypto.decrypt(enc, dec)
print(dec)
