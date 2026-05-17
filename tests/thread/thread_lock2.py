# test _thread lock objects with multiple threads
#
# MIT license; Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd

import time
import _thread

lock = _thread.allocate_lock()


def thread_entry():
    lock.acquire()
    print("have it")
    lock.release()


# spawn the threads
for i in range(4):
    _thread.start_new_thread(thread_entry, ())

# wait for threads to finish
time.sleep(1)
print("done")
