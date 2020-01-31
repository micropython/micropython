# test raising exception within thread which is not caught
try:
    import utime as time
except ImportError:
    import time
import _thread

def thread_entry():
    raise ValueError

_thread.start_new_thread(thread_entry, ())
time.sleep(1)
print('done')
