# stress test for creating many threads

try:
    import utime

    sleep_ms = utime.sleep_ms
except ImportError:
    import time

    sleep_ms = lambda t: time.sleep(t / 1000)
import _thread


def thread_entry(n):
    pass


thread_num = 0
while thread_num < 500:
    try:
        _thread.start_new_thread(thread_entry, (thread_num,))
        thread_num += 1
    except (MemoryError, OSError) as er:
        # Cannot create a new thead at this stage, yield for a bit to
        # let existing threads run to completion and free up resources.
        sleep_ms(50)

# wait for the last threads to terminate
sleep_ms(500)
print("done")
