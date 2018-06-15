# This tests minimal configuration of ucrypto module, which is
# AES128 encryption (anything else, including AES128 decryption,
# is optional).
try:
    from Crypto.Cipher import AES
    aes = AES.new
    MODE_ECB = AES.MODE_ECB
except ImportError:
    try:
        from ucryptolib import aes, MODE_ECB
    except ImportError:
        print("SKIP")
        raise SystemExit

crypto = aes(b"1234" * 4, MODE_ECB)
enc = crypto.encrypt(bytes(range(32)))
print(enc)
