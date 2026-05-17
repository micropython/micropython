import ssl
import os
import asyncio

if not hasattr(ssl, "CERT_REQUIRED"):
    print("SKIP")
    raise SystemExit

# For details about this cert, see comment in test_sslcontext_client.py
root_cert_path = "isrgrootx1.der"

with open(root_cert_path, "rb") as ca:
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
