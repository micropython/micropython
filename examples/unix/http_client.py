try:
    import usocket as _socket
except:
    import _socket


s = _socket.socket()

if 1:
    ai = _socket.getaddrinfo("google.com", 80)
    print("Address infos:", ai)
    addr = ai[0][4]
else:
    # Deprecated ways to construct connection address
    addr = _socket.sockaddr_in()
    addr.sin_family = 2
    #addr.sin_addr = (0x0100 << 16) + 0x007f
    #addr.sin_addr = (0x7f00 << 16) + 0x0001
    #addr.sin_addr = _socket.inet_aton("127.0.0.1")
    addr.sin_addr = _socket.gethostbyname("google.com")
    addr.sin_port = _socket.htons(80)

print("Connect address:", addr)
s.connect(addr)

if 0:
    # MicroPython rawsocket module supports file interface directly
    s.write("GET / HTTP/1.0\n\n")
    print(s.readall())
else:
    s.send(b"GET / HTTP/1.0\n\n")
    print(s.recv(4096))
