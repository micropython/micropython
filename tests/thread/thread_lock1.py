# test _thread lock object using a single thread
#
# MIT license; Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd

import _thread

# create lock
lock = _thread.allocate_lock()

print(type(lock) == _thread.LockType)

# should be unlocked
print(lock.locked())

# basic acquire and release
print(lock.acquire())
print(lock.locked())
lock.release()
print(lock.locked())

# try acquire twice (second should fail)
print(lock.acquire())
print(lock.locked())
print(lock.acquire(0))
print(lock.locked())
lock.release()
print(lock.locked())

# test with capabilities of lock
with lock:
    print(lock.locked())

# test that lock is unlocked if an error is raised
try:
    with lock:
        print(lock.locked())
        raise KeyError
except KeyError:
    print("KeyError")
    print(lock.locked())

# test that we can't release an unlocked lock
try:
    lock.release()
except RuntimeError:
    print("RuntimeError")
