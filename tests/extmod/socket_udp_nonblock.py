# test non-blocking UDP sockets

try:
    import socket, errno
except ImportError:
    print("SKIP")
    raise SystemExit

try:
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.bind(socket.getaddrinfo("127.0.0.1", 8000)[0][-1])
except OSError:
    print("SKIP")
    raise SystemExit

s.settimeout(0)

try:
    s.recv(1)
except OSError as er:
    print("EAGAIN:", er.errno == errno.EAGAIN)
