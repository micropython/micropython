# test that having multiple threads block on stdin doesn't cause any issues.
#
# The test doesn't expect any input on stdin.
#
# This is a regression test for https://github.com/micropython/micropython/issues/15230
# on rp2, but doubles as a general property to test across all ports.
import select
import sys
import time
import _thread


class StdinWaiter:
    def __init__(self):
        self._done = False

    def wait_stdin(self, timeout_ms):
        poller = select.poll()
        poller.register(sys.stdin, select.POLLIN)
        poller.poll(timeout_ms)
        # ignoring the poll result as we don't expect any input
        self._done = True

    def is_done(self):
        return self._done


thread_waiter = StdinWaiter()
_thread.start_new_thread(thread_waiter.wait_stdin, (1000,))
StdinWaiter().wait_stdin(1000)

# add a short delay to account for inconsistency waking the two threads (on
# CPython CI runners there seems to be some intermittent difference in thread
# wakeup delays.) On Linux we expect both threads should resume more or less
# concurrently - aka the "thundering herd" effect"
try:
    time.sleep_ms(100)
except AttributeError:
    time.sleep(0.1)

# the background thread should have completed its wait by now
print(thread_waiter.is_done())
