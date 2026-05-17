# Test ssl.SSLContext.verify_mode attribute, with additional CERT_xxx constants.
# These aren't available in the axtls implementation.

try:
    import ssl
except ImportError:
    print("SKIP")
    raise SystemExit

if not hasattr(ssl, "OPENSSL_VERSION") and not hasattr(ssl, "MBEDTLS_VERSION"):
    print("SKIP")
    raise SystemExit

ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)

# Test setting and getting verify_mode.
ctx.verify_mode = ssl.CERT_OPTIONAL
print(ctx.verify_mode == ssl.CERT_OPTIONAL)
ctx.verify_mode = ssl.CERT_REQUIRED
print(ctx.verify_mode == ssl.CERT_REQUIRED)
