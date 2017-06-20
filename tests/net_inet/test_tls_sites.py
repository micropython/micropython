try:
    import usocket as _socket
except:
    import _socket
try:
    import ussl as ssl
except:
    import ssl


def test_one(site, opts):
    ai = _socket.getaddrinfo(site, 443)
    addr = ai[0][-1]

    s = _socket.socket()

    try:
        s.connect(addr)

        if "sni" in opts:
            s = ssl.wrap_socket(s, server_hostname=opts["host"])
        else:
            s = ssl.wrap_socket(s)

        s.write(b"GET / HTTP/1.0\r\n\r\n")
        resp = s.read(4096)
#        print(resp)

    finally:
        s.close()


SITES = [
    "google.com",
    "www.google.com",
    "api.telegram.org",
#    "w9rybpfril.execute-api.ap-southeast-2.amazonaws.com",
    {"host": "w9rybpfril.execute-api.ap-southeast-2.amazonaws.com", "sni": True},
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
            print(site, repr(e))


main()
