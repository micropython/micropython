# test that socket.accept() on a non-blocking socket raises EAGAIN

import socket

s = socket.socket()
s.bind(socket.getaddrinfo("127.0.0.1", 8123)[0][-1])
s.setblocking(False)
s.listen(1)
try:
    s.accept()
except OSError as er:
    print(er.errno == 11)  # 11 is EAGAIN
s.close()
