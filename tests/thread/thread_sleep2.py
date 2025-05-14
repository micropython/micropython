# Test accuracy of sleep within a thread.

import time
import _thread


def sleep(t, valid_range):
    t0 = time.time_ns()
    time.sleep(t)
    dt_ms = (time.time_ns() - t0) // 1_000_000
    if dt_ms in valid_range:
        print("dt in range", t)
    else:
        print("dt not in range:", dt_ms)


def thread_entry():
    lock.acquire()
    print("thread start")
    sleep(0.2, range(180, 400))
    print("thread end")


lock = _thread.allocate_lock()
lock.acquire()
_thread.start_new_thread(thread_entry, ())

print("main start")
lock.release()
sleep(0.5, range(480, 800))
print("main end")
