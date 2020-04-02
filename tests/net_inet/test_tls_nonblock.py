try:
    import usocket as socket, ussl as ssl, sys
except:
    import socket, ssl, sys, time, select


def test_one(site, opts):
    ai = socket.getaddrinfo(site, 443)
    addr = ai[0][-1]
    print(addr)

    use_send = "send" in opts and opts["send"]

    # Connect the raw socket
    s = socket.socket()
    s.setblocking(False)
    try:
        s.connect(addr)
        raise OSError(-1, "connect blocks")
    except OSError as e:
        if e.args[0] != 115:  # 115=EINPROGRESS
            raise

    if sys.implementation.name != "micropython":
        # in CPython we have to wait, otherwise wrap_socket is not happy
        select.select([], [s], [])

    try:
        # Wrap with SSL
        try:
            if sys.implementation.name == "micropython":
                s = ssl.wrap_socket(s, do_handshake=False)
            else:
                s = ssl.wrap_socket(s, do_handshake_on_connect=False)
        except OSError as e:
            if e.args[0] != 115:  # 115=EINPROGRESS
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
        if use_send:
            while len(out) > 0:
                try:
                    n = s.send(out)
                except OSError as e:
                    if e.args[0] != 11:  # 11=EAGAIN
                        raise
                    continue
                if n > 0:
                    out = out[n:]
                else:
                    raise OSError(-1, "unexpected write result")
        else:
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
            if use_send:
                try:
                    b = s.recv(128)
                except OSError as e:
                    if e.args[0] == 11:  # 11=EAGAIN
                        continue
                    if e.args[0] == 2:  # 2=ssl.SSL_ERROR_WANT_READ:
                        continue
                    raise
                if len(b) > 0:
                    if len(resp) < 1024:
                        resp += b
                elif len(b) == 0:
                    break
                else:
                    raise OSError(-1, "unexpected read result")
            else:
                try:
                    b = s.read(128)
                except OSError as err:
                    if err.args[0] == 2:  # 2=ssl.SSL_ERROR_WANT_READ:
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
    "google.com",
    {"host": "www.google.com", "send": True},
    "micropython.org",  # used in the built-in upip, it better work...
    "pypi.org",  # ditto
    "api.telegram.org",
    {"host": "api.pushbullet.com", "sni": True},
    # this no longer works, not sure which special case it is supposed to test...
    #    "w9rybpfril.execute-api.ap-southeast-2.amazonaws.com",
    #    {"host": "w9rybpfril.execute-api.ap-southeast-2.amazonaws.com", "sni": True},
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
    print("DONE")


main()
