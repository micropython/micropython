# HTTP GET sample.

import network, socket

n = network.NRF91()
n.connect()

addr = socket.getaddrinfo("micropython.org", 80)
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)
s.connect(addr[0][-1])
s.send("GET / HTTP/1.1\r\nHost: micropython.org\r\n\r\n")
res = s.recv(1000)
print(res)
