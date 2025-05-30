# Test UDP recv and recvfrom with MSG_DONTWAIT
import random
import socket

try:
    import errno, time
except ImportError:
    print("SKIP")
    raise SystemExit


# Server
def instance0():
    PORT = random.randrange(10000, 50000)
    multitest.globals(IP=multitest.get_network_ip(), PORT=PORT)
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(socket.getaddrinfo("0.0.0.0", PORT)[0][-1])
    multitest.next()
    begin = time.ticks_ms()

    # do some recvs before instance1 starts, when we know no packet is waiting
    try:
        print(s.recvfrom(8, socket.MSG_DONTWAIT))
    except OSError as e:
        print(e.errno == errno.EAGAIN)
    try:
        print(s.recv(8, socket.MSG_DONTWAIT))
    except OSError as e:
        print(e.errno == errno.EAGAIN)

    # the above steps should not have taken any substantial time
    elapsed = time.ticks_diff(time.ticks_ms(), begin)
    print(True if elapsed < 50 else elapsed)

    # Now instance1 will send us a UDP packet
    multitest.broadcast("0-ready")
    multitest.wait("1-sent")

    for _ in range(10):  # retry if necessary, to allow for network delay
        time.sleep_ms(100)
        try:
            print(s.recv(8, socket.MSG_DONTWAIT))
            break
        except OSError as er:
            if er.errno != errno.EAGAIN:
                raise er
    s.close()


# Client
def instance1():
    multitest.next()
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.connect(socket.getaddrinfo(IP, PORT)[0][-1])
    multitest.wait("0-ready")
    print(s.send(b"abcdefgh"))
    multitest.broadcast("1-sent")
    s.close()
