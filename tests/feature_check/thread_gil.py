# Test if the target has threading, and whether it has the GIL or not.
#
# This is done by executing a busy loop with the native emitter, which will
# hog the GIL (if the GIL is enabled), not allowing any other code to run.
# Then time how long things take to determine if the GIL is enabled or not.

import time
import _thread

T = 50
T_MARGIN = 25


# This function with the native emitter is atomic with respect to the GIL.
# It will take (at least) T time to run and not release the GIL in that time.
@micropython.native
def busy():
    t0 = time.ticks_ms()
    while time.ticks_diff(time.ticks_ms(), t0) < T:
        pass


def thread():
    global thread_done
    busy()
    thread_done = True


thread_done = False
t0 = time.ticks_ms()
_thread.start_new_thread(thread, ())
busy()
while not thread_done:
    time.sleep(0)
dt = time.ticks_diff(time.ticks_ms(), t0)

if dt < T + T_MARGIN:
    print("thread no GIL")
else:
    print("thread with GIL")
