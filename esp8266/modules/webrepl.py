# This module should be imported from REPL, not run from command line.
import socket
import uos
import network
import websocket
import websocket_helper
import _webrepl

listen_s = None
client_s = None


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
            print("WebREPL daemon started on ws://%s:%d" % (iface.ifconfig()[0], port))
    return listen_s


def accept_conn(listen_sock):
    global client_s
    cl, remote_addr = listen_sock.accept()
    if uos.dupterm():
        print("\nConcurrent WebREPL connection from", remote_addr, "rejected")
        cl.close()
        return
    print("\nWebREPL connection from:", remote_addr)
    client_s = cl
    websocket_helper.server_handshake(cl)
    ws = websocket.websocket(cl, True)
    ws = _webrepl._webrepl(ws)
    cl.setblocking(False)
    # notify REPL on socket incoming data
    cl.setsockopt(socket.SOL_SOCKET, 20, uos.dupterm_notify)
    uos.dupterm(ws)


def stop():
    global listen_s, client_s
    uos.dupterm(None)
    if client_s:
        client_s.close()
    if listen_s:
        listen_s.close()


def start(port=8266, password=None):
    stop()
    if password is None:
        raise ValueError("Password is required")
    else:
        _webrepl.password(password)
        setup_conn(port, accept_conn)
        print("Started webrepl in manual override mode")


def start_foreground(port=8266):
    stop()
    s = setup_conn(port, None)
    accept_conn(s)

def _sanitize_password(password):
    if not password:
            raise ValueError("Password is required")
    # Check length.
    if not 4 <= len(password) <= 8:
        raise ValueError("Password has to be between 4 and 8 characters long")
    # Only allow printable ASCII characters.
    for c in password:
        if not 32 <= ord(c) <= 126:
            raise ValueError("Invalid character in the password")
    # Escape backslashes and quotes.
    password = password.replace('\\', '\\\\').replace("'", "\\'")
    return password

def _change_daemon(enable, password=''):
    with open('./start_webrepl.py', 'w'):
        if enable:
            new_f.write("""\
import webrepl
webrepl.start(password='%s')
""" % password)

def enable(password):
    password = _sanitize_password(password)
    _change_daemon(True, password)
    print("WebREPL daemon auto-start enabled.")

def disable():
    _change_daemon(False)
    print("WebREPL daemon auto-start disabled.")
