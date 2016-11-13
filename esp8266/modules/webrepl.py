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
        try:
            import webrepl_cfg
            _webrepl.password(webrepl_cfg.PASS)
            setup_conn(port, accept_conn)
            print("Started webrepl in normal mode")
        except:
            print("WebREPL is not configured, run 'import webrepl_setup'")
    else:
        _webrepl.password(password)
        setup_conn(port, accept_conn)
        print("Started webrepl in manual override mode")


def start_foreground(port=8266):
    stop()
    s = setup_conn(port, None)
    accept_conn(s)
