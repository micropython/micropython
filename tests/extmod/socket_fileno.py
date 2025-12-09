# Test socket.fileno() functionality

try:
    import socket
except ImportError:
    print("SKIP")
    raise SystemExit

if not hasattr(socket.socket, "fileno"):
    print("SKIP")
    raise SystemExit

s = socket.socket()
print(s.fileno() >= 0)

s.close()
print(s.fileno())  #  should print -1
