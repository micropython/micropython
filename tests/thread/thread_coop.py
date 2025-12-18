# Threads should be semi-cooperative, to the point where one busy
# thread can't starve out another.
#
# (Note on ports without the GIL this one should always be true, on ports with GIL it's
# a test of the GIL behaviour.)

import _thread
import sys
from time import ticks_ms, ticks_diff, sleep_ms
import micropython


done = False

ITERATIONS = 5
SLEEP_MS = 250
MAX_DELTA = 30

if sys.platform in ("win32", "linux", "darwin"):
    # Conventional operating systems get looser timing restrictions
    SLEEP_MS = 300
    MAX_DELTA = 100


# This function must always use the bytecode emitter so the VM can bounce the GIL when running.
@micropython.bytecode
def busy_thread():
    while not done:
        pass


def test_sleeps():
    global done
    ok = True
    for _ in range(ITERATIONS):
        t0 = ticks_ms()
        sleep_ms(SLEEP_MS)
        t1 = ticks_ms()
        d = ticks_diff(t1, t0)
        if d < SLEEP_MS - MAX_DELTA or d > SLEEP_MS + MAX_DELTA:
            print("Slept too long ", d)
            ok = False
    print("OK" if ok else "Not OK")
    done = True


# make the thread the busy one, and check sleep time on main task
_thread.start_new_thread(busy_thread, ())
test_sleeps()

sleep_ms(100)
done = False

# now swap them
_thread.start_new_thread(test_sleeps, ())
busy_thread()
