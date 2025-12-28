try:
    from cryptography.hazmat.primitives.ciphers.aead import AESGCM

    aesgcm = AESGCM
except ImportError:
    try:
        from cryptolib import aesgcm
    except ImportError:
        print("SKIP")
        raise SystemExit

plaintext = b"This is a test message"
nonce = b"ABCDEFGHIJKL"
key = b"0123456789ABCDEF"
auth = b"Additional data"

a = aesgcm(key)

ciphertext = a.encrypt(nonce, plaintext, None)
print(ciphertext)
print(a.decrypt(nonce, ciphertext, None))

ciphertext = a.encrypt(nonce, plaintext, auth)
print(ciphertext)
print(a.decrypt(nonce, ciphertext, auth))
