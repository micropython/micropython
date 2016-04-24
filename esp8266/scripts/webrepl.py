# This module should be imported from REPL, not run from command line.
import socket
import uos
import websocket
import websocket_helper

listen_s = None
client_s = None

def wait_connection():
    global listen_s, client_s
    listen_s = socket.socket()
    listen_s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    ai = socket.getaddrinfo("0.0.0.0", 8266)
    print("Bind address info:", ai)
    addr = ai[0][4]

    listen_s.bind(addr)
    listen_s.listen(1)
    client_s, remote_addr = listen_s.accept()
    print(client_s)
    return client_s


def start():
    global listen_s, client_s
    uos.dupterm(None)
    if client_s:
        client_s.close()
    if listen_s:
        listen_s.close()
    cl = wait_connection()
    websocket_helper.server_handshake(cl)
    ws = websocket.websocket(cl, True)
    cl.setblocking(False)
    # notify REPL on socket incoming data
    cl.setsockopt(socket.SOL_SOCKET, 20, uos.dupterm_notify)
    uos.dupterm(ws)
    print("Connected")

start()
