import sys
import socket
import time

from websocket import *
import websocket_helper


def setup_server():
    s = socket.socket()
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    ai = socket.getaddrinfo("0.0.0.0", 8266)
    addr = ai[0][4]

    s.bind(addr)
    s.listen(1)
    return s

def getpass(stream, prompt):
    stream.write(prompt)
    passwd = b""
    while 1:
        c = stream.read(1)
        if c in (b"\r", b"\n"):
            stream.write("\r\n")
            return passwd
        passwd += c
        stream.write("*")

def handle_conn(listen_sock):
    cl, remote_addr = listen_sock.accept()

    print("""

First-time WebREPL connection has been received. WebREPL initial setup
will now start over this connection. During setup, UART REPL will be
non-responsive. After setup finishes, the board will be rebooted. In
case of error during setup, current session will continue.

If you receive this message unexpectedly, it may mean that your WebREPL
connection is being hacked (power off board if unsure).
""")

    websocket_helper.server_handshake(cl)
    ws = websocket(cl)

    ws.write("""\
Welcome to MicroPython WebREPL!\r
\r
This is the first time you connect to WebREPL, so please set a password\r
to use for the following WebREPL sessions. Once you enter the password\r
twice, your board will reboot with WebREPL running in active mode. On\r
some boards, you may need to press reset button or reconnect power.\r
\r
""")

    while 1:
        passwd1 = getpass(ws, "New password: ")
        if len(passwd1) < 4:
            ws.write("Password too short\r\n")
            continue
        elif len(passwd1) > 9:
            ws.write("Password too long\r\n")
            continue
        passwd2 = getpass(ws, "Confirm password: ")
        if passwd1 == passwd2:
            break
        ws.write("Passwords do not match\r\n")

    with open("port_config.py", "w") as f:
        f.write("WEBREPL_PASS = %r\n" % passwd1.decode("ascii"))

    ws.write("Password successfully set, restarting...\r\n")
    cl.close()
    time.sleep(2)
    import machine
    machine.reset()


def test():
    s = setup_server()
    handle_conn(s)
