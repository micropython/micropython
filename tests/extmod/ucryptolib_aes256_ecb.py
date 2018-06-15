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

crypto = aes(b"1234" * 8, MODE_ECB)
enc = crypto.encrypt(bytes(range(32)))
print(enc)
crypto = aes(b"1234" * 8, MODE_ECB)
print(crypto.decrypt(enc))
