try:
    import uio as io, usocket as socket, uevent
except ImportError:
    print("SKIP")
    raise SystemExit


poller = uevent.poll()

# Simple poll with nothing registered.
poller.poll_ms(1)
print(list(poller))

# Register a socket.
s = socket.socket()
entry = poller.register(s)

# Registering the same stream again should retrieve the same entry.
print(poller.register(s, 0) is entry)

# Load/store attributes of the poll entry.
entry.data = 123
entry.custom_method = lambda self: ("custom_method", self.data)
print(entry.flags, entry.custom_method())

# Unregister socket via poll entry.
entry.unregister()

# Poll a closed socket, should be readable and writable.
entry = poller.register(s, 3)
s.close()
poller.poll_ms(1)
ready = list(poller)
print(len(ready), ready[0].flags)

# Register many sockets to test reallocation of internal array.
poller.register(s)
for _ in range(4):
    poller.register(socket.socket())

# Try registering an object that is not compatible.
try:
    poller.register(io.StringIO())
except OSError:
    print("OSError")
