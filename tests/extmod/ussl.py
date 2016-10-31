try:
    import usocket as socket
    import ussl as ssl
except ImportError:
    print("SKIP")
    import sys
    sys.exit()


# client
s = socket.socket()
addr = socket.getaddrinfo("google.com", 443)[0][-1]
s.connect(addr)

ss = ssl.wrap_socket(s)

# check that sock implements stream protocol
# ss = ssl.wrap_socket(42)

sock_str = str(ss)
print(sock_str[:12], sock_str[-1:])

try:
    ss.setblocking(False)
except NotImplementedError:
    print("setblocking: NotImplementedError")
ss.setblocking(True)

# uPy SSLSocket objects implement only stream interface
ss.write(b"GET / HTTP/1.0\r\n\r\n")
print(ss.read(8))

ss.close()

# write on closed or unwrapped SSL socket
# ss.write(b"GET / HTTP/1.0\r\n\r\n")
