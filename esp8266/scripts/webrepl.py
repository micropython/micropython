# This module should be imported from REPL, not run from command line.
import socket
import uos
import network
import websocket
import websocket_helper

listen_s = None
client_s = None

def setup_conn(port):
    global listen_s, client_s
    listen_s = socket.socket()
    listen_s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    ai = socket.getaddrinfo("0.0.0.0", port)
    addr = ai[0][4]

    listen_s.bind(addr)
    listen_s.listen(1)
    listen_s.setsockopt(socket.SOL_SOCKET, 20, accept_conn)
    for i in (network.AP_IF, network.STA_IF):
        iface = network.WLAN(i)
        if iface.active():
            print("WebREPL daemon started on %s:%d" % (iface.ifconfig()[0], port))


def accept_conn(listen_sock):
    global client_s
    cl, remote_addr = listen_sock.accept()
    print("\nWebREPL connection from:", remote_addr)
    client_s = cl
    websocket_helper.server_handshake(cl)
    ws = websocket.websocket(cl, True)
    cl.setblocking(False)
    # notify REPL on socket incoming data
    cl.setsockopt(socket.SOL_SOCKET, 20, uos.dupterm_notify)
    uos.dupterm(ws)
    print("WebREPL connected\n>>> ", end="")


def stop():
    global listen_s, client_s
    uos.dupterm(None)
    if client_s:
        client_s.close()
    if listen_s:
        listen_s.close()


def start(port=8266):
    stop()
    setup_conn(port)
