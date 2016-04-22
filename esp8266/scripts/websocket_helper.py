import sys
try:
    import ubinascii as binascii
except:
    import binascii
try:
    import uhashlib as hashlib
except:
    import hashlib


def server_handshake(sock):
    clr = sock.makefile("rwb", 0)
    l = clr.readline()
    sys.stdout.write(repr(l))

    webkey = None

    while 1:
        l = clr.readline()
        if not l:
            raise OSError("EOF in headers")
        if l == b"\r\n":
            break
    #    sys.stdout.write(l)
        h, v = [x.strip() for x in l.split(b":", 1)]
        print((h, v))
        if h == b'Sec-WebSocket-Key':
            webkey = v

    if not webkey:
        raise OSError("Not a websocket request")

    print(webkey, len(webkey))

    respkey = webkey + b"258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
    respkey = hashlib.sha1(respkey).digest()
    print(repr(respkey))
    respkey = binascii.b2a_base64(respkey)[:-1]
    print(repr(respkey))

    resp = b"""\
HTTP/1.1 101 Switching Protocols\r
Upgrade: websocket\r
Connection: Upgrade\r
Sec-WebSocket-Accept: %s\r
\r
""" % respkey

    print(resp)
    sock.send(resp)


# Very simplified client handshake, works for MicroPython's
# websocket server implementation, but probably not for other
# servers.
def client_handshake(sock):
    cl = sock.makefile("rwb", 0)
    cl.write(b"""\
GET / HTTP/1.1\r
Host: echo.websocket.org\r
Connection: Upgrade\r
Upgrade: websocket\r
Sec-WebSocket-Key: foo\r
\r
""")
    l = cl.readline()
#    print(l)
    while 1:
        l = cl.readline()
        if l == b"\r\n":
            break
#        sys.stdout.write(l)
