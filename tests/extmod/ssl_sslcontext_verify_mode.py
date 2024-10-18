# Test ssl.SSLContext.verify_mode attribute.

try:
    import ssl
except ImportError:
    print("SKIP")
    raise SystemExit

# Test default verify_mode for server (client default is different in MicroPython).
ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
print(ctx.verify_mode == ssl.CERT_NONE)

# Test setting and getting verify_mode.
ctx.verify_mode = ssl.CERT_NONE
print(ctx.verify_mode == ssl.CERT_NONE)
