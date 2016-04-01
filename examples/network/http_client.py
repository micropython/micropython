try:
    import usocket as _socket
except:
    import _socket


s = _socket.socket()

ai = _socket.getaddrinfo("google.com", 80)
print("Address infos:", ai)
addr = ai[0][4]

print("Connect address:", addr)
s.connect(addr)

if 0:
    # MicroPython rawsocket module supports file interface directly
    s.write("GET / HTTP/1.0\n\n")
    print(s.readall())
else:
    s.send(b"GET / HTTP/1.0\n\n")
    print(s.recv(4096))
