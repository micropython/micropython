# test raising exception within thread which is not caught
import time
import _thread


def thread_entry():
    raise ValueError


_thread.start_new_thread(thread_entry, ())
time.sleep(1)
print("done")
