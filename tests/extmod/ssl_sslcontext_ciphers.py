# Basic test of ssl.SSLContext get_ciphers() and set_ciphers() methods.

try:
    import ssl
except ImportError:
    print("SKIP")
    raise SystemExit


ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)

ciphers = ctx.get_ciphers()

for ci in ciphers:
    # Only print those ciphers know to exist on all ports.
    if ("TLS-ECDHE-ECDSA-WITH-AES" in ci or "TLS-RSA-WITH-AES" in ci) and "CBC" in ci:
        print(ci)

ctx.set_ciphers(ciphers[:1])

# Test error cases.

try:
    ctx.set_ciphers(ciphers[0])
except TypeError as e:
    print(e)

try:
    ctx.set_ciphers(["BAR"])
except OSError as e:
    print(e)
