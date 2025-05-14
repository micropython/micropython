# test select.poll on UDP sockets

try:
    import socket, select

    select.poll  # Raises AttributeError for CPython implementations without poll()
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

try:
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.bind(socket.getaddrinfo("127.0.0.1", 8000)[0][-1])
except OSError:
    print("SKIP")
    raise SystemExit

poll = select.poll()

# UDP socket should not be readable
poll.register(s, select.POLLIN)
print(len(poll.poll(0)))

# UDP socket should be writable
poll.modify(s, select.POLLOUT)
print(poll.poll(0)[0][1] == select.POLLOUT)

# same test for select.select, but just skip it if the function isn't available
if hasattr(select, "select"):
    r, w, e = select.select([s], [], [], 0)
    assert not r and not w and not e
