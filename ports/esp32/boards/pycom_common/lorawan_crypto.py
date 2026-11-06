"""
LoRaWAN cryptographic primitives (AES-128-CMAC, CTR, key derivation).

Uses ucryptolib.aes (AES-128-ECB) as the only hardware dependency.
Implements CMAC (RFC 4493) and CTR mode in software for LoRaWAN MIC
calculation and payload encryption per LoRaWAN 1.0.x specification.
"""

import struct
from ucryptolib import aes

# AES block size
_BLK = 16


def _xor(a, b):
    """XOR two equal-length byte sequences."""
    return bytes(x ^ y for x, y in zip(a, b))


def _shift_left_1(b):
    """Left-shift a 16-byte block by 1 bit."""
    out = bytearray(_BLK)
    overflow = 0
    for i in range(_BLK - 1, -1, -1):
        out[i] = ((b[i] << 1) | overflow) & 0xFF
        overflow = 1 if b[i] & 0x80 else 0
    return bytes(out)


def _aes_ecb(key, block):
    """Encrypt a single 16-byte block with AES-128-ECB."""
    return aes(key, 1).encrypt(block)


def _cmac_subkeys(key):
    """Derive CMAC sub-keys K1, K2 from AES key (RFC 4493 section 2.3)."""
    L = _aes_ecb(key, b"\x00" * _BLK)
    K1 = _shift_left_1(L)
    if L[0] & 0x80:
        K1 = _xor(K1, b"\x00" * 15 + b"\x87")
    K2 = _shift_left_1(K1)
    if K1[0] & 0x80:
        K2 = _xor(K2, b"\x00" * 15 + b"\x87")
    return K1, K2


def aes_cmac(key, msg):
    """
    Compute AES-128-CMAC (RFC 4493).

    Args:
        key: 16-byte AES key
        msg: message bytes (any length)

    Returns:
        16-byte CMAC tag
    """
    K1, K2 = _cmac_subkeys(key)
    n = (len(msg) + _BLK - 1) // _BLK if msg else 1
    last_complete = len(msg) > 0 and len(msg) % _BLK == 0

    X = b"\x00" * _BLK
    for i in range(n - 1):
        block = msg[i * _BLK : (i + 1) * _BLK]
        X = _aes_ecb(key, _xor(X, block))

    # Last block
    if last_complete:
        block = msg[(n - 1) * _BLK : n * _BLK]
        block = _xor(block, K1)
    else:
        block = msg[(n - 1) * _BLK :]
        block = block + b"\x80" + b"\x00" * (_BLK - 1 - len(block))
        block = _xor(block, K2)

    return _aes_ecb(key, _xor(X, block))


def aes_ctr_encrypt(key, payload, dev_addr, fcnt, direction):
    """
    LoRaWAN payload encryption using AES-128 in CTR mode.

    Per LoRaWAN 1.0.x spec section 4.3.3.

    Args:
        key: 16-byte AppSKey (for FRMPayload) or NwkSKey (for FOpts)
        payload: plaintext bytes
        dev_addr: 4-byte device address (little-endian int)
        fcnt: frame counter (uint32)
        direction: 0 = uplink, 1 = downlink

    Returns:
        encrypted/decrypted bytes (symmetric)
    """
    k = (len(payload) + _BLK - 1) // _BLK
    S = bytearray()
    for i in range(1, k + 1):
        A_i = struct.pack("<BBBBIIBBBBB", 0x01, 0, 0, 0, direction, dev_addr, fcnt, 0, 0, 0, i)
        S += _aes_ecb(key, A_i)
    return _xor(payload, S[: len(payload)])


# Decryption is identical to encryption in CTR mode
aes_ctr_decrypt = aes_ctr_encrypt


def compute_mic(key, msg, dev_addr, fcnt, direction, data_len):
    """
    Compute LoRaWAN MIC (4-byte Message Integrity Code).

    Per LoRaWAN 1.0.x spec section 4.4.

    Args:
        key: 16-byte NwkSKey
        msg: MHDR | FHDR | FPort | FRMPayload (encrypted)
        dev_addr: device address (little-endian int)
        fcnt: frame counter (uint32)
        direction: 0 = uplink, 1 = downlink
        data_len: length of msg

    Returns:
        4-byte MIC
    """
    B0 = struct.pack("<BBBBIIBH", 0x49, 0, 0, 0, direction, dev_addr, fcnt, 0, data_len)
    cmac = aes_cmac(key, B0 + msg)
    return cmac[:4]


def derive_session_keys(app_key, app_nonce, net_id, dev_nonce):
    """
    Derive NwkSKey and AppSKey from OTAA join-accept parameters.

    Per LoRaWAN 1.0.x spec section 6.2.5.

    Args:
        app_key: 16-byte application key
        app_nonce: 3-byte AppNonce from join-accept
        net_id: 3-byte NetID from join-accept
        dev_nonce: 2-byte DevNonce used in join-request

    Returns:
        (nwk_skey, app_skey) tuple of 16-byte keys
    """
    # NwkSKey = aes128_encrypt(AppKey, 0x01 | AppNonce | NetID | DevNonce | pad16)
    nwk_input = b"\x01" + app_nonce + net_id + dev_nonce + b"\x00" * 7
    nwk_skey = _aes_ecb(app_key, nwk_input)

    # AppSKey = aes128_encrypt(AppKey, 0x02 | AppNonce | NetID | DevNonce | pad16)
    app_input = b"\x02" + app_nonce + net_id + dev_nonce + b"\x00" * 7
    app_skey = _aes_ecb(app_key, app_input)

    return nwk_skey, app_skey
