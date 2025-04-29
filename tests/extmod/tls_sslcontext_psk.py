# Test for the PSK (Pre-Shared Key) functionality in tls.SSLContext

try:
    import tls
except ImportError:
    print("SKIP")
    raise SystemExit

# Create a TLS context
ctx = tls.SSLContext(tls.PROTOCOL_TLS_CLIENT)

# Test setting PSK and identity
identity = b"test-identity"
psk = b"test-preshared-key"

# Test the PSK functionality - this will throw an exception if PSK is not supported
try:
    ctx.set_psk_identity(identity)
    ctx.set_psk_key(psk)
    print("PSK successfully set")
except Exception as e:
    print("Failed to set PSK:", e)
    raise SystemExit

# Test setting PSK ciphersuites with error handling
# Use set_ciphers("PSK") to enable PSK mode
try:
    ctx.set_ciphers("PSK")
    print("PSK ciphersuites set")
except Exception as e:
    print("Failed to set PSK ciphersuites:", e)
    raise SystemExit

print("PSK test complete")
