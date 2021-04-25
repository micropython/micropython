# test that socket.accept() on a socket with timeout raises ETIMEDOUT

try:
    import usocket as socket
except:
    import socket

try:
    socket.socket.settimeout
except AttributeError:
    print("SKIP")
    raise SystemExit

s = socket.socket()
s.bind(socket.getaddrinfo("127.0.0.1", 8123)[0][-1])
s.settimeout(1)
s.listen(1)
try:
    s.accept()
except OSError as er:
    print(er.args[0] in (110, "timed out"))  # 110 is ETIMEDOUT; CPython uses a string
s.close()
