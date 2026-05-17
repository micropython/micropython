# Test making HTTPS requests to sites that allow simple ciphers.

import sys
import socket
import ssl

SITES = (
    ("micropython.org", "/ks/test.html"),
    ("pypi.org", "/"),
)


def test_one(site, path):
    ai = socket.getaddrinfo(site, 443, socket.AF_INET)
    addr = ai[0][-1]

    # Create SSLContext.
    ssl_context = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
    if sys.implementation.name != "micropython":
        # CPython compatibility: disable check_hostname
        ssl_context.check_hostname = False
    ssl_context.verify_mode = ssl.CERT_NONE

    s = socket.socket(socket.AF_INET)
    s.connect(addr)
    s = ssl_context.wrap_socket(s)

    s.write(b"GET %s HTTP/1.0\r\nHost: %s\r\n\r\n" % (bytes(path, "ascii"), bytes(site, "ascii")))
    resp = s.read(4096)
    s.close()

    if resp.startswith(b"HTTP/1."):
        print(site, "ok")
    else:
        print(site, "response doesn't start with HTTP/1.")


def main():
    for site, path in SITES:
        test_one(site, path)


main()
