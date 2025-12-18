import socket, ssl, errno, sys, time, select

# Although this test doesn't need ssl.CERT_REQUIRED, it does require the ssl module
# to support modern ciphers.  So exclude the test on axTLS which doesn't have
# CERT_REQUIRED.
if not hasattr(ssl, "CERT_REQUIRED"):
    print("SKIP")
    raise SystemExit


def test_one(site, opts):
    ai = socket.getaddrinfo(site, 443, socket.AF_INET)
    addr = ai[0][-1]
    print(site)

    # Connect the raw socket
    s = socket.socket(socket.AF_INET)
    s.setblocking(False)
    try:
        s.connect(addr)
        raise OSError(-1, "connect blocks")
    except OSError as e:
        if e.errno != errno.EINPROGRESS:
            raise

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
        # Wrap with SSL
        try:
            s = ssl_context.wrap_socket(s, do_handshake_on_connect=False)
        except OSError as e:
            if e.errno != errno.EINPROGRESS:
                raise
        print("wrapped")

        # CPython needs to be told to do the handshake
        if sys.implementation.name != "micropython":
            while True:
                try:
                    s.do_handshake()
                    break
                except ssl.SSLError as err:
                    if err.args[0] == ssl.SSL_ERROR_WANT_READ:
                        select.select([s], [], [])
                    elif err.args[0] == ssl.SSL_ERROR_WANT_WRITE:
                        select.select([], [s], [])
                    else:
                        raise
                time.sleep(0.1)
            # print("shook hands")

        # Write HTTP request
        out = b"GET / HTTP/1.0\r\nHost: %s\r\n\r\n" % bytes(site, "latin")
        while len(out) > 0:
            n = s.write(out)
            if n is None:
                continue
            if n > 0:
                out = out[n:]
            elif n == 0:
                raise OSError(-1, "unexpected EOF in write")
        print("wrote")

        # Read response
        resp = b""
        while True:
            try:
                b = s.read(128)
            except OSError as err:
                if err.errno == 2:  # 2=ssl.SSL_ERROR_WANT_READ:
                    continue
                raise
            if b is None:
                continue
            if len(b) > 0:
                if len(resp) < 1024:
                    resp += b
            elif len(b) == 0:
                break
        print("read")

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
            print(site, "error", e)
    print("DONE")


main()
