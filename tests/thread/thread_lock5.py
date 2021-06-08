# test _thread lock objects where a lock is acquired/released by a different thread

import _thread


def thread_entry():
    print("thread about to release lock")
    lock.release()


lock = _thread.allocate_lock()
lock.acquire()
_thread.start_new_thread(thread_entry, ())
lock.acquire()
print("main has lock")
lock.release()
