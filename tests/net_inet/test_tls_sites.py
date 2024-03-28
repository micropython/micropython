# Test making HTTPS requests to sites that may require advanced ciphers.

import sys
import select
import socket
import ssl

# Don't run if ssl doesn't support required certificates (eg axtls).
if not hasattr(ssl, "CERT_REQUIRED"):
    print("SKIP")
    raise SystemExit


def test_one(site, opts):
    ai = socket.getaddrinfo(site, 443, socket.AF_INET)
    addr = ai[0][-1]

    s = socket.socket(socket.AF_INET)

    # Create SSLContext.
    ssl_context = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)

    # CPython compatibility:
    # - disable check_hostname
    # - load default system certificate chain
    # - must wait for socket to be writable before calling wrap_socket
    if sys.implementation.name != "micropython":
        ssl_context.check_hostname = False
        ssl_context.load_default_certs()
        select.select([], [s], [])

    try:
        s.connect(addr)

        if "sni" in opts:
            s = ssl_context.wrap_socket(s, server_hostname=opts["host"])
        else:
            s = ssl_context.wrap_socket(s)

        s.write(b"GET / HTTP/1.0\r\nHost: %s\r\n\r\n" % bytes(site, "latin"))
        resp = s.read(4096)
        if resp[:7] != b"HTTP/1.":
            raise ValueError("response doesn't start with HTTP/1.")
        # print(resp)

    finally:
        s.close()


SITES = [
    "www.github.com",
    "micropython.org",
    "pypi.org",
    {"host": "api.pushbullet.com", "sni": True},
]


def main():
    for site in SITES:
        opts = {}
        if isinstance(site, dict):
            opts = site
            site = opts["host"]

        try:
            test_one(site, opts)
            print(site, "ok")
        except Exception as e:
            print(site, e)


main()
