try:
    import usocket as socket
except:
    import socket


CONTENT = """\
HTTP/1.0 200 OK

Hello #{} from MicroPython!
"""

s = socket.socket()

ai = socket.getaddrinfo("127.0.0.1", 8080)
print("Bind address info:", ai)
addr = ai[0][4]

s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.bind(addr)
s.listen(5)
print("Listening, connect your browser to http://127.0.0.1:8080/")

counter = 0
while True:
    res = s.accept()
    client_s = res[0]
    client_addr = res[1]
    print("Client address:", client_addr)
    print("Client socket:", client_s)
    print("Request:")
    if 0:
        # MicroPython rawsocket module supports file interface directly
        print(client_s.read(4096))
        #print(client_s.readall())
        client_s.write(CONTENT.format(counter))
    else:
        print(client_s.recv(4096))
        client_s.send(bytes(CONTENT.format(counter), "ascii"))
    client_s.close()
    counter += 1
