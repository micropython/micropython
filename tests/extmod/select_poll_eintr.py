# Test interruption of select.poll by EINTR signal, when
# MICROPY_PY_SELECT_POSIX_OPTIMISATIONS is enabled.

try:
    import time, gc, select, socket, _thread

    time.time_ns  # Check for time_ns on MicroPython
    select.poll  # Raises AttributeError for CPython implementations without poll()
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


def thread_main():
    lock.acquire()
    time.sleep(0.2)
    print("thread gc start")
    # The unix gc.collect() implementation will raise EINTR on other threads.
    # Could possibly use _thread._interrupt_main() instead if MicroPython had it.
    gc.collect()
    print("thread gc end")


# Start a thread to interrupt the main thread during its call to poll.
lock = _thread.allocate_lock()
lock.acquire()
_thread.start_new_thread(thread_main, ())

# Use a new UDP socket for tests, which should be writable but not readable.
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind(socket.getaddrinfo("127.0.0.1", 8000)[0][-1])

# Create the poller object.
poller = select.poll()
poller.register(s, select.POLLIN)

# Poll on the UDP socket for a set timeout, which should be reached.
print("poll")
lock.release()
t0 = time.time_ns()
result = poller.poll(400)
dt_ms = (time.time_ns() - t0) / 1e6
print("result:", result)
if 380 <= dt_ms <= 600:
    print("dt in range")
else:
    print("dt not in range:", dt_ms)

# Clean up.
s.close()
