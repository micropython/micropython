import ssl
import os
import asyncio

if not hasattr(ssl, "CERT_REQUIRED"):
    print("SKIP")
    raise SystemExit

# This certificate was obtained from micropython.org using openssl:
# $ openssl s_client -showcerts -connect micropython.org:443 </dev/null 2>/dev/null
# The certificate is from Let's Encrypt:
# 1 s:/C=US/O=Let's Encrypt/CN=R3
#   i:/C=US/O=Internet Security Research Group/CN=ISRG Root X1
# Validity
#            Not Before: Sep  4 00:00:00 2020 GMT
#            Not After : Sep 15 16:00:00 2025 GMT
# Copy PEM content to a file (certmpy.pem) and convert to DER e.g.
# $ openssl x509 -in certmpy.pem -out certmpy.der -outform DER
# Then convert to hex format, eg using binascii.hexlify(data).

# Note that the instructions above is to obtain an intermediate
# root CA cert that works for MicroPython. However CPython needs the ultimate root CA
# cert from ISRG

ca_cert_chain = "isrg.der"

with open(ca_cert_chain, "rb") as ca:
    cadata = ca.read()

client_ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
client_ctx.verify_mode = ssl.CERT_REQUIRED
client_ctx.load_verify_locations(cadata=cadata)


async def http_get(url, port, sslctx):
    reader, writer = await asyncio.open_connection(url, port, ssl=sslctx)

    print("write GET")
    writer.write(b"GET / HTTP/1.0\r\n\r\n")
    await writer.drain()

    print("read response")
    while True:
        data = await reader.readline()
        # avoid printing datetime which makes the test fail
        if b"GMT" not in data:
            print("read:", data)
        if not data:
            break

    print("close")
    writer.close()
    await writer.wait_closed()
    print("done")


asyncio.run(http_get("micropython.org", 443, client_ctx))
