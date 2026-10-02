# Test that a TCP connection with a full window of unread data does not stop the
# network interface from receiving on other connections.
#
# The receive window of a connection must be smaller than the pool of buffers the
# interface receives into, otherwise a single connection whose data is not being read
# can use up all the buffers and then nothing more can be received by any connection.

import socket, time

PORT = 8000


# Server
def instance0():
    multitest.globals(IP=multitest.get_network_ip())
    s = socket.socket()
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(socket.getaddrinfo("0.0.0.0", PORT)[0][-1])
    s.listen(2)
    multitest.next()

    # Accept the data connection, and never read from it.
    s_data, _ = s.accept()

    # Accept the control connection.
    s_ctrl, _ = s.accept()
    s_ctrl.settimeout(3)

    # The client sends on the control connection once the data connection is full.
    try:
        print("server recv", s_ctrl.recv(4))
        s_ctrl.send(b"pong")
    except OSError:
        print("server recv failed")

    multitest.wait("client done")
    s_ctrl.close()
    s_data.close()
    s.close()


# Client
def instance1():
    multitest.next()
    addr = socket.getaddrinfo(IP, PORT)[0][-1]
    s_data = socket.socket()
    s_data.connect(addr)
    s_ctrl = socket.socket()
    s_ctrl.connect(addr)
    s_ctrl.settimeout(3)

    # Send on the data connection until the server's receive window is full, which is
    # when nothing more has been accepted for sending for 1 second.
    s_data.setblocking(False)
    buf = b"0123456789abcdef" * 64
    idle = 0
    while idle < 20:
        try:
            n = s_data.send(buf)
        except OSError:
            n = None
        if n:
            idle = 0
        else:
            idle += 1
            time.sleep(0.05)

    # The server must still be able to receive on the control connection.
    try:
        s_ctrl.send(b"ping")
        print("client recv", s_ctrl.recv(4))
    except OSError:
        print("client recv failed")

    multitest.broadcast("client done")
    s_ctrl.close()
    s_data.close()
