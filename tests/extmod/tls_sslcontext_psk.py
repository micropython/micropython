# Test for the PSK (Pre-Shared Key) functionality in tls.SSLContext

try:
    import tls
except ImportError:
    print("SKIP")
    raise SystemExit

# Create a TLS context
ctx = tls.SSLContext(tls.PROTOCOL_TLS_CLIENT)

# Test setting PSK and identity
psk = b"test-preshared-key"
identity = b"test-identity"

ctx.set_psk(psk, identity)

# Test setting PSK ciphersuites
# This list contains common PSK ciphersuites
ctx.set_psk_ciphersuites([
    "TLS-PSK-WITH-AES-128-CBC-SHA256",
    "TLS-PSK-WITH-AES-128-GCM-SHA256"
])

print("PSK test complete")