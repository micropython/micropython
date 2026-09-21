# test select.poll on TCP sockets
# Specifically, testing the functionality of socket.MSG_OOB and select.POLLPRI

try:
    import socket, select

    socket.MSG_OOB
    select.poll
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


try:
    # import random
    # port = random.randint(1024, 49151)
    addr = socket.getaddrinfo("127.0.0.1", 8002)[0][-1]

    listener = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    listener.bind(addr)
    listener.listen(1)

    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client.connect(addr)

    server = listener.accept()[0]

    listener.close()
except OSError:
    print("SKIP")
    raise SystemExit


def get_sock(fd):
    # workaround for cpydiff
    # CPython: poll returns list[tuple[int, int]]
    # MicroPython: poll returns list[tuple[socket, int]]
    socks = {
        "listener": listener,
        "client": client,
        "server": server,
    }
    for name, sock in socks.items():
        if fd == sock or fd == sock.fileno():
            return name, sock


def rx_all(p):
    for fd, ev in p.poll(0):
        name, sock = get_sock(fd)
        flags = socket.MSG_OOB if ev == select.POLLPRI else 0
        print(name, ev, sock.recv(1, flags))


poller = select.poll()

print("!PRI !oob")
rx_all(poller)

poller.register(server, select.POLLPRI)

print("PRI !oob")
rx_all(poller)

client.send(b"X", socket.MSG_OOB)

print("PRI oob")
rx_all(poller)

client.send(b"A")

print("!IN pkt")
rx_all(poller)

poller.modify(server, select.POLLIN)

print("IN pkt")
rx_all(poller)

client.close()
server.close()
