# test _thread.get_ident() function
#
# MIT license; Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd

try:
    import utime as time
except ImportError:
    import time
import _thread

def thread_entry():
    tid = _thread.get_ident()
    print('thread', type(tid) == int, tid != 0, tid != tid_main)

tid_main = _thread.get_ident()
print('main', type(tid_main) == int, tid_main != 0)

_thread.start_new_thread(thread_entry, ())

time.sleep(0.2)
print('done')
