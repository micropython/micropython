# very basic test of ssl module, just to test the methods exist

try:
    import io
    import ssl
except ImportError:
    print("SKIP")
    raise SystemExit


class TestSocket(io.IOBase):
    def write(self, buf):
        return len(buf)

    def readinto(self, buf):
        return 0

    def ioctl(self, cmd, arg):
        print("TestSocket.ioctl", cmd, arg)
        return 0

    def setblocking(self, value):
        print("TestSocket.setblocking({})".format(value))


# create in client mode
try:
    ss = ssl.wrap_socket(TestSocket(), server_hostname="test.example.com")
except OSError as er:
    print("OSError: client")

# create in server mode (can use this object for further tests)
ss = ssl.wrap_socket(TestSocket(), server_side=1, do_handshake=0)

# print
print(ss)

# setblocking() propagates call to the underlying stream object
ss.setblocking(False)
ss.setblocking(True)

# write
try:
    ss.write(b"aaaa")
except OSError:
    pass

# read (underlying socket has no data)
try:
    ss.read(8)
except OSError:
    pass

# close
ss.close()
# close 2nd time
ss.close()

# read on closed socket
try:
    ss.read(10)
except OSError as er:
    print("OSError: read")

# write on closed socket
try:
    ss.write(b"aaaa")
except OSError as er:
    print("OSError: write")
