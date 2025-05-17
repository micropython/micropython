# Test UDP recv and recvfrom with MSG_DONTWAIT
import socket

try:
    import errno, time
except ImportError:
    print("SKIP")
    raise SystemExit

PORT = 8000


# Server
def instance0():
    multitest.globals(IP=multitest.get_network_ip())
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(socket.getaddrinfo("0.0.0.0", PORT)[0][-1])
    multitest.next()
    try:
        print(s.recvfrom(8, socket.MSG_DONTWAIT))
    except OSError as e:
        print(e.errno == errno.EAGAIN)
    try:
        print(s.recv(8, socket.MSG_DONTWAIT))
    except OSError as e:
        print(e.errno == errno.EAGAIN)
    multitest.next()
    # during this window, client is sending a packet
    multitest.next()
    time.sleep(1)  # allow for some transmission delay
    print(s.recv(8, socket.MSG_DONTWAIT))
    s.close()


# Client
def instance1():
    multitest.next()
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.connect(socket.getaddrinfo(IP, PORT)[0][-1])
    # during this window, server is performing the failing MSG_DONTWAIT receives
    multitest.next()
    s.send(b"abcdefgh")
    multitest.next()
    s.close()
