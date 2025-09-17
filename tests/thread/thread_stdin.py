# Test that having multiple threads block on stdin doesn't cause any issues.
#
# The test doesn't expect any input on stdin.
#
# This is a regression test for https://github.com/micropython/micropython/issues/15230
# on rp2, but doubles as a general property to test across all ports.
import sys
import time
import _thread

try:
    import select
except ImportError:
    print("SKIP")
    raise SystemExit


class StdinWaiter:
    def __init__(self):
        self._done = False

    def wait_stdin(self, timeout_ms):
        poller = select.poll()
        poller.register(sys.stdin, select.POLLIN)
        poller.poll(timeout_ms)
        # Ignoring the poll result as we don't expect any input
        self._done = True

    def is_done(self):
        return self._done


thread_waiter = StdinWaiter()
_thread.start_new_thread(thread_waiter.wait_stdin, (1000,))
StdinWaiter().wait_stdin(1000)

# Spinning here is mostly not necessary but there is some inconsistency waking
# the two threads, especially on CPython CI runners where the thread may not
# have run yet. The actual delay is <20ms but spinning here instead of
# sleep(0.1) means the test can run on MP builds without float support.
while not thread_waiter.is_done():
    time.sleep(0)

# The background thread should have completed its wait by now.
print(thread_waiter.is_done())
