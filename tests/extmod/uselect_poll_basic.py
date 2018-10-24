try:
    import usocket as socket, uselect as select, uerrno as errno
except ImportError:
    try:
        import socket, select, errno
        select.poll  # Raises AttributeError for CPython implementations without poll()
    except (ImportError, AttributeError):
        print("SKIP")
        raise SystemExit


poller = select.poll()

s = socket.socket()

poller.register(s)
# https://docs.python.org/3/library/select.html#select.poll.register
# "Registering a file descriptor that’s already registered is not an error,
# and has the same effect as registering the descriptor exactly once."
poller.register(s)

# 2 args are mandatory unlike register()
try:
    poller.modify(s)
except TypeError:
    print("modify:TypeError")

poller.modify(s, select.POLLIN)

poller.unregister(s)

try:
    poller.modify(s, select.POLLIN)
except OSError as e:
    assert e.args[0] == errno.ENOENT
