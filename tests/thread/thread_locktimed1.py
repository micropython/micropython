# test timed _thread lock object using a single thread

import _thread

# create lock
lock = _thread.allocate_lock()

print(type(lock) == _thread.LockType)

# should be unlocked
print(lock.locked())

# try acquire
print(lock.acquire())
print(lock.locked())

# this fail with timeout
print(lock.acquire(1, 0.1))
print(lock.locked())

# this will fail with negative timeout
print(lock.acquire(0, -1))
print(lock.locked())

# these will raise value errors
try:
    print(lock.acquire(0, 1))
except ValueError:
    print("can't specify a timeout for a non-blocking call")

try:
    print(lock.acquire(0, 0))
except ValueError:
    print("can't specify a timeout for a non-blocking call")

print(lock.locked())
lock.release()
print(lock.locked())
