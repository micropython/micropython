# Test MicroPython's idempotent extension to poll.unregister().
try:
    import usocket as socket, uselect as select, uerrno as errno
except ImportError:
    print("SKIP")
    raise SystemExit


poller = select.poll()

s = socket.socket()

poller.register(s)

print(poller.unregister(s, False))

# Can unregister multiple times
print(poller.unregister(s, False))
print(poller.unregister(s, False))
