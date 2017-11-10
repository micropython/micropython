import sys
import urandom
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

    webkey = _extract_header_values(clr, (b'sec-websocket-key',))[0]
    if not webkey:
        raise OSError("Not a websocket request")
    if DEBUG:
        print("Sec-WebSocket-Key:", webkey, len(webkey))

    respkey = _calc_respkey(webkey)
    if DEBUG:
        print("respkey:", respkey)

    sock.send(b"""\
HTTP/1.1 101 Switching Protocols\r
Upgrade: websocket\r
Connection: Upgrade\r
Sec-WebSocket-Accept: """)
    sock.send(respkey)
    sock.send(b"\r\n\r\n")

def client_handshake(sock, host, path=b'/', sps=None):
    webkey = bytearray(16)
    for i in range(len(webkey)):
        webkey[i] = urandom.getrandbits(8)
    webkey = binascii.b2a_base64(webkey)[:-1]
    if DEBUG:
        print("Sec-WebSocket-Key:", webkey, len(webkey))

    sock.write(b"""\
GET """)
    sock.write(path)
    sock.write(b""" HTTP/1.1\r
Host: """)
    sock.write(host)
    sock.write(b"""\r
Connection: Upgrade\r
Upgrade: websocket\r
Sec-WebSocket-Key: """)
    sock.write(webkey)
    if sps:
        sock.write(b"""\r
Sec-WebSocket-Protocol: """)
        sock.write(sps)
    sock.write(b"""\r
Sec-WebSocket-Version: 13\r
\r
""")

    l = sock.readline()
    #sys.stdout.write(repr(l))

    respkey, sel_sp = _extract_header_values(sock, (b'sec-websocket-accept', b'sec-websocket-protocol'))
    if not respkey:
        raise OSError("Handshake failed")
    if DEBUG:
        print("respkey:", respkey, len(respkey))

    exp_respkey = _calc_respkey(webkey)
    if DEBUG:
        print("exp_respkey:", exp_respkey, len(exp_respkey))
    if respkey != exp_respkey:
        raise OSError("Handshake failed")

    return sel_sp

def _extract_header_values(sock, dhs):
    dvs = [None] * len(dhs)

    while True:
        l = sock.readline()
        if not l:
            raise OSError("EOF in headers")
        if l == b"\r\n":
            break
        #sys.stdout.write(repr(l))
        h, v = [x.strip() for x in l.split(b":", 1)]
        if DEBUG:
            print((h, v))
        h = h.lower()
        try:
            dvs[dhs.index(h)] = v
        except ValueError:
            pass

    return dvs

def _calc_respkey(webkey):
    d = hashlib.sha1(webkey)
    d.update(b"258EAFA5-E914-47DA-95CA-C5AB0DC85B11")
    respkey = d.digest()
    respkey = binascii.b2a_base64(respkey)[:-1]

    return respkey
