# Test ssl.SSLContext.verify_mode attribute.
# It's not available in the axtls implementation, so has an independent test.

try:
    import ssl
except ImportError:
    print("SKIP")
    raise SystemExit

if not hasattr(ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT), "verify_mode"):
    print("SKIP")
    raise SystemExit

# Test default verify_mode for server (client default is different in MicroPython).
ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
print(ctx.verify_mode == ssl.CERT_NONE)

# Test setting and getting verify_mode.
ctx.verify_mode = ssl.CERT_NONE
print(ctx.verify_mode == ssl.CERT_NONE)
ctx.verify_mode = ssl.CERT_OPTIONAL
print(ctx.verify_mode == ssl.CERT_OPTIONAL)
ctx.verify_mode = ssl.CERT_REQUIRED
print(ctx.verify_mode == ssl.CERT_REQUIRED)
