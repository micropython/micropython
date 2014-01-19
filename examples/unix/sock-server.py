mod = rawsocket
s = mod.socket()

ai = mod.getaddrinfo("127.0.0.1", 8080)
print("Bind address info:", ai)
addr = ai[0][4]

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
    print(client_s.read(4096))
    #print(client_s.readall())
    client_s.write("""\
HTTP/1.0 200 OK

Hello #{} from MicroPython!
""".format(counter))
    client_s.close()
    counter += 1
