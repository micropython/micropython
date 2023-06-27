# Basic test of ssl.SSLContext get_ciphers() and set_ciphers() methods.

try:
    import ssl
except ImportError:
    print("SKIP")
    raise SystemExit


ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)

ciphers = ctx.get_ciphers()

for ci in ciphers:
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
