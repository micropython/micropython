# Test select.ipoll().

try:
    import socket, select
except ImportError:
    print("SKIP")
    raise SystemExit


def print_poll_output(lst):
    print([(type(obj), flags) for obj, flags in lst])


poller = select.poll()

# Use a new UDP socket for tests, which should be writable but not readable.
try:
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.bind(socket.getaddrinfo("127.0.0.1", 8000)[0][-1])
except OSError:
    print("SKIP")
    raise SystemExit

poller.register(s)

# Basic polling.
print_poll_output(poller.ipoll(0))

# Pass in flags=1 for one-shot behaviour.
print_poll_output(poller.ipoll(0, 1))

# Socket should be deregistered and poll should return nothing.
print_poll_output(poller.ipoll(0))

# Create a second socket.
s2 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s2.bind(socket.getaddrinfo("127.0.0.1", 8001)[0][-1])

# Register both sockets (to reset the first one).
poller.register(s)
poller.register(s2)

# Basic polling with two sockets.
print_poll_output(poller.ipoll(0))

# Unregister the first socket, to test polling the remaining one.
poller.unregister(s)
print_poll_output(poller.ipoll(0))

# Unregister the second socket, to test polling none.
poller.unregister(s2)
print_poll_output(poller.ipoll(0))

s2.close()
s.close()
