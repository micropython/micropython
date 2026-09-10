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

# Encrypt and decrypt without a nonce
ciphertext = a.encrypt(nonce, plaintext, None)
print(ciphertext)
print(a.decrypt(nonce, ciphertext, None))

# Encrypt and decrypt with an empty nonce
ciphertext = a.encrypt(nonce, plaintext, bytearray(0))
print(ciphertext)
print(a.decrypt(nonce, ciphertext, bytearray(0)))

# Encrypt and decrypt with a nonce
ciphertext = a.encrypt(nonce, plaintext, auth)
print(ciphertext)
print(a.decrypt(nonce, ciphertext, auth))

# Encrypt with specified output buffers
c_out = bytearray(len(plaintext) + 16)
p_out = bytearray(len(plaintext))
# Encrypt and decrypt with a nonce
a.encrypt(nonce, plaintext, auth, c_out)
print(c_out)
a.decrypt(nonce, c_out, auth, p_out)
print(p_out)

# Test with bad key length
try:
    a = aesgcm(nonce)
    print("FAIL: key length")
except ValueError:
    print("PASS: key length")

# Decrypt with corrupted ciphertext
c2 = bytearray(ciphertext)
c2[5] ^= 255
try:
    p2 = a.decrypt(nonce, c2, auth)
    print("FAIL: decrypt corrupted (no error)")
except ValueError as e:
    if "mbedtls err" in e.value:
        print("PASS: decrypt corrupted")
    else:
        print("FAIL: decrypt corrupted (bad error)")

# Decrypt with truncated ciphertext
try:
    p2 = a.decrypt(nonce, ciphertext[:8], auth)
    print("FAIL: decrypt truncated (no error)")
except ValueError as e:
    if "input too small" in e.value:
        print("PASS: decrypt truncated")
    else:
        print("FAIL: decrypt truncated (bad error)")

# Call with the wrong length of output buffer
try:
    a.encrypt(nonce, plaintext, auth, p_out)
    print("FAIL: encrypt to wrong sized buffer (no error)")
except ValueError as e:
    if e.value == "incorrect output length":
        print("PASS: encrypt to wrong sized buffer")
    else:
        print("FAIL: encrypt to wrong sized buffer (bad error)")

try:
    a.decrypt(nonce, ciphertext, auth, c_out)
    print("FAIL: decrypt to wrong sized buffer (no error)")
except ValueError as e:
    if e.value == "incorrect output length":
        print("PASS: decrypt to wrong sized buffer")
    else:
        print("FAIL: decrypt to wrong sized buffer (bad error)")
