# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2026 Nicko van Someren
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

# net_https.py — fetch https://www.google.com/ over TLS, driven by asyncio.
#   exec(open('/samples/net_https.py').read())
# Needs networking — launch with `make run`.
#
# This exercises the in-app mbedTLS backend (TLS=mbedtls) end to end: asyncio's
# open_connection(ssl=...) wraps the non-blocking TCP socket, the TLS handshake is
# pumped by the event loop, and the HTTP exchange rides the encrypted stream.

import network
import tls
import asyncio

HOST = "www.google.com"
PATH = "/"


async def fetch():
    nic = network.LAN()
    nic.active(True)
    nic.ipconfig(dhcp4=True)
    for _ in range(75):
        if nic.isconnected():
            break
        await asyncio.sleep_ms(200)
    if not nic.isconnected():
        raise OSError("no network — see net_info.py")
    # Inside the dev container SLIRP's DNS forwarder can't reach an upstream
    # resolver; point at a public one. On real hardware the DHCP DNS works too.
    network.ipconfig(dns="8.8.8.8")

    # There is no system CA bundle in the firmware, so we don't verify the chain
    # here (CERT_NONE). SNI (server_hostname) is still sent, so Google returns the
    # right certificate/vhost. To verify for real, load a CA with
    # ctx.load_verify_locations(cadata=...) and set CERT_REQUIRED.
    ctx = tls.SSLContext(tls.PROTOCOL_TLS_CLIENT)
    ctx.verify_mode = tls.CERT_NONE

    reader, writer = await asyncio.open_connection(HOST, 443, ssl=ctx, server_hostname=HOST)

    req = (
        b"GET "
        + PATH.encode()
        + b" HTTP/1.1\r\nHost: "
        + HOST.encode()
        + b"\r\nConnection: close\r\n\r\n"
    )
    writer.write(req)
    await writer.drain()

    # Read the status line + headers.
    buf = b""
    while b"\r\n\r\n" not in buf:
        chunk = await reader.read(512)
        if not chunk:
            break
        buf += chunk
    status = buf.split(b"\r\n", 1)[0]
    print("TLS:", ctx, "->", status.decode("utf-8", "replace"))

    # Drain the body so we can report the size, then close.
    total = len(buf)
    while True:
        chunk = await reader.read(512)
        if not chunk:
            break
        total += len(chunk)
    writer.close()
    await writer.wait_closed()
    print("received %d bytes from https://%s%s" % (total, HOST, PATH))


asyncio.run(fetch())
