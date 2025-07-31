# Test select.poll in combination with file descriptors.

try:
    import select, errno

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

# Register invalid file descriptor.
try:
    select.poll().register(-1)
except ValueError:
    print("ValueError")

# Test polling stdout, it should be writable.
poller = select.poll()
poller.register(1)
poller.modify(1, select.POLLOUT)
print(poller.poll())

# Unregister then re-register.
poller.unregister(1)
poller.register(1, select.POLLIN)

# Poll for input, should return an empty list.
print(poller.poll(0))

# Test registering a very large number of file descriptors (will trigger
# EINVAL due to more than OPEN_MAX fds). Typically it's 1024 (and on GitHub CI
# we force this via `ulimit -n 1024`).
poller = select.poll()
for fd in range(6000):
    poller.register(fd)
try:
    poller.poll()
    assert False
except OSError as er:
    print(er.errno == errno.EINVAL)

# Register stdout/stderr, plus many extra ones to trigger the fd vector
# resizing. Then unregister the excess ones and verify poll still works.
poller = select.poll()
for fd in range(1, 1000):
    poller.register(fd)
for i in range(3, 1000):
    poller.unregister(i)
print(sorted(poller.poll()))
