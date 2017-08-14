# very basic test of ssl module, just to test the methods exist

try:
    import uio as io
    import ussl as ssl
except ImportError:
    print("SKIP")
    import sys
    sys.exit()

# create in client mode
try:
    ss = ssl.wrap_socket(io.BytesIO())
except OSError as er:
    print('wrap_socket:', repr(er))

# create in server mode (can use this object for further tests)
socket = io.BytesIO()
ss = ssl.wrap_socket(socket, server_side=1)

# print
print(repr(ss)[:12])

# setblocking
try:
    ss.setblocking(False)
except NotImplementedError:
    print('setblocking: NotImplementedError')
ss.setblocking(True)

# write
print(ss.write(b'aaaa'))

# read (underlying socket has no data)
print(ss.read(8))

# read (underlying socket has data, but it's bad data)
socket.write(b'aaaaaaaaaaaaaaaa')
socket.seek(0)
try:
    ss.read(8)
except OSError as er:
    print('read:', repr(er))

# close
ss.close()

# write on closed socket
try:
    ss.write(b'aaaa')
except OSError as er:
    print('write:', repr(er))
