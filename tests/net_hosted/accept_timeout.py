# test that socket.accept() on a socket with timeout raises ETIMEDOUT

try:
    import uerrno as errno, usocket as socket
except:
    import errno, socket

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
    print(er.errno in (errno.ETIMEDOUT, "timed out"))  # CPython uses a string instead of errno
s.close()
