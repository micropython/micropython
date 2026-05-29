# Test the tls.SSLContext PSK attributes (psk_identity, psk_key,
# server_psk_keys) and basic validation, without needing a network connection.

try:
    import tls

    # PSK support is optional; psk_identity only exists when it's enabled.
    tls.SSLContext(tls.PROTOCOL_TLS_CLIENT).psk_identity
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


ctx = tls.SSLContext(tls.PROTOCOL_TLS_CLIENT)

# The PSK attributes default to None.
print(ctx.psk_identity, ctx.psk_key, ctx.server_psk_keys)

# They can be set and read back.
ctx.psk_identity = b"micropython"
ctx.psk_key = b"secret-shared-key"
ctx.server_psk_keys = {b"micropython": b"secret-shared-key"}
print(ctx.psk_identity, ctx.psk_key, ctx.server_psk_keys)

# A PSK key longer than mbedTLS's maximum (MBEDTLS_PSK_MAX_LEN) is rejected
# when it is set on the context (the identity is already set above).
try:
    ctx.psk_key = b"x" * 49
    print("no error")
except OSError:
    print("OSError")

# The key/identity are only applied once both are set, in either order.  Here
# the over-length key is set first (a no-op while the identity is None), so the
# rejection only surfaces when the identity is set and the pair is applied.
ctx = tls.SSLContext(tls.PROTOCOL_TLS_CLIENT)
ctx.psk_key = b"x" * 49
try:
    ctx.psk_identity = b"micropython"
    print("no error")
except OSError:
    print("OSError")
