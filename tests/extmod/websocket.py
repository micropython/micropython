try:
    import usocket as socket
    import websocket
    import uerrno
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
    sz = len(msg)
    c.write(msg)
    return ws.read(sz-2 if sz < 126 else sz-4)

# client receives from server
def pong(msg):
    sz = len(msg)
    ws.write(msg)
    return c.read(sz+2 if sz < 126 else sz+4)


print(ping(b"\x81\x04ping"))
print(pong(b"pong"))

# double header size
print(ping(b'\x81~\x00\x80' + b'ping' * 32))
print(pong(b"pong" * 32))

# ioctl
print(ws.ioctl(8)) # GET_DATA_OPTS
print(ws.ioctl(9, 2)) # SET_DATA_OPTS
print(ws.ioctl(9))
try:
    ws.ioctl(-1)
except OSError as e:
    print("EINVAL:", e.args[0] == uerrno.EINVAL)

c.close()
ws.close()
