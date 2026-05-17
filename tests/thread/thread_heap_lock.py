# test interaction of micropython.heap_lock with threads

import _thread, micropython

lock1 = _thread.allocate_lock()
lock2 = _thread.allocate_lock()


def thread_entry():
    lock1.acquire()
    print([1, 2, 3])
    lock2.release()


lock1.acquire()
lock2.acquire()

_thread.start_new_thread(thread_entry, ())

micropython.heap_lock()
lock1.release()
lock2.acquire()
micropython.heap_unlock()

lock1.release()
lock2.release()
