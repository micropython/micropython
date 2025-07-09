# Test basic IPv6 socket functionality

try:
    import socket, errno
except ImportError:
    print("SKIP")
    raise SystemExit

# Check if IPv6 is supported
try:
    socket.AF_INET6
except AttributeError:
    print("SKIP")
    raise SystemExit

# Test IPv6 socket creation
s = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)
print("IPv6 TCP socket created")

# Test IPv6 UDP socket creation
u = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM)
print("IPv6 UDP socket created")

# Test binding to IPv6 localhost
try:
    s.bind(socket.getaddrinfo("::1", 0, socket.AF_INET6)[0][-1])
    print("IPv6 TCP bind successful")
except OSError as e:
    print("IPv6 TCP bind failed:", e)

try:
    u.bind(socket.getaddrinfo("::1", 0, socket.AF_INET6)[0][-1])
    print("IPv6 UDP bind successful")
except OSError as e:
    print("IPv6 UDP bind failed:", e)

# Test binding to IPv6 wildcard address
s2 = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)
u2 = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM)

try:
    s2.bind(socket.getaddrinfo("::", 0, socket.AF_INET6)[0][-1])
    print("IPv6 TCP wildcard bind successful")
except OSError as e:
    print("IPv6 TCP wildcard bind failed:", e)

try:
    u2.bind(socket.getaddrinfo("::", 0, socket.AF_INET6)[0][-1])
    print("IPv6 UDP wildcard bind successful")
except OSError as e:
    print("IPv6 UDP wildcard bind failed:", e)

# recv() on a fresh IPv6 socket should raise ENOTCONN
s3 = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)
try:
    s3.recv(1)
except OSError as er:
    print("IPv6 ENOTCONN:", er.errno == errno.ENOTCONN)

# Clean up
s.close()
u.close()
s2.close()
u2.close()
s3.close()
