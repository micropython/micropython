#!/usr/bin/env python3
#
# Generate an ESP32 digital-signature provisioning blob.
#
# This is a tool for provisioning an ESP32 board that uses
# the digital-signature peripheral. It reads:
# - a private RSA key in PEM format
# - the 32-byte HMAC key file that was burned into the target
#
# and writes the `ds_blob.bin` used with `esp32.DS.sign()`.
#
# See `tests/ports/esp32/esp32_ds.py` for a concrete example of the hardware
# flow.
#
# To create the HMAC key file:
#
#     head -c 32 /dev/random > ds_hmac_key.bin
#
# To generate a private RSA key for provisioning:
#
#     openssl genpkey -algorithm RSA -pkeyopt rsa_keygen_bits:2048 -out rsa_private.pem
#
# To burn the HMAC key onto the board, use:
#
#     espefuse.py --chip esp32s3 --port /dev/ttyACM0 burn_key \
#         BLOCK_KEY1 ds_hmac_key.bin HMAC_DOWN_DIGITAL_SIGNATURE
#
# The blob format depends on the RSA size supported by the selected target, so
# the script validates both the target and the key length before writing.
import argparse
import hashlib
import hmac
import os
import struct
import sys

from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes


SUPPORTED_TARGETS = {
    "esp32s2": [4096, 3072, 2048, 1024],
    "esp32c3": [3072, 2048, 1024],
    "esp32s3": [4096, 3072, 2048, 1024],
    "esp32c6": [3072, 2048, 1024],
    "esp32h2": [3072, 2048, 1024],
    "esp32p4": [4096, 3072, 2048, 1024],
    "esp32c5": [3072, 2048, 1024],
}

ESP_DS_IV_LEN = 16


def modinv(a: int, m: int) -> int:
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


def int_to_le_padded(n: int, pad_bytes: int) -> bytes:
    if n < 0:
        raise ValueError("negative integer not supported")
    if n == 0:
        raw = b"\x00"
    else:
        raw = n.to_bytes((n.bit_length() + 7) // 8, "little")
    if len(raw) > pad_bytes:
        raise ValueError(f"integer does not fit in {pad_bytes} bytes")
    return raw + (b"\x00" * (pad_bytes - len(raw)))


def load_rsa_private_key(path: str):
    with open(path, "rb") as f:
        pem = f.read()
    key = serialization.load_pem_private_key(pem, password=None, backend=default_backend())
    numbers = key.private_numbers()
    n = numbers.public_numbers.n
    d = numbers.d
    e = numbers.public_numbers.e
    return n, d, e


def read_exact(path: str, expected_len: int) -> bytes:
    with open(path, "rb") as f:
        data = f.read()
    if len(data) != expected_len:
        raise ValueError(f"{path}: expected {expected_len} bytes, got {len(data)}")
    return data


def rsa_bits_to_len_enum(bits: int) -> int:
    if bits not in (1024, 2048, 3072, 4096):
        raise ValueError(f"unsupported RSA size: {bits}")
    return bits // 32 - 1


def build_ds_blob(
    target: str, rsa_pem_path: str, hmac_key_path: str, out_path: str, iv_path: str | None
):
    if target not in SUPPORTED_TARGETS:
        raise ValueError(f"unsupported target: {target}")

    max_key_bits = max(SUPPORTED_TARGETS[target])
    max_key_bytes = max_key_bits // 8
    esp_ds_c_len = max_key_bytes * 3 + 32 + 8 + 8

    hmac_key = read_exact(hmac_key_path, 32)
    iv = read_exact(iv_path, ESP_DS_IV_LEN) if iv_path else os.urandom(ESP_DS_IV_LEN)

    n, d, e = load_rsa_private_key(rsa_pem_path)
    key_bits = n.bit_length()

    if key_bits not in SUPPORTED_TARGETS[target]:
        raise ValueError(f"RSA size {key_bits} is not supported on {target}")

    rsa_length = rsa_bits_to_len_enum(key_bits)

    y = int_to_le_padded(d, max_key_bytes)
    m = int_to_le_padded(n, max_key_bytes)

    rr = 1 << (key_bits * 2)
    rb_int = rr % n
    rb = int_to_le_padded(rb_int, max_key_bytes)

    mprime = (-modinv(n, 1 << 32)) & 0xFFFFFFFF
    length = rsa_length

    aes_key = hmac.new(hmac_key, b"\xff" * 32, hashlib.sha256).digest()

    md_in = y + m + rb + struct.pack("<II", mprime, length) + iv
    md = hashlib.sha256(md_in).digest()

    p = y + m + rb + md + struct.pack("<II", mprime, length) + (b"\x08" * 8)

    if len(p) != esp_ds_c_len:
        raise ValueError(f"internal error: plaintext length {len(p)} != {esp_ds_c_len}")

    cipher = Cipher(algorithms.AES(aes_key), modes.CBC(iv), backend=default_backend())
    encryptor = cipher.encryptor()
    c = encryptor.update(p) + encryptor.finalize()

    if len(c) != esp_ds_c_len:
        raise ValueError(f"internal error: ciphertext length {len(c)} != {esp_ds_c_len}")

    blob = struct.pack("<I", rsa_length) + iv + c

    with open(out_path, "wb") as f:
        f.write(blob)

    print(f"wrote {out_path}")
    print(f"  target        : {target}")
    print(f"  max_key_bits  : {max_key_bits}")
    print(f"  rsa_bits      : {key_bits}")
    print(f"  public_exp    : {e}")
    print(f"  rsa_length    : {rsa_length}")
    print(f"  iv            : {iv.hex()}")
    print(f"  c_len         : {len(c)}")
    print(f"  blob_len      : {len(blob)}")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--target", default="esp32s3", choices=sorted(SUPPORTED_TARGETS.keys()))
    ap.add_argument("--key", required=True, help="RSA private key PEM")
    ap.add_argument("--hmac", required=True, help="32-byte HMAC key file")
    ap.add_argument("--out", required=True, help="output ds_blob.bin")
    ap.add_argument("--iv", help="optional 16-byte IV file")
    args = ap.parse_args()

    try:
        build_ds_blob(args.target, args.key, args.hmac, args.out, args.iv)
    except Exception as e:
        print(f"error: {e}", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
