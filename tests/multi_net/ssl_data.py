# Simple test creating an SSL connection and transferring some data
# This test won't run under CPython because it requires key/cert

import usocket as socket, ussl as ssl

PORT = 8000


# Server
def instance0():
    multitest.globals(IP=multitest.get_network_ip())
    s = socket.socket()
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(socket.getaddrinfo("0.0.0.0", PORT)[0][-1])
    s.listen(1)
    multitest.next()
    s2, _ = s.accept()
    s2 = ssl.wrap_socket(s2, server_side=True)
    print(s2.read(16))
    s2.write(b"server to client")
    s.close()


# Client
def instance1():
    multitest.next()
    s = socket.socket()
    s.connect(socket.getaddrinfo(IP, PORT)[0][-1])
    s = ssl.wrap_socket(s)
    s.write(b"client to server")
    print(s.read(16))
    s.close()
