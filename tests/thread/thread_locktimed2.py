# test timed _thread lock object using multiple threads
#
# MIT license; Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd
try:
    import utime as time
except ImportError:
    import time
import _thread

# create lock
lock = _thread.allocate_lock()

def thread_entry():
    lock.acquire()
    time.sleep(1)
    print('have it')
    lock.release()

# spawn the threads
for i in range(4):
    _thread.start_new_thread(thread_entry, ())

# wait for threads to start
time.sleep(.1)

# will timeout and roughly measure time
start = time.time()
lock.acquire(1, 2.5)
stop = time.time()
print((stop - start) > 2)
print((stop - start) < 3)

# wait for threads to finish
lock.acquire()
print('done')