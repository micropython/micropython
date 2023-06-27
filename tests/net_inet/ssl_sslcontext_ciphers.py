import ssl

ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)

ciphers = ctx.get_ciphers()

for ci in ciphers:
    print(ci)

ctx.set_ciphers(ciphers[:1])

# errors
try:
    ctx.set_ciphers("FOO")

except Exception as e:
    print(e)


try:
    ctx.set_ciphers(["BAR"])

except Exception as e:
    print(e)
