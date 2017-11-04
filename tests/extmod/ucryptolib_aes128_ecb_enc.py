# This tests minimal configuration of ucrypto module, which is
# AES128 encryption (anything else, including AES128 decryption,
# is optional).
try:
    from Crypto.Cipher import AES
    aes = AES.new
except ImportError:
    try:
        from ucryptolib import aes
    except ImportError:
        print("SKIP")
        raise SystemExit

crypto = aes(b"1234" * 4, 1)
enc = crypto.encrypt(bytes(range(32)))
print(enc)
