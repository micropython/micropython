# This module should be imported from REPL, not run from command line.
import binascii
import hashlib
import network
import os
import socket
import sys
import websocket
import _webrepl

listen_s = None
client_s = None

DEBUG = 0

_DEFAULT_STATIC_HOST = const("https://micropython.org/webrepl/")
static_host = _DEFAULT_STATIC_HOST


def server_handshake(cl):
    req = cl.makefile("rwb", 0)
    # Skip HTTP GET line.
    l = req.readline()
    if DEBUG:
        sys.stdout.write(repr(l))

    webkey = None
    upgrade = False
    websocket = False

    while True:
        l = req.readline()
        if not l:
            # EOF in headers.
            return False
        if l == b"\r\n":
            break
        if DEBUG:
            sys.stdout.write(l)
        h, v = [x.strip() for x in l.split(b":", 1)]
        if DEBUG:
            print((h, v))
        if h == b"Sec-WebSocket-Key":
            webkey = v
        elif h == b"Connection" and b"Upgrade" in v:
            upgrade = True
        elif h == b"Upgrade" and v == b"websocket":
            websocket = True

    if not (upgrade and websocket and webkey):
        return False

    if DEBUG:
        print("Sec-WebSocket-Key:", webkey, len(webkey))

    d = hashlib.sha1(webkey)
    d.update(b"258EAFA5-E914-47DA-95CA-C5AB0DC85B11")
    respkey = d.digest()
    respkey = binascii.b2a_base64(respkey)[:-1]
    if DEBUG:
        print("respkey:", respkey)

    cl.send(
        b"""\
HTTP/1.1 101 Switching Protocols\r
Upgrade: websocket\r
Connection: Upgrade\r
Sec-WebSocket-Accept: """
    )
    cl.send(respkey)
    cl.send("\r\n\r\n")

    return True


def send_html(cl):
    cl.send(
        b"""\
HTTP/1.0 200 OK\r
\r
<base href=\""""
    )
    cl.send(static_host)
    cl.send(
        b"""\"></base>\r
<script src="webrepl_content.js"></script>\r
"""
    )
    cl.close()


def setup_conn(port, accept_handler):
    global listen_s
    listen_s = socket.socket()
    listen_s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    ai = socket.getaddrinfo("0.0.0.0", port)
    addr = ai[0][4]

    listen_s.bind(addr)
    listen_s.listen(1)
    if accept_handler:
        listen_s.setsockopt(socket.SOL_SOCKET, 20, accept_handler)
    for i in (network.AP_IF, network.STA_IF):
        iface = network.WLAN(i)
        if iface.active():
            print("WebREPL server started on http://%s:%d/" % (iface.ifconfig()[0], port))
    return listen_s


def accept_conn(listen_sock):
    global client_s
    cl, remote_addr = listen_sock.accept()

    if not server_handshake(cl):
        send_html(cl)
        return False

    prev = os.dupterm(None)
    os.dupterm(prev)
    if prev:
        print("\nConcurrent WebREPL connection from", remote_addr, "rejected")
        cl.close()
        return False
    print("\nWebREPL connection from:", remote_addr)
    client_s = cl

    ws = websocket.websocket(cl, True)
    ws = _webrepl._webrepl(ws)
    cl.setblocking(False)
    # notify REPL on socket incoming data (ESP32/ESP8266-only)
    if hasattr(os, "dupterm_notify"):
        cl.setsockopt(socket.SOL_SOCKET, 20, os.dupterm_notify)
    os.dupterm(ws)

    return True


def stop():
    global listen_s, client_s
    os.dupterm(None)
    if client_s:
        client_s.close()
    if listen_s:
        listen_s.close()


def start(port=8266, password=None, accept_handler=accept_conn):
    global static_host
    stop()
    webrepl_pass = password
    if webrepl_pass is None:
        try:
            import webrepl_cfg

            webrepl_pass = webrepl_cfg.PASS
            if hasattr(webrepl_cfg, "BASE"):
                static_host = webrepl_cfg.BASE
        except:
            print("WebREPL is not configured, run 'import webrepl_setup'")

    _webrepl.password(webrepl_pass)
    s = setup_conn(port, accept_handler)

    if accept_handler is None:
        print("Starting webrepl in foreground mode")
        # Run accept_conn to serve HTML until we get a websocket connection.
        while not accept_conn(s):
            pass
    elif password is None:
        print("Started webrepl in normal mode")
    else:
        print("Started webrepl in manual override mode")


def start_foreground(port=8266, password=None):
    start(port, password, None)
