try:
    from Crypto.Cipher import AES
    aes = AES.new
except ImportError:
    try:
        from ucryptolib import aes
    except ImportError:
        print("SKIP")
        raise SystemExit

crypto = aes(b"1234" * 4, 2, b"5678" * 4)
enc = crypto.encrypt(bytes(range(32)))
print(enc)
crypto = aes(b"1234" * 4, 2, b"5678" * 4)
print(crypto.decrypt(enc))
