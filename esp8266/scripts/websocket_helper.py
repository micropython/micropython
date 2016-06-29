import sys
try:
    import ubinascii as binascii
except:
    import binascii
try:
    import uhashlib as hashlib
except:
    import hashlib

DEBUG = 0

def server_handshake(sock):
    clr = sock.makefile("rwb", 0)
    l = clr.readline()
    #sys.stdout.write(repr(l))

    webkey = None

    while 1:
        l = clr.readline()
        if not l:
            raise OSError("EOF in headers")
        if l == b"\r\n":
            break
    #    sys.stdout.write(l)
        h, v = [x.strip() for x in l.split(b":", 1)]
        if DEBUG:
            print((h, v))
        if h == b'Sec-WebSocket-Key':
            webkey = v

    if not webkey:
        raise OSError("Not a websocket request")

    if DEBUG:
        print("Sec-WebSocket-Key:", webkey, len(webkey))

    d = hashlib.sha1(webkey)
    d.update(b"258EAFA5-E914-47DA-95CA-C5AB0DC85B11")
    respkey = d.digest()
    respkey = binascii.b2a_base64(respkey)[:-1]
    if DEBUG:
        print("respkey:", resp)

    sock.send(b"""\
HTTP/1.1 101 Switching Protocols\r
Upgrade: websocket\r
Connection: Upgrade\r
Sec-WebSocket-Accept: """)
    sock.send(respkey)
    sock.send("\r\n\r\n")


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
