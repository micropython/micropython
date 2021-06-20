# stress test for creating many threads

try:
    import utime as time
except ImportError:
    import time
import _thread


def thread_entry(n):
    pass


thread_num = 0
while thread_num < 500:
    try:
        _thread.start_new_thread(thread_entry, (thread_num,))
        thread_num += 1
    except MemoryError:
        pass

# wait for the last threads to terminate
time.sleep(1)
print("done")
