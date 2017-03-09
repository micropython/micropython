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
def ping(msg, sz):
    c.write(msg)
    return ws.read(sz)

# client receives from server
def pong(msg, sz):
    ws.write(msg)
    return c.read(sz)


print(ping(b"\x81\x04ping", 4))
print(ping(b"\x80\x04ping", 4)) # FRAME_CONT
print(pong(b"pong", 6))

# split frames are not supported
# print(ping(b"\x01\x04ping", 4))

# extended payloads
print(ping(b'\x81~\x00\x80' + b'ping' * 32, 128))
print(pong(b"pong" * 32, 132))

# mask
print(ping(b"\x81\x80mask", 1))

# control frames
print(ping(b"\x89\x00", 1)) # FRAME_PING
print(pong(b"\x8a\x00", 4)) # FRAME_PONG
print(ping(b"\x88\x00", 2)) # FRAME_CLOSE
print(c.read(4))

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
