# Test that a failed poll.register() leaves the poll object usable.

try:
    import select

    select.poll  # Raises AttributeError for CPython implementations without poll()
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

poller = select.poll()
obj = object()

# Registering a non-stream object must fail.
try:
    poller.register(obj, select.POLLIN)
except OSError:
    print("OSError")

# The failed object must not be registered.
print(poller.poll(0))
try:
    poller.modify(obj, select.POLLOUT)
except OSError:
    print("OSError")
poller.unregister(obj)
print(poller.poll(0))

# Try again, the result must be the same.
try:
    poller.register(obj, select.POLLIN)
except OSError:
    print("OSError")
print(poller.poll(0))
