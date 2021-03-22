# test raising exception within thread which is not caught
import utime
import _thread


def thread_entry():
    raise ValueError


_thread.start_new_thread(thread_entry, ())
utime.sleep(1)
print("done")
