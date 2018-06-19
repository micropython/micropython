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
crypto = aes(b"1234" * 4, 1)
print(crypto.decrypt(enc))
