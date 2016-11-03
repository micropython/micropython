try:
    import usocket as socket
    import websocket
except ImportError:
    import sys
    print("SKIP")
    sys.exit()


# server
s = socket.socket()
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.bind(socket.getaddrinfo("0.0.0.0", 8433)[0][4])
s.listen(1)

# echo dummy socket
c = socket.socket()
c.connect(socket.getaddrinfo("0.0.0.0", 8433)[0][-1])

# start
ws = s.accept()[0]
ws = websocket.websocket(ws, True) # blocking write
print(ws)

# server receives from client
def ping(msg):
    c.write(msg)
    return ws.read(len(msg)-2)

# client receives from server
def pong(msg):
    ws.write(msg)
    return c.read(len(msg)+2)


print(ping(b"\x81\x04ping"))
print(pong(b"pong"))

c.close()
ws.close()
