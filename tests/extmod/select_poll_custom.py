# Test custom pollable objects implemented in Python.

try:
    import socket, select, io
except ImportError:
    print("SKIP")
    raise SystemExit

from micropython import const

_MP_STREAM_POLL = const(3)
_MP_STREAM_GET_FILENO = const(10)

_MP_STREAM_POLL_RD = const(0x0001)
_MP_STREAM_POLL_WR = const(0x0004)


def print_poll_output(lst):
    print([(type(obj), flags) for obj, flags in lst])


class CustomPollable(io.IOBase):
    def __init__(self):
        self.poll_state = 0

    def ioctl(self, cmd, arg):
        if cmd == _MP_STREAM_GET_FILENO:
            # Bare-metal ports don't call this ioctl, so don't print it.
            return -1

        print("CustomPollable.ioctl", cmd, arg)
        if cmd == _MP_STREAM_POLL:
            if self.poll_state == "delay_rd":
                self.poll_state = _MP_STREAM_POLL_RD
                return 0
            elif self.poll_state < 0:
                return self.poll_state
            else:
                return self.poll_state & arg


poller = select.poll()

# Use a new UDP socket for tests, which should be writable but not readable.
try:
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.bind(socket.getaddrinfo("127.0.0.1", 8000)[0][-1])
except OSError:
    print("SKIP")
    raise SystemExit

x = CustomPollable()

# Register both a file-descriptor-based object and a custom pure-Python object.
poller.register(s)
poller.register(x)

# Modify the flags for the custom object.
poller.modify(x, select.POLLIN)

# Test polling.
print_poll_output(poller.poll(0))
x.poll_state = _MP_STREAM_POLL_WR
print_poll_output(poller.poll(0))
x.poll_state = _MP_STREAM_POLL_RD
print_poll_output(poller.poll(0))

# The custom object becomes readable only after being polled.
poller.modify(s, select.POLLIN)
x.poll_state = "delay_rd"
print_poll_output(poller.poll())

# The custom object returns an error.
x.poll_state = -1000
try:
    poller.poll(0)
except OSError as er:
    print("OSError", er.errno)

# Register then unregister a socket (a native stream), then test
# that the Python object is still pollable.
s2 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
x.poll_state = _MP_STREAM_POLL_RD
poller.register(s2)
poller.unregister(s2)
print_poll_output(poller.poll())

# Test registering and unregistering multiple times.
for _ in range(2):
    poller.unregister(s)
    poller.unregister(x)
    poller.register(s2)
    poller.register(s, select.POLLIN)
    poller.register(x, select.POLLIN)
    poller.unregister(s2)
    print_poll_output(poller.poll())

# Clean up.
poller.unregister(x)
poller.unregister(s)
s2.close()
s.close()
