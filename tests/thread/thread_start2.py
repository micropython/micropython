# test capability to start a thread with keyword args
#
# MIT license; Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd

try:
    import utime as time
except ImportError:
    import time
import _thread

def thread_entry(a0, a1, a2, a3):
    print('thread', a0, a1, a2, a3)

# spawn thread using kw args
_thread.start_new_thread(thread_entry, (10, 20), {'a2': 0, 'a3': 1})

# wait for thread to finish
time.sleep(1)

# incorrect argument where dictionary is needed for keyword args
try:
    _thread.start_new_thread(thread_entry, (), ())
except TypeError:
    print('TypeError')

print('done')
