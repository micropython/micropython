# Test the ESP32-S3 digital signature peripheral.
#
# The target must be built with MICROPY_PY_ESP32_DS=1. If the feature is not
# enabled on the board under test, the script prints SKIP and exits.
#
# The fixed HMAC key used here must be provisioned into KEY1 without read or
# write protection so the test remains repeatable.

try:
    import uhashlib as hashlib
except ImportError:
    import hashlib

try:
    import ubinascii as binascii
except ImportError:
    import binascii

try:
    from cryptolib import aes
except ImportError:
    print("SKIP")
    raise SystemExit

import struct

try:
    import esp32
except ImportError:
    print("SKIP")
    raise SystemExit


RSA_BYTES = 256
MAX_KEY_BYTES = 512
MESSAGE = b"esp32.DS test payload"
HMAC_KEY = bytes.fromhex("00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff")
IV = bytes.fromhex("0102030405060708090a0b0c0d0e0f10")

PRIVATE_KEY_PEM = """-----BEGIN PRIVATE KEY-----
MIIEvAIBADANBgkqhkiG9w0BAQEFAASCBKYwggSiAgEAAoIBAQCJnKLi77098HWx
K6rOQDeaInY2srstEMj30CrhcPUeeUJFKgowdPtlm55EqzEomowX5U0UD4M0I2OT
UR1FUfkWCT3IiVQVYecXgyMbCTiHAkdn2ztTanLUiCDzTJks97MM1lcQAxKfyFtp
inAvB43kKNE8/2lvM3In6xmZEjRJgjBduayYYphAMIaxctc+54tEUGH8yWxhNLYg
kNiDVXC8F2A1XKzBp6wXzARUoHRYo2MdnwGYPJfk41h9u8lpoXMA+O6Gm83IijdZ
BFhJw78tXZaN937b/mQlnNgxW+QhO4/7NsoQl8R4Xb34nHnmo68/napeNR04GEkO
JznRQ1HTAgMBAAECggEALjJgWFDlNdLwn66qI51ZTSw+hTRRM9rLrK28h3w1Paq1
faSUURokf0LTyfeyhBAF8cuvqYlfoXQz7HOxoODY/vXRQO4hRSBdxP3oHh5lVKWG
NS5a5zD4JbMhZKwVTAxHloqqJZzydqN7VJ399TvS/YJaY+DWk1cw/oelS0baOMa2
0pkHB64YhhW/4DMXYlbvdcDTa1+kr4+JyFClMCI6/T/mdwTro7OZiRVD5w3YDtMM
xRhaHpaY7dWi0e2tu9XohPMsYFjs+6QRGIacFaeZdIkZgsDjpqA6o/lRQMcrvum7
i+l0w0/7yRe0EX9H+YUSQ8pwSzKFG95DKl25p67KwQKBgQC8RLHi5kF6J+t5sflq
Wu4slQbwplgNudoqCY39qdb5n+5Co/f3t1DfuJz0DHb3pyISHIM+ddjlaIUYL+uL
WQ1ZrdwDL2cN6fa9fcdyUHIOmgxD+N20H4auYMOSTjjgx/ez+nmpvq/m7io6V0YO
3HHv/LiHXr+oV3Czm51pbc3QswKBgQC7HokxvzHn26rIiIWFE4+aOply3wv8a/BC
8uB9CEzTS439aE2CYeJhCPD8kSsReRfmhesfOJUP4bK1Rb53gUChhhTdDrMcXiOf
sHHYjgFmtakCe6kARz/mxtbExmEzVXpVQJ6WAemNK5WmIM3XxHLXKbT5mhD6QS5i
cLN+Sv7KYQKBgADWvYQpSnlk3CO6q0XSCeWg9Fr2IsZM4a/2Qu+yZgbOs50QZfZw
lqeohup/c7g+wmQaRGtu0vySrqUg8Ye5adnQcH6DI45oUHUfrlfQC/IMtalH3pUC
3vK/858fQhmeSng+0XP0KYx52y5PXTyLtuY+1gOkZG43lYT3WIgzuQsJAoGAF37F
EUcFmCxf7GAgC86h+GalP+Q72A0hlPZ0M57oLpvND2WaXeW2jCKYR85ejwoacyqb
lEcBiIX5b4N7X/wNxnpUglFHMier53A7S0rBaEklGJe4Z72Ki4qqCvkx2UVBYBUG
FKXW0pAHcV2rGw2isdudyr2KpjGrBldKYnA+P+ECgYAesmbRIpPMmI6XEtUOuAXH
LKSogialcAaV4EUwJVASQFNi+t3bwRwFPauNEJp7VSrTY8MefOieaowgE4qRV/sX
vP+RHvEaq7oR0KP8qZxLtwT8wkynAjfSj3ntvuC7R3eGS5tX4SpcBNK9hHfKj9CC
MPYp3Xz+OVuqc4/frJ/G8w==
-----END PRIVATE KEY-----"""

SHA256_DIGESTINFO_PREFIX = bytes(
    (
        0x30,
        0x31,
        0x30,
        0x0D,
        0x06,
        0x09,
        0x60,
        0x86,
        0x48,
        0x01,
        0x65,
        0x03,
        0x04,
        0x02,
        0x01,
        0x05,
        0x00,
        0x04,
        0x20,
    )
)


def sha256(data):
    h = hashlib.sha256()
    h.update(data)
    return h.digest()


def hmac_sha256(key, msg):
    block_size = 64
    if len(key) > block_size:
        key = sha256(key)
    key = key + (b"\x00" * (block_size - len(key)))
    o_key = bytes((b ^ 0x5C) for b in key)
    i_key = bytes((b ^ 0x36) for b in key)
    return sha256(o_key + sha256(i_key + msg))


