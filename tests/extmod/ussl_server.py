try:
    import usocket as socket
    import ussl as ssl
except ImportError:
    print("SKIP")
    import sys
    sys.exit()


# server
s_server = socket.socket()
s_server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s_server.bind(socket.getaddrinfo("0.0.0.0", 8443)[0][-1])
s_server.listen(5)

# dummy client
s_client = socket.socket()
s_client.connect(socket.getaddrinfo("0.0.0.0", 8443)[0][-1])
# s_client = ssl.wrap_socket(s_client)

# connect
cl = s_server.accept()[0]
cl = ssl.wrap_socket(cl, server_side=True)

# ping pong
s_client.send(b'ping')
# print(cl.read(4))
cl.write(b'pong')
print(s_client.read(4))

cl.close()
s_client.close()
