# Test a custom pollable object (without a file descriptor) registered together
# with many file descriptors, so the internal pollfd array is grown and moved.

try:
    import select, io

    select.poll  # Raises AttributeError for CPython implementations without poll()
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

# Check that poll supports registering file descriptors (integers).
try:
    select.poll().register(0)
except OSError:
    print("SKIP")
    raise SystemExit

from micropython import const

_MP_STREAM_POLL = const(3)
_MP_STREAM_GET_FILENO = const(10)

_MP_STREAM_POLL_RD = const(0x0001)


class CustomPollable(io.IOBase):
    def ioctl(self, cmd, arg):
        if cmd == _MP_STREAM_GET_FILENO:
            return -1
        print("CustomPollable.ioctl", cmd, arg)
        if cmd == _MP_STREAM_POLL:
            return _MP_STREAM_POLL_RD & arg


poller = select.poll()
x = CustomPollable()
poller.register(x, select.POLLIN)

# Register many file descriptors, interleaved with other allocations so the
# pollfd array can't grow in place and has to be moved.
fds = range(100, 140)
blocks = []
for fd in fds:
    blocks.append(bytearray(64))
    poller.register(fd, select.POLLIN)
for fd in fds:
    poller.unregister(fd)

# The custom object must still be polled via its ioctl.
poller.modify(x, select.POLLIN | select.POLLOUT)
print([(type(obj), flags) for obj, flags in poller.poll(0)])