def int_to_le_padded(n, pad_bytes):
    if n < 0:
        raise ValueError("negative integer not supported")
    if n == 0:
        raw = b"\x00"
    else:
        raw = n.to_bytes((int_bit_length(n) + 7) // 8, "little")
    if len(raw) > pad_bytes:
        raise ValueError("integer does not fit")
    return raw + (b"\x00" * (pad_bytes - len(raw)))


def modinv(a, m):
    t, new_t = 0, 1
    r, new_r = m, a % m
    while new_r != 0:
        q = r // new_r
        t, new_t = new_t, t - q * new_t
        r, new_r = new_r, r - q * new_r
    if r != 1:
        raise ValueError("inverse does not exist")
    if t < 0:
        t += m
    return t


def reverse_bytes(b):
    n = len(b)
    out = bytearray(n)
    i = 0
    while i < n:
        out[i] = b[n - 1 - i]
        i += 1
    return bytes(out)


def int_bit_length(x):
    n = 0
    while x:
        x >>= 1
        n += 1
    return n


def der_read_len(buf, i):
    first = buf[i]
    i += 1
    if first < 0x80:
        return first, i
    n = first & 0x7F
    if n == 0 or n > 4:
        raise ValueError("bad DER length")
    val = 0
    for _ in range(n):
        val = (val << 8) | buf[i]
        i += 1
    return val, i


def der_read_tlv(buf, i, expected_tag=None):
    tag = buf[i]
    i += 1
    length, i = der_read_len(buf, i)
    start = i
    end = i + length
    if end > len(buf):
        raise ValueError("truncated DER")
    if expected_tag is not None and tag != expected_tag:
        raise ValueError("unexpected DER tag")
    return tag, start, end


def der_read_integer(buf, i):
    _, start, end = der_read_tlv(buf, i, 0x02)
    val = buf[start:end]
    while len(val) > 1 and val[0] == 0x00:
        val = val[1:]
    return int.from_bytes(val, "big"), end


def parse_rsa_private_key_from_pem(pem_text):
    body = []
    for line in pem_text.splitlines():
        if line and not line.startswith("-----"):
            body.append(line.strip())
    der = binascii.a2b_base64("".join(body))

    _, pos, top_e = der_read_tlv(der, 0, 0x30)
    _, _, pos = der_read_tlv(der, pos, 0x02)  # version
    _, _, pos = der_read_tlv(der, pos, 0x30)  # algorithm identifier
    _, key_s, key_e = der_read_tlv(der, pos, 0x04)
    if key_e != top_e:
        raise ValueError("unexpected trailing data in private key")

    rsa_der = der[key_s:key_e]
    _, rsa_s, rsa_e = der_read_tlv(rsa_der, 0, 0x30)
    pos = rsa_s
    _, pos = der_read_integer(rsa_der, pos)  # version
    n, pos = der_read_integer(rsa_der, pos)
    e, pos = der_read_integer(rsa_der, pos)
    d, pos = der_read_integer(rsa_der, pos)
    while pos != rsa_e:
        _, pos = der_read_integer(rsa_der, pos)
    return n, d, e


def build_ds_blob(n, d, key_bits, rsa_length, iv, hmac_key):
    y = int_to_le_padded(d, MAX_KEY_BYTES)
    m = int_to_le_padded(n, MAX_KEY_BYTES)
    rb = int_to_le_padded((1 << (key_bits * 2)) % n, MAX_KEY_BYTES)
    mprime = (-modinv(n, 1 << 32)) & 0xFFFFFFFF
    aes_key = hmac_sha256(hmac_key, b"\xff" * 32)
    md_in = y + m + rb + struct.pack("<II", mprime, rsa_length) + iv
    md = sha256(md_in)
    p = y + m + rb + md + struct.pack("<II", mprime, rsa_length) + (b"\x08" * 8)
    cipher = aes(aes_key, 2, iv)
    c = cipher.encrypt(p)
    return struct.pack("<I", rsa_length) + iv + c


def emsa_pkcs1_v1_5_sha256(message, k):
    digest = sha256(message)
    t = SHA256_DIGESTINFO_PREFIX + digest
    if k < len(t) + 11:
        raise ValueError("RSA modulus too short for PKCS#1 v1.5 SHA-256")
    ps = b"\xff" * (k - len(t) - 3)
    return b"\x00\x01" + ps + b"\x00" + t


if not hasattr(esp32, "DS"):
    print("SKIP")
    raise SystemExit

n, d, e = parse_rsa_private_key_from_pem(PRIVATE_KEY_PEM)
rsa_bytes = (int_bit_length(n) + 7) // 8
if rsa_bytes != RSA_BYTES:
    raise ValueError("unexpected RSA size: %d" % rsa_bytes)

blob = build_ds_blob(n, d, int_bit_length(n), rsa_bytes // 4 - 1, IV, HMAC_KEY)
if len(blob) != 1604:
    raise ValueError("unexpected DS blob length: %d" % len(blob))

ds = esp32.DS()
message = reverse_bytes(emsa_pkcs1_v1_5_sha256(MESSAGE, RSA_BYTES))
sig_le = ds.sign(message, blob, esp32.DS.KEY1)

if len(sig_le) != RSA_BYTES:
    raise ValueError("unexpected signature length: %d != %d" % (len(sig_le), RSA_BYTES))

recovered = pow(int.from_bytes(sig_le, "little"), e, n).to_bytes(RSA_BYTES, "big")
expected = emsa_pkcs1_v1_5_sha256(MESSAGE, RSA_BYTES)
if recovered != expected:
    print("DS TEST FAILED")
    print("expected block prefix:", expected[:32].hex())
    print("got block prefix     :", recovered[:32].hex())
    raise ValueError("signature verification failed")

print("DS TEST PASSED")
