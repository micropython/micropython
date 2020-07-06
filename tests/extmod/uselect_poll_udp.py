# test select.poll on UDP sockets

try:
    import usocket as socket, uselect as select
except ImportError:
    try:
        import socket, select
    except ImportError:
        print("SKIP")
        raise SystemExit


s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind(socket.getaddrinfo("127.0.0.1", 8000)[0][-1])
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
