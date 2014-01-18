mod = rawsocket
s = mod.socket()

if 1:
    ai = mod.getaddrinfo("google.com", 80)
    print("Address infos:", ai)
    addr = ai[0][4]
else:
    # Deprecated way to construct connection address
    addr = mod.sockaddr_in()
    addr.sin_family = 2
    #addr.sin_addr = (0x0100 << 16) + 0x007f
    #addr.sin_addr = (0x7f00 << 16) + 0x0001
    #addr.sin_addr = mod.inet_aton("127.0.0.1")
    addr.sin_addr = mod.gethostbyname("google.com")
    addr.sin_port = mod.htons(80)

print("Connect address:", addr)
s.connect(addr)

s.write("GET / HTTP/1.0\n\n")
print(s.readall())
