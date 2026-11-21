# Test LoRaWAN cryptographic primitives.
# These tests use known test vectors from the LoRaWAN specification.
# Requires ucryptolib (AES-128-ECB) which is available on ESP32.

try:
    from ucryptolib import aes
except ImportError:
    print("SKIP")
    raise SystemExit

try:
    from machine import Pin

    Pin("LORA_CS")
except (ImportError, ValueError):
    print("SKIP")
    raise SystemExit

from lorawan_crypto import (
    aes_cmac,
    aes_ctr_encrypt,
    aes_ctr_decrypt,
    compute_mic,
    derive_session_keys,
)

# Test 1: AES-CMAC with RFC 4493 test vectors
# Key = 2b7e1516 28aed2a6 abf71588 09cf4f3c
key = bytes.fromhex("2b7e151628aed2a6abf7158809cf4f3c")

# Empty message
mac = aes_cmac(key, b"")
print("cmac empty:", mac.hex()[:8])

# 16-byte message
msg16 = bytes.fromhex("6bc1bee22e409f96e93d7e117393172a")
mac = aes_cmac(key, msg16)
print("cmac 16b:", mac.hex()[:8])

# 40-byte message
msg40 = bytes.fromhex(
    "6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411"
)
mac = aes_cmac(key, msg40)
print("cmac 40b:", mac.hex()[:8])

# Test 2: CTR encrypt/decrypt round-trip
app_skey = bytes(16)  # all zeros for test
payload = b"Hello LoRaWAN"
dev_addr = 0x01020304
fcnt = 1

encrypted = aes_ctr_encrypt(app_skey, payload, dev_addr, fcnt, 0)
decrypted = aes_ctr_decrypt(app_skey, encrypted, dev_addr, fcnt, 0)
print("ctr roundtrip:", decrypted == payload)
print("ctr differs:", encrypted != payload)

# Test 3: MIC computation
nwk_skey = bytes(16)
msg = b"\x40\x04\x03\x02\x01\x00\x01\x00\x01" + b"test"
mic = compute_mic(nwk_skey, msg, 0x01020304, 1, 0, len(msg))
print("mic length:", len(mic))
print("mic type:", type(mic).__name__)

# Test 4: Session key derivation
app_key = bytes.fromhex("2b7e151628aed2a6abf7158809cf4f3c")
app_nonce = bytes(3)
net_id = bytes(3)
dev_nonce = bytes(2)
nwk_s, app_s = derive_session_keys(app_key, app_nonce, net_id, dev_nonce)
print("nwk_skey len:", len(nwk_s))
print("app_skey len:", len(app_s))
print("keys differ:", nwk_s != app_s)

# Test 5: Verify CMAC deterministic
mac1 = aes_cmac(key, b"test")
mac2 = aes_cmac(key, b"test")
print("cmac deterministic:", mac1 == mac2)

print("all tests passed")
